/******************************************************************************
Copyright (c) 2015, Geoffrey TOURON
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dusty nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include "lightthreadpool.hpp"

void				*Lightthreadpool::_runthrd(void *data)
{
	Worker			*wrkr = (Worker *)data;
	Lightthreadpool	*ltp = wrkr->ltp;

	while (ltp->_running)
	{
		if (wrkr->front != wrkr->back)
		{
			wrkr->tasks[wrkr->back].function(wrkr->tasks[wrkr->back].data);
			wrkr->back = (wrkr->back + 1) % ltp->_queuesize;
			
		}
		else
		{
			ltp->_mtx.lock();
			ltp->_mcv.wake();
			ltp->_scv.sleep(ltp->_mtx);
			ltp->_mtx.unlock();
		}
	}
	return (0);
}

Lightthreadpool::Lightthreadpool(unsigned int n, unsigned int s) : _thrdnbr(n), _queuesize(s), _looper(0), _running(true)
{
	_wrkrs = new Worker[_thrdnbr];

	for (unsigned int i = 0; i < _thrdnbr; ++i)
	{
		_wrkrs[i].tasks = new Task[_queuesize];
		_wrkrs[i].front = 0;
		_wrkrs[i].back = 0;
		_wrkrs[i].thrd.create(Lightthreadpool::_runthrd, _wrkrs + i);
	}
}

Lightthreadpool::~Lightthreadpool()
{
	_running = false;
	for (unsigned int i = 0; i < _thrdnbr; ++i)
	{
		_wrkrs[i].thrd.wait();
		delete [] _wrkrs[i].tasks;
	}
	delete [] _wrkrs;
}

void	Lightthreadpool::addtask(void *(*function)(void*), void *data)
{
	if ((_wrkrs[_looper].front + 1) % _queuesize != _wrkrs[_looper].back)
	{
		_wrkrs[_looper].tasks[_wrkrs[_looper].front].function = function;
		_wrkrs[_looper].tasks[_wrkrs[_looper].front].data = data;
		_wrkrs[_looper].front = (_wrkrs[_looper].front + 1) % _queuesize;
	}
	else
		run();
	_looper = (_looper + 1) % _thrdnbr;
}

void	Lightthreadpool::run()
{
	bool	done = false;

	_mtx.lock();
	_scv.wakeall();
	while (!done)
	{
		done = true;
		for (unsigned int i = 0; i < _thrdnbr; ++i)
			done = done && (_wrkrs[i].front == _wrkrs[i].back);
		if (!done)
			_mcv.sleep(_mtx);
	}
	_mtx.unlock();
}
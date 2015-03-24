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

#ifndef LIGHTTHREADPOOL_H_
#define LIGHTTHREADPOOL_H_

#include "thread.hpp"
#include "mutex.hpp"
#include "conditionvariable.hpp"

class	Lightthreadpool
{
	public:

		struct		Task
		{
			void	*(*function)(void*);
			void	*data;
		};

		struct				Worker
		{
			Thread			thrd;
			Lightthreadpool	*ltp;
			Task			*tasks;
			unsigned int	front;
			unsigned int	back;
		};


		static	void		*_runthrd(void *);


		unsigned int		_thrdnbr;
		unsigned int		_queuesize;
		unsigned int		_looper;
		bool				_running;

		Worker				*_wrkrs;

		Mutex				_mtx;
		Conditionvariable	_mcv;
		Conditionvariable	_scv;


		Lightthreadpool(unsigned int thrdnbr, unsigned int queuesize);
		~Lightthreadpool();

		void	addtask(void *(*function)(void*), void *data);
		void	run();
};

#endif
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

#include "math/vec_util.hpp"
#include "actor.hpp"
//#include "body.hpp"
#include "physicengine.hpp"

Physicengine::Physicengine() : _sbbsize(4096), _sbbnbr(0), _staticbb(0), _dbbsize(4096), _dbbnbr(0), _dynamicbb(0), _prxsize(_sbbsize + _dbbsize), _prxnbr(0), _sortedprx(0), _faxis(0), _saxis(1), _taxis(2)
{
	_staticbb = new Boundingbox[_sbbsize];
	_dynamicbb = new Boundingbox[_dbbsize];
	_sortedprx = new Proxy[_prxsize];
}

Physicengine::~Physicengine()
{

}

void		Physicengine::add(Body *bd)
{
	//new BB and insert
}

void		Physicengine::remove(Body *bd)
{
	//remove BB
}

void		Physicengine::tick(float delta)
{	
	for (unsigned int i = 0; i < _dbbnbr; ++i)
	{
		_dynamicbb[i].loc = _dynamicbb[i].nextloc;
		_dynamicbb[i].spd = _dynamicbb[i].nextspd;
		_dynamicbb[i].nextloc = _dynamicbb[i].loc + _dynamicbb[i].spd * delta + _dynamicbb[i].acc * delta * delta * 0.5f; // + bd->ping
		_dynamicbb[i].nextspd = _dynamicbb[i].spd + _dynamicbb[i].acc * delta;
	}
	for (unsigned int i = 0; i < _prxnbr; ++i)
	{
		if (_sortedprx[i].dynamic)
		{
			_sortedprx[i].bot = min(_sortedprx[i].bb->loc, _sortedprx[i].bb->nextloc);
			_sortedprx[i].top = max(_sortedprx[i].bb->loc, _sortedprx[i].bb->nextloc + _sortedprx[i].bb->size);
		}
	}

	_insertion_sort();
	_check_overlap(delta);
}

void				Physicengine::_insertion_sort()
{
	unsigned int	i;
	unsigned int	j;
	Proxy			a;

	for (unsigned int j = 1; j < _prxnbr; ++j)
	{
		a = _sortedprx[j];
		for (i = j; i && _sortedprx[i - 1].bot[_faxis] > a.bot[_faxis]; --i)
			_sortedprx[i] = _sortedprx[i + 1];
		_sortedprx[i] = a;
	}
}

void		Physicengine::_check_overlap(float delta)
{
	for (unsigned int j = 0; j < _prxnbr; ++j)
	{
		for (unsigned int i = j + 1; i < _prxnbr; ++i)
		{
			if (_sortedprx[i].bot[_faxis] < _sortedprx[j].top[_faxis])
			{
				if (_sortedprx[i].dynamic || _sortedprx[j].dynamic)
				{
					if (_sortedprx[i].top[_saxis] > _sortedprx[j].bot[_saxis] && _sortedprx[j].top[_saxis] > _sortedprx[i].bot[_saxis]
						&& _sortedprx[i].top[_taxis] > _sortedprx[j].bot[_taxis] && _sortedprx[j].top[_taxis] > _sortedprx[i].bot[_taxis])
						_collide(delta, _sortedprx[j].bb, _sortedprx[i].bb);
				}
			}
			else
				break;
		}
	}
}

void				Physicengine::_collide2(float delta, Boundingbox *x, Boundingbox *y)
{
	vec<float, 3>	u;
	vec<float, 3>	v;
	
	vec<float, 3>	d;
	vec<float, 3>	e;
	
	d = 2.0f * (x->acc - y->acc) * (x->loc - (y->loc + y->size));
	if (d >= 0.0f)
	{
	
	}
	else
	{
		d = 2.0f * (y->acc - x->acc) * (y->loc - (x->loc + x->size));
	}
	
	u = (-x->spd + sqrt(d)) / (2.0f * x->acc);
	v = (-x->spd - sqrt(d)) / (2.0f * x->acc);
	w
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (d[i] > 0.0f)
		{
		
		}
		if (u[i] >= 0.0f && u[i] <= delta)
			t = v[i] >= 0.0f && v[i] < u[i] ? v[i] : u[i];
		else if (v[i] >= 0.0f && v[i] <= delta)
			t = v[i];
		if (t >= 0.0f && _reaction(x, y, t, i))
			break;
			
		float t = -1.0f;
	}
}

void				Physicengine::_collide(float delta, Boundingbox *x, Boundingbox *y)
{
	vec<float, 2>	u;
	vec<float, 2>	v;

	u = (y->loc - (x->loc + x->size)) / (x->spd - y->spd);
	v = (x->loc - (y->loc + y->size)) / (y->spd - x->spd);
	for (unsigned int i = 0; i < 2; ++i)
	{
		float t = -1.0f;
		if (u[i] >= 0.0f && u[i] <= delta)
			t = v[i] >= 0.0f && v[i] < u[i] ? v[i] : u[i];
		else if (v[i] >= 0.0f && v[i] <= delta)
			t = v[i];
		if (t >= 0.0f && _reaction(x, y, t, i))
			break;
	}
}

bool				Physicengine::_reaction(Boundingbox *x, Boundingbox *y, float t, unsigned int axis)
{
	vec<float, 2>	xloc;
	vec<float, 2>	yloc;

	xloc = x->loc + x->spd * t;
	yloc = y->loc + y->spd * t;
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (i != axis && (xloc[i] > yloc[i] + y->size[i] || yloc[i] > xloc[i] + x->size[i]))
			return (false);
	}
	if (x->actor->collide(*y->actor) && x->dynamic)
		x->nextloc[axis] = xloc[axis];
	if (y->actor->collide(*x->actor) && y->dynamic)
		y->nextloc[axis] = yloc[axis];
	return (true);
}
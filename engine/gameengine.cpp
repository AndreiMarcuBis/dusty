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

#include "new.hpp"
#include "actor.hpp"
#include "replication.hpp"
#include "controller.hpp"
#include "factory.hpp"
#include "configmanager.hpp"
#include "console.hpp"
#include "physicengine.hpp"
#include "networkengine.hpp"
#include "raytracer.hpp"
#include "eventmanager.hpp"
#include "callbackmanager.hpp"
#include "gameengine.hpp"

#include <iostream>

Gameengine::Gameengine(Gameengine::Option const &opt) : master(opt.master), _actsize(1024), _actors(0), callback(0), physic(0), network(0), console(0), event(0), graphic(0), controllerclass()
{
	Factory::get_instance().generate_type();

	_actors = new Actor*[_actsize];
	for (unsigned int i = 0; i < _actsize; ++i)
		_actors[i] = 0;
	
	callback = new Callbackmanager();
	physic = new Physicengine();

	if (!opt.local)
	{
		network = new Networkengine(this);
		network->connect(opt.ip, opt.port);
	}

	
	Df_node	*nd = Configmanager::get_instance().get("game.df");
	
	console = new Console(this);
	if (opt.graphic)
	{
		graphic = new Raytracer(1024, 768);
		event = new Eventmanager(this);
	}

	controllerclass = nd->safe_get("controller", Df_node::STRING, 1)->cstr[0];

	if (master)
		create("World", 0, true);
	if (!network)
	{
		Controller *ctrl = (Controller *)create(controllerclass, 0, true);
		_controllermap[ctrl->id] = ctrl;
		ctrl->bind();
	}
}

Gameengine::~Gameengine()
{
	for (unsigned int i = 0; i < _actsize; ++i)
		if (_actors[i])
			delete _actors[i];
	delete [] _actors;

	delete callback;
	delete physic;

	if (network)
		delete network;
	
	
	if (graphic)
	{
		delete event;
		delete graphic;
	}
	delete console;
}

Actor		*Gameengine::create(std::string const &name, Actor const *owner, bool const need_replication)
{
	int		id = -1;

	for (unsigned int i = 0; i < _actsize; ++i)
	{
		if (!_actors[i])
		{
			id = i;
			break;
		}
	}
	if (id < 0)
	{
		id = _actsize;
		_actors = resize(_actors, _actsize, _actsize << 1);
		_actsize <<= 1;
		for (unsigned int i = _actsize >> 1; i < _actsize; ++i)
			_actors[i] = 0;
	}
	_actors[id] = Factory::get_instance().create(this, (need_replication && network ? network->new_replication(id) : 0), id, name, owner);
	return (_actors[id]);
}

Actor		*Gameengine::create(Replication *r)
{
	_actors[r->id] = Factory::get_instance().create(this, r);
	return (_actors[r->id]);
}

Actor	*Gameengine::find_actor(int const id)
{
	return (id < (int)_actsize ? _actors[id] : 0);
}

void	Gameengine::notify_owner(Actor *a, bool const l)
{
	if (a->ownerid < (int)_actsize && _actors[a->ownerid])
		_actors[a->ownerid]->notified_by_owned(a, l);
}

void	Gameengine::notify_owned(Actor *a, bool const l)
{
	for	(unsigned int i = 0; i < _actsize; ++i)
	{
		if (_actors[i]->ownerid == a->id)
			_actors[i]->notified_by_owner(a, l);
	}
}

void			Gameengine::control(int const id)
{
	Controller	*ctrl;

	if ((ctrl = (Controller *)find_actor(id)))
		ctrl->bind();
	else
		_controllermap[id] = ctrl;
}

void				Gameengine::tick(float const delta)
{
	//std::cout << "em" << std::endl;
	if (event)
		event->event();
	//std::cout << "cl" << std::endl;
	console->tick(delta);
	//std::cout << "ne" << std::endl;
	if (network)
		network->tick(delta);
	//std::cout << "pe" << std::endl;
	physic->tick(delta);
	//std::cout << "ac" << std::endl;
	callback->tick(delta);
	
	for (unsigned int i = 0; i < _actsize; ++i)
	{
		if (_actors[i])
		{
			switch (_actors[i]->state)
			{
				case Actor::CREATED:
					_actors[i]->postinstanciation();
				case Actor::OK:
					_actors[i]->tick(delta);
					break;
				case Actor::DESTROYED:
					delete _actors[i];
					_actors[i] = 0;
					break;
			};
		}
	}

	if (graphic)
		graphic->tick(delta);
	//std::cout << "end" << std::endl;
}

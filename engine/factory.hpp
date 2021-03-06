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

#ifndef FACTORY_H_
#define FACTORY_H_

#include "hash/crc.hpp"

#define FACTORYREG(name) Factoryregister<name> const	reg_##name(CRC32(#name))

class	Entity;
class	Gameengine;
class	Replication;

class	Factory
{
	public:

		typedef Entity *(*CF)(Gameengine *, Replication *, int const, short int const, Entity const *);

		struct	Class
		{
			unsigned int	hash;
			CF				cf;
		};


		static Factory	&get_instance();


		unsigned int	_classcount;
		unsigned int	_classsize;
		Class			*_classes;


		Factory();
		~Factory();
		
		void		register_class(unsigned int const, CF);

		Entity		*create(Gameengine *, Replication *) const;
		Entity		*create(Gameengine *, Replication *, int const, unsigned int const, Entity const *) const;
};

template<class T>
Entity	*create(Gameengine *am, Replication *r, int const i, short int const t, Entity const *o)
{
	return (new T(am, r, i, t, o));
}

template<class T>
struct	Factoryregister
{
	Factoryregister(unsigned int const hash)
	{
		Factory::get_instance().register_class(hash, &create<T>);
	}
};

#endif

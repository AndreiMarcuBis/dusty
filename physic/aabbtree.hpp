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

#ifndef AABBTREE_H_
#define AABBTREE_H_

#include "math/vec.hpp"
#include "aabb.hpp"

struct		Node
{
	union
	{
		int	parent;
		int	next;
	};
	
	union
	{
		struct
		{
			int	left;
			int	right;
		};
		//unsigned int	data;	// id of the data
	};
	
	unsigned int	data;

	Aabb	aabb;
	int		height;
};

class	Aabbtree
{
	public:

		unsigned int	_nsize;
		Node			*_nodes;
		int				*_nstack;

		int				_root;
		int				_free;


		Aabbtree();
		~Aabbtree();

		int		add_aabb(Aabb const &, int const data);
		void	remove_aabb(int const);
		bool	move_aabb(int const, Aabb const &);
		bool	move_aabb(int const, Aabb const &, vec<float, 3> const &);

		template<class T>
		void	query(Aabb const &, T *, void (T::*)(int const));
		void	raycast() const;

		int		_allocate_node();
		void	_free_node(int const);

		void	_insert_leaf(int const);
		void	_remove_leaf(int const);

		void	_balance(int const);
		void	_rotate(int const, int const, int const);
};

template<class T>
void	Aabbtree::query(Aabb const &aabb, T* object, void (T::*callback)(int const))
{
	int	top = 0;
	
	_nstack[top++] = _root;
	while (top > 0)
	{
		int	index = _nstack[--top];
		if (index == -1)
			continue;

		if (_nodes[index].aabb.bottom <= aabb.top && _nodes[index].aabb.top >= aabb.bottom)
		{
			if (_nodes[index].right == -1)
				(object->*callback)(index);
			else
			{
				_nstack[top++] = _nodes[index].left;
				_nstack[top++] = _nodes[index].right;
			}
		}
	}
}

#endif

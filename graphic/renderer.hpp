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

#ifndef RENDERE_H_
#define RENDERE_H_

#include <CL/cl.h>
#include <SDL.h>

class	Graphicengine;

class	Renderer
{
	public:

		SDL_Window			*window;
		SDL_Renderer		*renderer;
		SDL_Texture			*texture;

		cl_context			_context;
		cl_command_queue	_queue;
		cl_program			_program;
		cl_kernel			_kernel;
		
		unsigned int		_nodes_mem_size;
		unsigned int		_materials_mem_size;
		unsigned int		_lights_mem_size;
		
		cl_mem				_nodes_mem;
		cl_mem				_materials_mem;
		cl_mem				_lights_mem;
		cl_mem				_image_mem;


		Renderer(unsigned int const, unsigned int const);
		~Renderer();

		void	set_resolution(unsigned int const, unsigned int const);

		void	render(Graphicengine const *);

		void	_set_buffer(Graphicengine const *);
};

#endif
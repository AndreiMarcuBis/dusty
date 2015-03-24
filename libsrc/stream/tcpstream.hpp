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

#ifndef TCP_STREAM_H_
#define TCP_STREAM_H_

#include "socket.hpp"
#include "stream.hpp"

class	Tcp_server
{
	public:

		Socket	_id;


		Tcp_server();
		Tcp_server(char const *port, bool ipv6);
		~Tcp_server();

		bool	operator()();
		bool	operator()(char const *port, bool ipv6);

		bool	is_good() const;
};

class	Tcpstream : public Stream
{
	public:

		Socket	_id;


		Tcpstream();
		Tcpstream(Tcp_server &srv, char *ip, char *port);
		Tcpstream(char const *ip, char const *port);
		~Tcpstream();

		bool	operator()();
		bool	operator()(Tcp_server &, char *ip, char *port);
		bool	operator()(char const *ip, char const *port);

		bool	is_good() const;

		int		read(unsigned int size, void *data);
		int		write(unsigned int size, void const *data);
};

#endif
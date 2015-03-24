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

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "stream/selector.hpp"
#include "stream/tcpstream.hpp"
#include "stream/udpstream.hpp"
#include "networkproto.hpp"

class	Messagequeue;

class	Server
{
	public:

		struct	Client
		{
			int			id;
			char		ip[IP_STRSIZE];
			char		port[PORT_STRSIZE];
			Tcpstream	*tcp;
			int			udpid;
			Ping		*ping;
		};


		static float const	pingrate;
		static float const	timeout;


		Messagequeue		*mq;

		Selector			_slctr;
		Tcp_server			_tcpsrv;
		Udp_server			_udpsrv;
		std::list<Client>	_cltlist;
		int					_currentid;

		Server(Messagequeue *, std::string const &);
		~Server();

		void	tick(float);

		void	_addclient();
		void	_pingclient(float);
		bool	_comtcpclient(std::list<Client>::iterator &);
		void	_receivepacket();
		void	_sendpacket();
		void	_delclient(std::list<Client>::iterator	&);
};

#endif
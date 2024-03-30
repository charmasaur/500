/*
	A client connection stores the information required by the server to represent a client.
*/
#ifndef CLIENTCON_H_
#define CLIENTCON_H_

#include "../constants.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
class cClientCon {
	private:
		int					socket;
		sockaddr_storage	addr;
		socklen_t			addrLen;
	public:
		cClientCon();
		~cClientCon();

		sockaddr_storage* 	getAddr() { return &addr; }
		socklen_t*			getLen() { return &addrLen; }
		int					getSocket() { return socket; }
		void				setSocket(int a) { socket=a; }
};
#endif

// socket.cpp

#ifdef WIN32
#include <windows.h>
#else
#include <stdafx.h>
#include <stdarg.h>
#endif

#include <stdio.h>
#include "socket.h"

//#include "mmgr.h"

//////////////////////////////////////////////////////////////////////////

WinsockInit::WinsockInit(WORD wVersionRequested)
{
	m_iStatus = WSAStartup(wVersionRequested, &m_wsd);
}

WinsockInit::~WinsockInit()
{
	WSACleanup();
}

//////////////////////////////////////////////////////////////////////////

InetAddr::InetAddr(WORD wPort)
	{
	memset(this, 0, sizeof(sockaddr_in));
	sin_family = AF_INET;
	sin_addr.s_addr = htonl(INADDR_ANY);
	sin_port = htons((u_short)wPort);
	}

InetAddr::InetAddr(LPCTSTR lpszAddress, WORD wPort)
	{
	Resolve(lpszAddress, wPort);
	}

InetAddr& InetAddr::operator = (LPCTSTR lpszAddress)
	{
	Resolve(lpszAddress);
	return *this;
	}

void InetAddr::Resolve(LPCTSTR lpszAddress, WORD wPort)
	{
	memset(this, 0, sizeof(sockaddr_in));
	sin_family = AF_INET;
	sin_addr.s_addr = inet_addr((LPTSTR)lpszAddress);
	if( sin_addr.s_addr == INADDR_NONE && strcmp((LPTSTR)lpszAddress, "255.255.255.255")!=0 )
		{
		HOSTENT* lphost = gethostbyname((LPTSTR)lpszAddress);
		if( lphost )
			sin_addr.s_addr = ((IN_ADDR*)lphost->h_addr)->s_addr;
		else
			sin_addr.s_addr = INADDR_ANY;
		}
	sin_port = htons((u_short)wPort);
	}

//////////////////////////////////////////////////////////////////////////

Socket::Socket()
	: m_sock(INVALID_SOCKET), m_bOwnSocket(false)
	{
	}

Socket::Socket(const Socket& s)
	: m_sock(s.m_sock), m_bOwnSocket(false)
	{
	}

Socket::Socket(SOCKET s)
	: m_sock(s), m_bOwnSocket(false)
	{
	}

Socket::~Socket()
	{
	if( m_bOwnSocket && m_sock != INVALID_SOCKET )
		Close();
	}

bool Socket::Create()
	{
//	_ASSERT(m_sock == INVALID_SOCKET);
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	return (m_bOwnSocket = (m_sock != INVALID_SOCKET));
	}

void Socket::Close()
	{
//	_ASSERT(m_sock != INVALID_SOCKET);
	shutdown(m_sock, 2);
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	}

bool Socket::Bind(const InetAddr& addr)
	{
	return bind(m_sock, (const sockaddr*)&addr, sizeof(sockaddr)) != SOCKET_ERROR;
	}

bool Socket::Connect(const InetAddr& addr)
	{
	return connect(m_sock, (const sockaddr*)&addr, sizeof(sockaddr)) != SOCKET_ERROR;
	}

bool Socket::Listen()
	{
	return listen(m_sock, 5) != SOCKET_ERROR;
	}

Socket Socket::Accept(InetAddr& addr)
	{
#ifndef WIN32
	socklen_t len = sizeof(sockaddr);
#else
	int len = (int) sizeof(sockaddr);
#endif
	return Socket(accept(m_sock, (sockaddr*)&addr, &len));
	}

int Socket::Send(const unsigned char* buf, int cbBuf)
	{
	return send(m_sock, (const char*)buf, cbBuf, 0);
	}

int Socket::Send(const char* fmt, ...)
	{
		va_list marker;
		va_start(marker, fmt);

		char szBuf[1024*4];
		vsprintf(szBuf, fmt, marker);

		va_end(marker);

		return Send((unsigned char*)szBuf, (int) strlen(szBuf));
	}

int Socket::Receive(unsigned char* buf, int cbBuf)
	{
	int n = recv(m_sock, (char*)buf, cbBuf, 0);
	return n;
	}

bool Socket::SetOpt(int opt, const char* pBuf, int cbBuf)
	{
	return setsockopt(m_sock, SOL_SOCKET, opt, pBuf, cbBuf) != SOCKET_ERROR;
	}

bool Socket::GetOpt(int opt, char* pBuf, int& cbBuf)
	{
#ifndef WIN32
	  socklen_t l = 0;
#else
	  int l = 0;
#endif
	  bool ret = ( getsockopt(m_sock, SOL_SOCKET, opt, pBuf, &l) != SOCKET_ERROR );
	  cbBuf = (int) l;
	  return ret;
	}

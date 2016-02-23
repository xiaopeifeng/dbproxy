#include "Connection.h"
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(const std::string& ip, int port)
	: m_ip(ip)
	, m_port(port)
{}

Connection::~Connection()
{}

bool Connection::initial()
{
	struct sockaddr_in sockaddr;

	m_fd = socket(AF_INET, SOCK_STREAM, 0);	
	assert(m_fd != -1);	
	bzero((char*)&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(m_port);
	if( !inet_aton(m_ip.c_str(), &(sockaddr.sin_addr)) ){
		return -1;
	}
	int rc = connect(m_fd, (struct sockaddr*)(&sockaddr), sizeof(sockaddr));	
	printf("connect return %d\n", rc);

	return rc == 0;
}

int Connection::send(const std::string& str)
{
	size_t sendtotal = 0;

	while(sendtotal != str.length())
	{
		int sendbytes = ::send(m_fd, str.c_str(), str.length(), 0);
		if( sendbytes < 0 ){
			if( errno == EINTR || errno == EAGAIN ){
				continue;
			}else{
				close(m_fd);
				return -1;
			}
		}else if( sendbytes > 0 ){
			sendtotal += sendbytes;
		}
	}

	return 0;
}

int Connection::recv(std::string& str)
{
	int recv_bytes = 0;
	char header[20] = {0};
	while(recv_bytes < MSG_HEAD_SIZE){
		int n = ::recv(m_fd, header+recv_bytes, MSG_HEAD_SIZE-recv_bytes, 0);
		if( n < 0 ){
			if( errno == EINTR || errno == EAGAIN ){
				continue;		
			}else{
				close(m_fd);
				return -1;
			}
		}else{
			recv_bytes += n;
		}
	}

	int* ptr = reinterpret_cast<int*>(header);
	long length = ntohl(*ptr);
//	std::cout << "need to recv length:" << length << std::endl;
	assert(length > 0);
	char result_buf[10240] = {0};
	recv_bytes = 0;
	while(recv_bytes < length){
		int n = ::recv(m_fd, result_buf+recv_bytes, length-recv_bytes, 0);
		if( n < 0 ){
			if( errno == EINTR || errno == EAGAIN ){
				continue;		
			}else{
				close(m_fd);
				return -1;
			}
		}else{
			recv_bytes += n;
		}
	}

	str = std::string(result_buf);
	return 0;
}

void Connection::setConnection()
{
	int flags = fcntl(m_fd, F_GETFL, 0);
	if( fcntl(m_fd, F_SETFL, flags|O_NONBLOCK) != 0 ){
		perror("fcntl fialed!");
		exit(-1);
	}
}

#ifndef DBPROXY_TCPCONNECTION_H
#define DBPROXY_TCPCONNECTION_H
#include "Handle.h"
#include "Socket.h"
#include <queue>
#include <string>

class Buffer;
class EventLoop;
class MessageCallBack;

class Client: public Handle
{
public:
	explicit Client(Socket* s, EventLoop* lp);
	~Client();

public:
	virtual void handleEvent(int events);
	virtual int getSocket() { return m_sock->getfd(); }
	void setMessageCallBack();

private:
	int handleRead();
	int handleWrite();
	int handleError();
	int handleDisconnect();
	void close();		
	bool parse(const char* str, std::string& dbname, std::string& sql);
	int sendPendingBuf();
	int activeSend(Buffer* buf);

private:
	Socket* m_sock;	
	EventLoop* m_loop;
	MessageCallBack*  m_cb;
	std::queue<Buffer*> m_pending_buf;
};

#endif

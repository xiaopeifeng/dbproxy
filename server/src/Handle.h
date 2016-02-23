#ifndef DBPROXY_HANDLE_H
#define DBPROXY_HANDLE_H

class Handle
{
public:
	virtual void handleEvent(int events) = 0;
	virtual int getSocket() = 0; 
};

#endif

#include <signal.h>
#include <iostream>
#include <vector>

#include "Acceptor.h"
//#include "DBConfig.h"
#include "DBConnPool.h"
#include "EventLoopThread.h"
#include "rapidxml_parser.h"

struct IgnoreSigPipe
{
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
  }
};

IgnoreSigPipe iniSig;

int main()
{
  XmlParser confParser("../conf/dbproxy.xml");
  if(!confParser.parse()){
    std::cout << "dbproxy parse failed" << std::endl;
    return -1;
  }
  
  bool b = DBConnPool::getDBPoolInstance()->initial(confParser.getDBServer());  
  if(b == false){
    std::cout << "DBConnect Pool initial failed" << std::endl;
    return -1;
  }

  int worker_count = confParser.getWorkCount();
  std::vector<EventLoop*> loops;
  std::vector<EventLoopThread*> threads;
  
  std::cout << "worker_cout: " << worker_count << std::endl;
  for(int i = 0; i < worker_count; ++i)
  {
    EventLoopThread* t= new EventLoopThread();
    EventLoop* lp = t->start();
    loops.push_back(lp);
    threads.push_back(t);
  }
  
  int port = confParser.getListenPort();
  std::cout << "port: " << port << std::endl;
  std::cout << "worker thread start finished.." << std::endl;
  Acceptor acceptor(loops, port);
  if(acceptor.start() == false){
    std::cout << "acceptor start failed!" << std::endl;
    return -1;
  }
  
  acceptor.waitStop();

  for(size_t i = 0; i < threads.size(); ++i)
  {
    threads[i]->waitStop();
  }

  return 0;
}

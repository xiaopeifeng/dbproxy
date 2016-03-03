#include "EventLoop.h"
#include "EventLoopThread.h"
#include <unistd.h>
#include <pthread.h>

void* func(void* arg)
{
  printf("come on baby %d!\n", *((int*)arg));
  return NULL;
}


int main()
{
  EventLoopThread loopthread;
  EventLoop* loop = loopthread.start();

  int i = 1;
  while(i < 20){
    Task* t = new Task;
    t->fun = func;
    int* arg = new int(i);
    t->arg = (void*)(arg);
    loop->runInLoop(t);
    i++;
    usleep(1000);
  }
  printf("over 10\n");

  while(i < 50){
    Task* t = new Task;
    t->fun = func;
    int* arg = new int(i);
    t->arg = (void*)(arg);
    loop->runInLoop(t);
    i++;
    usleep(1000);
  }

  loopthread.waitStop();
}

#ifndef _THREAD_H
#define _THREAD_H
#include <sys/syscall.h>
#include <string>

typedef void* (*ThreadFunc)(void*);

namespace detail
{
struct ThreadData
{
  ThreadData(ThreadFunc fun, void* arg, const std::string& name="")
    : m_func(fun)
    , m_arg(arg)
    , m_name(name)
  {}

  void run();
  long gettid() const 
  { return m_tid; }

  const std::string& getThreadName() const
  { return m_name; }

private:
  ThreadFunc m_func;
  void* m_arg;  
  std::string m_name;  
  long m_tid;
};
}

class Thread
{
public:
  Thread(const ThreadFunc& func, void* arg, const std::string& name="");
  ~Thread();

public:
  void start();
  void stop();
  void join();
  long gettid() const
  { return m_thread_data.gettid(); }

  const std::string& getThreadName() const
  { return m_thread_data.getThreadName(); };

private:
  Thread(const Thread&);
  void operator=(const Thread&);

private:
  detail::ThreadData m_thread_data;
  pthread_t m_pthread_id;
  bool m_start;
};

#endif

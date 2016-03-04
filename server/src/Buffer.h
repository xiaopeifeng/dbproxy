#ifndef DBPROXY_BUFFER_H
#define DBPROXY_BUFFER_H
#include <string.h>

class Buffer
{
public:
  Buffer(const char* ptr, int n)
    : m_data(new char[n])
    , m_size(n)
    , m_offset(0)
  {
    memcpy(m_data, ptr, n);
  }

  ~Buffer()
  {
    delete []m_data;
  }

private:
  Buffer(const Buffer&);
  void operator=(const Buffer&);

public:
  char* m_data;
  int m_size;
  int m_offset;  
};

#endif

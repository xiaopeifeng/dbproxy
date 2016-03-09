CC      =gcc
CXX     =g++
LINK    =g++
CFLAGS  = -Wall -O3 -std=c++0x
TARGET  =dbproxy
SRCS    =$(wildcard *.cpp)
SRCS    +=$(wildcard *.c)
export LD_LIBRARY_PATH=:/usr/lib:/usr/local/lib:/usr/lib64/mysql

CXX_OBJS =$(patsubst %.cpp, %.o, $(wildcard *.cpp))
C_OBJS  =$(patsubst %.c, %.o, $(wildcard *.c))

INCLUDE =-I.
INCLUDE +=-I/usr/include/mysql
LIB = -lpthread -liksemel -ldl -lmysqlclient

all:$(TARGET)

$(TARGET):$(CXX_OBJS) $(C_OBJS)
	$(LINK) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIB)

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)

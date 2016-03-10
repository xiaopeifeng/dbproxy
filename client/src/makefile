CC      =gcc
CXX     =g++
LINK    =g++
CFLAGS  = -Wall -Werror -O3 -std=c++0x
TARGET  =dbproxyclient
SRCS    =$(wildcard *.cpp)
SRCS    +=$(wildcard *.c)

CXX_OBJS =$(patsubst %.cpp, %.o, $(wildcard *.cpp))
C_OBJS  =$(patsubst %.c, %.o, $(wildcard *.c))

#LIB = -lpthread
LIB=dbproxy.a
all:$(LIB)

$(LIB):$(CXX_OBJS) $(C_OBJS)
	ar -r $(LIB) ${CXX_OBJS} ${C_OBJS}
#	$(LINK) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIB)

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS) $(LIB)

CC      =g++
CXX     =g++
LINK    =g++
CFLAGS  =-Wall -g -O3 -std=c++0x
TARGET  =clienttest
INCLUDE = -I. -I../src
SRCS    =$(wildcard *.cpp)
SRCS    +=$(wildcard *.c)
CXX_OBJS =$(patsubst %.cpp, %.o, $(wildcard *.cpp))
C_OBJS  =$(patsubst %.c, %.o, $(wildcard *.c))

LIB = -lpthread -liksemel dbproxy.a

all:$(TARGET)

$(TARGET):$(CXX_OBJS) $(C_OBJS)
	$(LINK) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIB)

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<
%.o:%.c
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)

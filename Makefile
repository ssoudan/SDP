
CPPFLAGS=-I. -g3 -ggdb
LDFLAGS=-g3 -ggdb
LDLIBS=

SRCS=util.cpp message.cpp SDP.cpp test.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

%.o: %.cpp 
	g++ $(CFLAGS) $(CPPFLAGS) -c $<

test: $(OBJS)
	g++ $(LDFLAGS) -o test $(OBJS) $(LDLIBS) 

clean:
	rm *.o test

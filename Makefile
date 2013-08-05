#
# Service Discovery Protocol - Makefile
# @author: Sebastien Soudan <sebastien.soudan@gmail.com>
#

CPPFLAGS=-I. -g3 -ggdb -DLOCAL 
#CPPFLAGS=-I. -g3 -ggdb -DARDUINO
LDFLAGS=-g3 -ggdb
LDLIBS=

SRCS=util.cpp message.cpp SDP.cpp tests/test.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

%.o: %.cpp 
	g++ $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.xxx 
	g++ -x cpp $(CFLAGS) $(CPPFLAGS) -c $< -o $@

all: test 
	./test
	cd examples ; $(MAKE)

test: $(OBJS)
	g++ $(LDFLAGS) -o test $(OBJS) $(LDLIBS) 

examples:
	cd examples ; $(MAKE)


clean:
	cd examples ; $(MAKE) clean
	rm -f tests/*.o *.o test

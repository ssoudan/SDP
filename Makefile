#
# Service Discovery Protocol - Makefile
# @author: Sebastien Soudan <sebastien.soudan@gmail.com>
#

CPPFLAGS=-I. -Ixbee_local -ggdb -g3 -DLOCAL -I/usr/local/include/boost/
#CPPFLAGS=-I. -Ixbee_local -ggdb -g3 -DDEBUG -DLOCAL -I/usr/local/include/boost/
#CPPFLAGS=-I. -g3 -ggdb 

LDFLAGS=
LDLIBS=-L/usr/local/lib/ -lboost_iostreams -lboost_system

SRCS=util.cpp message.cpp SDP.cpp xbee_local/XBee.cpp  xbee_local/Serial.cpp
TEST_SRCS=tests/test.cpp 
SERVER_SRCS=servers/collector.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
TEST_OBJS=$(subst .cpp,.o,$(TEST_SRCS))
SERVER_OBJS=$(subst .cpp,.o,$(SERVER_SRCS))

%.o: %.cpp 
	g++ $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.xxx 
	g++ -x cpp $(CFLAGS) $(CPPFLAGS) -c $< -o $@

all: test collector
	./test
	cd examples ; $(MAKE)

test: $(OBJS) $(TEST_OBJS)
	g++ $(LDFLAGS) -o test $(OBJS) $(TEST_OBJS) $(LDLIBS) 

collector: $(OBJS) $(SERVER_OBJS)
	g++ $(LDFLAGS) -o collector $(OBJS) $(SERVER_OBJS) $(LDLIBS) 

examples:
	cd examples ; $(MAKE)

clean:
	cd examples ; $(MAKE) clean
	rm -f tests/*.o *.o test
	rm -f xbee_local/*.o 
	rm -f servers/*.o *.o collector 

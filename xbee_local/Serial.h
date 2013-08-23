/*
 Service Discovery Protocol - Stream class for local use
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stream.h"

#include <stdint.h>

#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;

class Serial : public Stream {
private:    
    boost::asio::serial_port &port;
    size_t wait_time;
    char c;
    
    boost::asio::deadline_timer timer;
    
    bool read_error;

    // Read a character and return true or return false
    // Must not be called again before read() is called
    bool read_char();

public:
	~Serial() {
		timer.cancel();
		port.cancel();
	}
 
    Serial(boost::asio::serial_port* port, size_t wait_time) :
                                                port(*port), wait_time(wait_time),
                                                timer(port->get_io_service()),
                                                read_error(true) {         
    }

    Serial(boost::asio::serial_port& port, size_t wait_time) :
                                                port(port), wait_time(wait_time),
                                                timer(port.get_io_service()),
                                                read_error(true) {         
    }
    
    Serial(const Serial& serial) :
                            port(serial.port), wait_time(serial.wait_time),
                            timer(serial.port.get_io_service()),
                            read_error(true) {         
    }

   	virtual uint8_t read(); 

   	;
	virtual bool available();
	inline virtual void flush() { /* Doesn't seem to be needed with the synchronous write we use */ };
	virtual void write(uint8_t val);

    // Callback for async read
    void read_finished(const boost::system::error_code& error,
                        size_t bytes_transferred);
 
    // Callback for the timer
    void timeout(const boost::system::error_code& error);
 
};


#endif

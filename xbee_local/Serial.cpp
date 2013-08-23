/*
 Service Discovery Protocol - Stream class for local use
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "Serial.h"

bool Serial::read_char() {
        
    c = '\0';

    // Reset the io_service in preparation for a subsequent run() invocation.
    // see http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio/reference/io_service/reset.html
    port.get_io_service().reset();

    boost::asio::async_read(port, boost::asio::buffer(&c, 1), 
            boost::bind(&Serial::read_finished, 
                    this, 
                    boost::asio::placeholders::error, 
                    boost::asio::placeholders::bytes_transferred)); 

    timer.expires_from_now(boost::posix_time::milliseconds(wait_time));
    timer.async_wait(boost::bind(&Serial::timeout,
                            this,
                            boost::asio::placeholders::error));

    // Run the io_service object's event processing loop. 
    // see http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio/reference/io_service/run.html
    port.get_io_service().run();

    if (read_error) {
        // Not read anything
        return false;
    } else {        
        return true;
    }
};

uint8_t Serial::read() {
    return (c & 255);
};

   
bool Serial::available() {
	return read_char();
};

void Serial::write(uint8_t val) {    
	boost::asio::write(port, boost::asio::buffer(&val, 1));
};

void Serial::read_finished(const boost::system::error_code& error,
                    size_t bytes_transferred) {        

    read_error = (error || bytes_transferred == 0);
    
    // Okay for this round
    timer.cancel();
}
 
void Serial::timeout(const boost::system::error_code& error) {
     
    if (error) {
        // Timer has been cancelled
        return;
    }

    // Cancel the async read
    port.cancel();
}


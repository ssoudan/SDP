#
# Service Discovery Protocol - Makefile
# @author: Sebastien Soudan <sebastien.soudan@gmail.com>
#
ARDMK_DIR=../../Arduino-Makefile/
ARDUINO_DIR=/Applications/Arduino-1.0.5.app/Contents/Resources/Java

NO_CORE = Yes

BOARD_TAG = atmega16
MCU = atmega16
F_CPU = 8000000L

ISP_PROG   = stk500v1
AVRDUDE_ISP_BAUDRATE = 19200
ISP_PORT = /dev/ttyACM*

include $(ARDMK_DIR)/arduino-mk/Arduino.mk

# !!! Important. You have to use make ispload to upload when using ISP programmer

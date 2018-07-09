BASE_DIR          = /home/borut/Arduino

TARGET            = planter
PROJECT_DIR       = $(BASE_DIR)/usr_src/$(TARGET)
BOARD_TAG         = uno

ARDMK_DIR        = $(BASE_DIR)
ARDUINO_DIR      = $(BASE_DIR)/arduino-1.8.5
ARDUINO_LIB_PATH = $(ARDUINO_DIR)/libraries
USER_LIB_PATH   := $(BASE_DIR)/usr_lib
BLOCKS_LIB_PATH := $(USER_LIB_PATH)/Blocks


### CURRENT_DIR
### Do not touch - used for binaries path
CURRENT_DIR       = $(shell basename $(CURDIR))

### OBJDIR
### This is where you put the binaries you just compile using 'make'
OBJDIR            = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)

AVRDUDE          = /usr/bin/avrdude
AVRDUDE_CONF     = /etc/avrdude.conf

### Do not touch - the path to Arduino.mk, inside the ARDMK_DIR
include $(ARDMK_DIR)/Arduino.mk


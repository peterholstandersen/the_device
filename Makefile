ARDUINO_HOME = /home/peter/arduino-1.8.1
ESPTOOL      = $(ARDUINO_HOME)/hardware/esp8266com/esp8266/tools/esptool/esptool
SKETCH       = $(notdir $(CURDIR)).ino
TARGET_DIR   = $(CURDIR)/build
MONITOR_PORT = /dev/ttyUSB0
VERBOSE      = -verbose

all:
	@ mkdir -p $(TARGET_DIR)

	@$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
	-hardware "$(ARDUINO_HOME)/hardware" \
	-tools "$(ARDUINO_HOME)/tools-builder" \
	-tools "$(ARDUINO_HOME)/hardware/tools/avr" \
	-built-in-libraries "$(ARDUINO_HOME)/libraries" \
	-fqbn=esp8266com:esp8266:d1_mini:CpuFrequency=80,UploadSpeed=115200,FlashSize=4M3M \
	-ide-version=10611 \
	-build-path "$(TARGET_DIR)" \
	-warnings=none \
	-prefs=build.warn_data_percentage=75 \
	$(VERBOSE) "$(SKETCH)"

flash:
	$(ESPTOOL) -v -cd nodemcu -cb 115200 -cp $(MONITOR_PORT) -ca 0x00000 -cf $(TARGET_DIR)/$(SKETCH).bin

upload: all flash

clean:
	rm -rf $(TARGET_DIR)

monitor:
	screen $(MONITOR_PORT) 115200

#!/bin/bash

avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega168p -c -o "$1".o "$1".c 
avr-gcc -mmcu=atmega168p "$1".o -o "$1"
avr-objcopy -O ihex -R .eeprom "$1" "$1".hex
#avrdude -p m168 -P /dev/ttyUSB* -c stk500v1 -b 19200 -F -u -U flash:w:"$1".hex
#sudo avrdude -p m168 -c dragon_isp -P usb -e -U "$1":w:"$1".hex
sudo avrdude  -p m168 -c usbasp -P usb -b 19200 -F -u -U flash:w:"$1".hex
rm "$1".o "$1".hex "$1"

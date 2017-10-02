#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color
printf "${RED} COMENSA LA COMPILACIO ${NC} \n"
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega168p -c -o "$1".o "$1".c 
avr-gcc -mmcu=atmega168p "$1".o -o "$1"
avr-objcopy -O ihex -R .eeprom "$1" "$1".hex
#avrdude -p m168 -P /dev/ttyUSB* -c stk500v1 -b 19200 -F -u -U flash:w:"$1".hex
#sudo avrdude -p m168 -c dragon_isp -P usb -e -U "$1":w:"$1".hex
sudo avrdude  -p m168 -c usbasp -P usb -b 19200 -F -u -U flash:w:"$1".hex
rm "$1".o "$1".hex "$1"

#Black        0;30     Dark Gray     1;30
#Red          0;31     Light Red     1;31
#Green        0;32     Light Green   1;32
#Brown/Orange 0;33     Yellow        1;33
#Blue         0;34     Light Blue    1;34
#Purple       0;35     Light Purple  1;35
#Cyan         0;36     Light Cyan    1;36
#Light Gray   0;37     White         1;37

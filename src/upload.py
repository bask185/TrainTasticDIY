#!/usr/bin/env python
import os
import sys
retCode = os.system("python src/build.py")
if retCode == 0 :
    print("UPLOADING")
    retCode = os.system("arduino-cli upload -b arduino:avr:nano -p COM6 -i ./build/arduino.avr.nano/trainTasticArduinoLib.ino.hex")
    if retCode == 1 :
        print("UPLOADING FAILED!!! ")
    else :
        print("UPLOADING SUCCES!!! ")

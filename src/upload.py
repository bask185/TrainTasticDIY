#!/usr/bin/env python
import os
import sys
retCode = os.system("python src/build.py")
if retCode == 0 :
    print("UPLOADING")
    #retCode = os.system("arduino-cli upload -b arduino:avr:nano -p COM7 -i ./build/arduino.avr.nano/trainTasticArduinoLib.ino.hex")
    retCode = os.system("arduino-cli upload -b arduino:avr:uno -p COM4 -i ./build/arduino.avr.uno/TrainTasticDIY.ino.hex")
    if retCode == 1 :
        print("UPLOADING FAILED!!! ")
    else :
        print("UPLOADING SUCCES!!! ")

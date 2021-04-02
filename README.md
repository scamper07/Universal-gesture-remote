UNIVERSAL GESTURE REMOTE
========================

This is a project under PES Open Source community. The aim was to build a gesture recognition wrist band capable of recognizing hand gestures of the user and hence control a number of day to day electronic devices wirelessly.



Transmitter
===========

Mouse control :- Detects the movements of the users hand and maps them to appropriate HEX codes. These HEX codes are                    then transmitted using a standard IR protocol (SIRC or RC6).


Table lamp, TV and motor control :- Controls switching on and off of table lamp, TV and motor. It also performs motor                                       speed control and TV volume control. Swipe gesture recognition is done for                                              changing channels on the TV.


Receiver
========

Motor control :- Used for turning a motor on, off and also control its speed.


Mouse control :- This enables the arduino to receive and decode the incoming IR signals. The HEX code is then sent via                   the serial port to the computer.


PyMouse :- Receives the HEX code sent by the Arduino over the serial port and performs click events and cursor                     movements.


Table lamp :- This runs on an ATmega 328 that is embedded in the base of the table lamp. It controls the switching on                 and off of the lamp.


Demo
====
Link: https://www.youtube.com/watch?v=ReSPvCIFZkU


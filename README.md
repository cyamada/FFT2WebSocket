FFT2WebSocket
=============

A combination of FFT on input from an electret microphone and WebSockets for real-time applications. 

###FFT 

The FFT [library](http://forum.arduino.cc/index.php/topic,38153.0.html) I used is a simple one provided
by the user dief on the arduino forums. I borrowed from a [modified example](http://blurtime.blogspot.com/2010/11/arduino-realtime-audio-spectrum.html)
by Paul Bishop and his Realtime Audio Spectrum Analyzer project. The frequency breakdown is from around 0-30 kHz with
0-500 Hz ommitted due to garbage values.

###Websockets

The WebSocket [library](https://github.com/leg0/Arduino-Websocket) comes from github user leg0 and was modified to send out 
a char array containing the 63 values per sample. The real-time data is then processes server-side to display an audio spectrum. 
There appears to be a 126 long char limit or the connection will time out, it's possible that adding a delay will fix the
problem but I have not tried. 


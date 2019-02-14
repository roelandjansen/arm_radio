
https://www.i2phd.org/armradio/index.html


ARM Radio is a VLF-LF-MW (only the first part of the band) SDR receiver implemented entirely on the 
STM32F429 Discovery board, apart from two small anti alias hardware filters
 

 
It covers from 8 kHz to about 900 kHz, with AM, LSB USB and CW demodulation modes,
narrow / wide bandwidth, and fast / slow AGC
 	 
The PDF document describing its implementation can be downloaded from here
The source code (C language, for the MDK Keil compiler) can be downloaded from here
(Please read the ReadMeFirst.txt file contained in the ZIP...)

These are three MP3 files representative of the reception possible with this project :
DCF-77 at 77500 Hz           The standard time and frequency signal from Mainflingen, Germany
Polskie Radio 1 at 225 kHz   A broadcasting station from Poland
Italy RAI 1 at 900 kHz        A broadcasting station from Italy

You can find the back in the mp3/ directory.

 
by Alberto I2PHD, i2phd (at) weaksignals.com



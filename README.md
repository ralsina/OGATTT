# Old Guy At The Terminal's Terminal

This project aims (with several stops along the way) to create a 
cheap, tiny, WORKING vt100-style serial terminal.

You can see the schematics for the hardware (which includes a 
keyboard and a 128x64 screen) in [Teclado.fzz](Teclado.fzz)

* For a screen I am using a SDD1306 OLED screen (128x64)
* As controller an Arduino Nano
* To connecto to the server, it uses a USB cable and does
  serial-over-USB
  
With minimal software modifications this also works as:

* A "mechanical" keyboard (in principle with awful switches, 
  but that can change!
* A **huge** macro pad (with a display!)
* An [Arduboy](https://arduboy.com/)
* Just an arduino with a bunch of buttons and an screen which
  you can use for whatever you want to do with an arduino 
  (although you will have very few free pins)

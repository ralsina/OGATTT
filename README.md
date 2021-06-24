# Old Guy At The Terminal's Terminal

## What is this?

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

## Current Status

**This is a toy**. Currently the software works well enough as
a dumb terminal (REALLY dumb) that you can login using 
busybox as a getty.

Haven't started building the keyboard prototype yet, so I am using
an old project for it in the meantime (I have just 5 keys ;-)

So, there is no way for anyone else to test this since I have
not documented the wiring or anything else (you could figure it out
from the Fritzing file in the repo) but it **does** work, as [this
video](https://www.youtube.com/watch?v=cyT2SGMdR8Y) shows.

## Testing IRL

You can send data to the terminal and see what it does by connecting to it over serial.

For example, if you send it normal characters, they will be displayed on the screen, and on the serial terminal you may see something like this:

```
I: ACTION: PRINT 's'
I: 0 Intermediate chars:
I: 1 Parameters:
I: 	2
I: ACTION: PRINT 'd'
I: 0 Intermediate chars:
I: 1 Parameters:
I: 	2
```

And if you sent a control sequence, you may see something like this 
for "ESC[3A" (move cursor 3 up):

```
I: ACTION: CSI_DISPATCH 'A'
I: 0 Intermediate chars:
I: 1 Parameters:
I: 	3
I: Unknown CSI character A
```

## Implemented Control Characters

* NUL ✔
* ENQ ✔
* BEL ✔
* BS  ✔
* HT  ✔
* LF
* VT
* FF
* CR
* SO
* SI
* XON
* XOFF
* CAN
* SUB
* ESC ✔
* DEL ✔

## Implemented Control Sequences

This will probably never be implement fully everything a terminal should support (hey, I have only 2KB of RAM!) but I'll move forward even if slowly :-)

### Erasing:

 *  ESC [ K           erase to end of line (inclusive)
 *  ESC [ 0 K         erase to end of line (inclusive)
 *  ESC [ 1 K         erase to beginning of line (inclusive)
 *  ESC [ 2 K         erase entire line (cursor doesn't move)
 *  ESC [ J           erase to end of screen (inclusive)
 *  ESC [ 0 J         erase to end of screen (inclusive)
 *  ESC [ 1 J         erase to beginning of screen (inclusive)
 *  ESC [ 2 J         erase entire screen (cursor doesn't move)

 ## Unit Testing

 It has a bunch of unit tests that can run using PlatformIO (no idea how to run them without that)

 ## Reference Material

 * [VT100 control sequences and Keyboard Map](http://braun-home.net/michael/info/misc/VT100_commands.htm)
 * [vtxxx state machine](https://vt100.net/emu/dec_ansi_parser#ACESCDIS)
 * [Keyboard Matrix](http://blog.komar.be/how-to-make-a-keyboard-the-matrix/)
 * [VT100 User Guide](https://vt100.net/docs/vt100-ug/)

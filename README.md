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
* LF  ✔ (partial)
* VT  ✔ (partial)
* FF  ✔ (partial)
* CR  ✔
* SO
* SI
* XON
* XOFF
* CAN ✔ (I think?)
* SUB ✔ (I think?)
* ESC ✔
* DEL ✔

## Implemented Control Sequences

The host sends the terminal "Control sequences" that do things such as move the cursor or clear a piece of screen.

This section will list the ones I implemented.

This will probably never be implement fully everything a terminal should support (hey, I have only 2KB of RAM!) but I'll move forward even if slowly :-)

### Cursor movement

* CUU - ESC [ Pn A          Move cursor Pn rows up (up to top margin)
* CUD - ESC [ Pn B          Move cursor Pn rows down (up to bottom margin)
* CUF - ESC [ Pn C          Move cursor Pn to the right (up to right margin)
* CUB - ESC [ Pn D          Move cursor Pn to the left (up to left margin)
* CUP - ESC [ Pn ; Pn H     Move cursor to specific position
* DECSC - ESC 7             Save cursor position
* DECRC - ESC 8             Restore cursor position

### Erasing:

*  ESC [ K           erase to end of line (inclusive)
*  ESC [ 0 K         erase to end of line (inclusive)
*  ESC [ 1 K         erase to beginning of line (inclusive)
*  ESC [ 2 K         erase entire line (cursor doesn't move)
*  ESC [ J           erase to end of screen (inclusive)
*  ESC [ 0 J         erase to end of screen (inclusive)
*  ESC [ 1 J         erase to beginning of screen (inclusive)
*  ESC [ 2 J         erase entire screen (cursor doesn't move)

### Misc

* DECSCNM - ESC [ ? 5 h|l   set/unset inverse video

### Weird

* DECALN - ESC # 8            Fill the screen with the letter "E"
* DECLL  - ESC [ Ps q	        Turn LEDs on/off (partial support)

### Not gonna implement

* DECANM
* DECARM
* DECAWM
* DECCKM
* DECCOLM [80 or 132 columns? YOU WISH!]
* DECDHL
* DECDWL
* DECID
* DECINLM
* DECKPAM
* DECKPNM
* DECOM  [MAYBE!]
* DECREPTPARM
* DECREPQTPARM
* DECSCLM

### Need Research


## Unit Testing

It has a bunch of unit tests that can run using PlatformIO (no idea how to run them without that)

## Reference Material

* [VT100 control sequences and Keyboard Map](http://braun-home.net/michael/info/misc/VT100_commands.htm)
* [vtxxx state machine](https://vt100.net/emu/dec_ansi_parser#ACESCDIS)
* [Keyboard Matrix](http://blog.komar.be/how-to-make-a-keyboard-the-matrix/)
* [VT100 User Guide](https://vt100.net/docs/vt100-ug/)
* [Line wrapping semantics](https://github.com/mattiase/wraptest)  (have not tried to follow it yet)
* [VT100 Escape Codes](https://www.cs.csub.edu/howto/vt100_escape_codes.html) has some the official manual doesn't say, like DECSCNM
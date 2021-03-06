Awesome vt codes parser:

https://github.com/haberman/vtparse

I have shamelessly:

* Copied the code
* Include the generated tables directly
* Removed the ruby code that generates the tables
* Merged a lot of vtparse.c into this project's terminal class for simplicty

Thanks Joshua Haberman for making this code public domain! I could
not have gotten this working without it!

Below is the original README

--------------


VTParse - an implementation of Paul Williams' DEC compatible state machine parser
          <http://www.vt100.net/emu/dec_ansi_parser>

Author: Joshua Haberman <joshua@reverberate.org>

This code is in the public domain.

BUILDING
========

Ruby is required at build time to generate the tables in C.  To build the library
and test program, just type make:

$ make
<this will create the test program and libvtparse.a>

I have only tested the build on Mac OS X and Linux, but it should build anywhere
Ruby and GCC are available.  Other C compilers should work as well, I just haven't
tried them.

TEST PROGRAM
============

The test program shows how to use vtparse.  To see what kind of data is returned
by the library, try:

--------------------------------

$ vim > terminaloutput
<type ':q<ENTER>' to exit vim, even though you won't see it>
$ ./test < terminaloutput  | head -16
Received action ESC_DISPATCH
Char: 0x37 ('7')

Received action CSI_DISPATCH
Char: 0x68 ('h')
1 Intermediate chars:
  0x3f ('?')
1 Parameters:
        47

Received action CSI_DISPATCH
Char: 0x68 ('h')
1 Intermediate chars:
  0x3f ('?')
1 Parameters:
        1

---------------------------------

VERIFYING
=========

You can also verify the validity of the state tables by running:

$ ruby vtparse_check_tables.rb
Tables had all necessary transitions defined.

This checks to make sure that state transitions are defined for all states, for all
characters 0-0xA0.


TODO
====

One possible enhancement is to allow the client to pass a return value from the
callback to say "stop parsing."  The vtparse() function could return the number
of bytes consumed from the input buffer.  This would be quite simple to do if
someone needed this functionality, but I don't, so I didn't bother.

Currently there is no explicit character set support.  I don't know enough about
the relevant standards to know if vtparse should have character set support, or
what that support would look like.  Suggestions along this vein are very welcome!

CREDITS
=======

Thanks to Julian Scheid for several bugfixes and enhancements.



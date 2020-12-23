At first:<br>
<br>
Like all other text that I have written in english language: please excuse my bad english
because I'm a native speaken german (and a view people said, i could write my README.md
in english language. So okay, let's go<br>
<br>
---------------------------------------------------------------------------------------<br>
<br>
In the early 1990th, a legendary sofware company called "Borland" was great and they
developed great Turbo Pascal and C-Compilers. From Turbo-Pascal version 6.0 and Borland C
version 5.0 on, the compilers were packed in an integrated development environment (IDE)
called TurboVison. This offers a textmode IDE (TUI) with at this time inovative things
like multi edior windows with changeable size and pull-down menues. All of them could
be controlled also with a mouse. On old computers this was really fast IDE.
<br>
I loved it in the 1990th, and I love it today.
<br>
Borland puplished the source and so TurboVision was available for all software developers.
FreePascal modified the TurboVison and called it FreeVision and the IDE for console
development is FreeVision till today.
<br>
                                 But it is PASCAL !<br>
<br>
So I take a look at the FreePascal source of the IDE and modified really a lot to get
an IDE (without a compiler) for programming in C. The result is<br>
<br>
                                        CIDE<br>
<br>
For smaller console projects in C and a lot of projects for microcontroller i use my
own IDE.<br>
<br>
CIDE does not include any file managment ! If you want to copy, delete or move files
on console, there is a marvelous (and absolut legandary) program called "Midnight
Commander". If this is installed (this is in 99% the case), you can call the
"Midnight Commander" with the key F7.
<br>
Due to the fact, that all my projects are "Makefile-Projects", I only have to edit
my sourcefile either with an GUI-editor (here I prefer "Geany") or with CIDE and can
call a "make all" from the IDE to build binaries.<br>
<br>
Major things in CIDE is (of course) editing the source code, call make commands:
make all, make clean, make flash.<br>
<br>
So, if you start a make command, there must be a "Makefile" in the active directory
and this must have labels called "all", "clean" and "flash".<br>
<br>
PC programs cannot (of course) be flashed, so I have added a hotkey "alt-r" to start
a compiled PC console programm.<br>
<br>
-------------------------------------------------------------------------------<br>
GENMAKEF<br>
-------------------------------------------------------------------------------<br>
<br>
To get a new project, there is given the FreeVision programm "genmakef" and this is a
"makefile generator" which works with my structure of project directories:<br>
<br>
Example hierarchy for pc programming:<br>
<br><pre>
~./pcprjoects
      |
      +--projects1
      |
      +--projects2
      |
      +..src
      |
      +..include
      |
      makefile.mk
</pre>
In directory "src" should stored all common sources from a .c / .h combination, in
directory "include" should stored all used headerfiles. makefile.mk is (for different
controllers or console PC-programs) my common functionality that is included by
the makefile of the project.<br>
<br>
Perhaps you take a look at:<br>
<br>
https://github.com/jjflash65/attiny44_projects<br>
<br>
If you follow this organiztion, you can use a program that is always stored in this
github repository called "genmakef". This program creates a project directory, a
initial source-program file and a makefile. With "genmakef" you can choose one of
the target systems: AVR, STM32F, STM8S, MCS51 and PIC16. An installed compiler for
the target system is been expected.<br>
<br>
-------------------------------------------------------------------------------<br>
TUIDUDE<br>
-------------------------------------------------------------------------------<br>
If you are programing vor Atmel/Microchip AVR-Series, there is a textmode-
interface, to flash your controllers. This is called "tuidude".<br>
<br>
An installed program "avrdude" is been expected.<br>
<br>
--------------------------------------------------------------------------------<br>
<br>
So I wish good development with the FreeVision programs on console,<br>
<br>
Greetings R. Seelig / JJFlash


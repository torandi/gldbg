OpenGL Debugger
===============
A tool for debugging buffer content in opengl applications.

Requirements
===============
This debugger makes the following assumptions:

* You're using SDL
* Each frame is terminated with SDL_GL_SwapBuffers()
* On exit you call SDL_Quit (this is when the debugger clean up it's internal state)

The debugger is only intended for modern opengl (opengl 3 or newer) and is not tested with older versions.

The debugger most likely only works on linux.

Dependencies
-----------
These are all problably already used for your opengl application
* OpenGL
* GLEW
* SDL

Usage
===========
To use the debugger, start your GL-application with the gldbg script.
To be able to run gdb or valgrind you must export LD_PRELOAD before starting your program with gdb or valgrind

The content of each buffer can be written to stderr ("print") or a gldbg.log ("log") on given conditions.
These condition can be either every n:th frame, or on a given keycombination. 
Each buffer can also be configured individually where it should be outputed (if at all).

The default behaviour is to output all buffers to the log every 300'th frame, and to stderr on ctrl+t

By default each buffer is interprented as vector of 4 floats, but this can be changed in a config file,
the number (4) only affects how many numbers are printed on each row.

Config files
==============

Both config files are located in the directory that the program was run from, and are generated with default values
if they do not exist.

gldbg.conf
---------
This is the main config.

It contains two sections:

### keyboard
Keyboard shortcuts, max 5 keys per combination.
Only ascii-letterns and the following special keys can be used:
* [r|l]?ctrl
* [r|l]?alt
* [r|l]?shift
* [r|l]?meta

If r or l are not specified l is assumed.

### buffers
Configuration for buffer printing

* defaults: Default values for buffer configuration (see buffers.conf)
* {log,print}_interval: When to output to stderr and the log, can be any of the following:
** never
** keydown (on the print_buffer key combination)
** integer >= 0 (every n:th frame, 0 means every frame)

buffers.conf
-----------
This file is regeneraded each run (but your configuration is kept), and contains
one line for each buffer in your program.

The format is:

{buffer_id}	{int,float}{number} {none,print,log,both} {buffer target}

* buffer_id: The opengl "name" of the buffer
* int/float: How to interprent the data
* number: How many numbers to print on each row on output
* none,print,log,both: Which outputs to output this buffer to, this is combined with the global settings.

Use the output setting to control which buffers you want to see, for example setting all buffers to none except one
will make it much easier to follow the content of that buffer.


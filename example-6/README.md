# Example 6

Explore the pixel data captured by the camera with the GNU debugger.

## Summary

In this example I explore the data captured by my laptop's camera in the YUYV 422 format. I use the
[GNU debugger](https://www.gnu.org/software/gdb/) to pause the program from
[example-5](https://github.com/kmdouglass/v4l2-examples/tree/master/example-5) while it's executing
and examine the memory in the image buffer.

- [YUYV 4:2:2 in V4L2
  docs](https://www.linuxtv.org/downloads/v4l-dvb-apis-old/V4L2-PIX-FMT-YUYV.html)
- [YUYV format description](http://www.fourcc.org/pixel-format/yuv-yuy2/)

## Output

```
$ gdb ./example-6
GNU gdb (Ubuntu 8.2-0ubuntu1) 8.2
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./example-6...done.
(gdb) br 187
Breakpoint 1 at 0x17d3: file example-6.c, line 187.
(gdb) run
Starting program: /home/kmd/src/v4l2-examples/example-6/example-6 

Using format: YUYV 4:2:2
Set format:
 Width: 640
 Height: 480
 Pixel format: YUYV
 Field: 1


Breakpoint 1, process_image (pBuffer=0x7ffff7fc4000) at example-6.c:187
187       fputc('.', stdout);
(gdb) x/4xb pBuffer
0x7ffff7fc4000: 0x20    0x80    0x27    0x8a
(gdb) x/8xb pBuffer
0x7ffff7fc4000: 0x20    0x80    0x27    0x8a    0x2b    0x77    0x2c    0x85
(gdb) continue
Continuing.
.
Breakpoint 1, process_image (pBuffer=0x7ffff7fc4000) at example-6.c:187                                                              
187       fflush(stdout);
(gdb) x/8xb pBuffer
0x7ffff7fc4000: 0x26    0x7f    0x28    0x85    0x2b    0x79    0x2f    0x86
(gdb) q
A debugging session is active.

        Inferior 1 [process 4647] will be killed.

Quit anyway? (y or n) y

```

## Explanation

To add debugger information into the executable, we first modify the Makefile so that the `-g` flag
is passed to the compiler:

```
CCFLAGS=-Wall -g
```

Next, I modified the `example-6.c` file by addind a `FORMAT_INDEX` variable that allows me to
select the image format that I want to explore.

```
static const __u32 FORMAT_INDEX = 0;
```

The value of `fmtdesc.index` is set to this value, rather than the value of the larget index as it
is in example-5. On my laptop, this corresponds to the format YUYV 4:2:2; on your machine, it will
likely be different depending on your camera.

I compiled the executable and ran the debugger:

```

$ make
$ gdb ./example-6

```

This launches the GNU debugger. I set a breakpoint at line 187, which corresponds to the code `
fflush(stdout);` inside the `process_image()` method. Breaking here lets us examine the value of
the image buffer. After adding the breakpoint, run the program:

```

(gdb) br 187
Breakpoint 1 at 0x17e7: file example-6.c, line 187.
(gdb) run

```

When the breakpoint was reached, I investigated 4 bytes of the memory of the buffer that was read:

```
(gdb) x/4xb pBuffer
0x7ffff7fc4000: 0x20    0x80    0x27    0x8a
```

In the [YUYV 4:2:2](http://www.fourcc.org/pixel-format/yuv-yuy2/) format, each pair of two bytes
forms an image pixel. The first byte has a value of `0x20`: this is the Y component of the first
pixel pixel. (Note that its value will vary based on the light signal on your camera. The second
byte is one chromatic component (U) and its value is `0x80`. These two bytes define one image
pixel.

But what is the other chromatic component (V) of the first pixel? Its value is `0x8a`. In YUYV
4:2:2, the first four bytes define the first two pixels, and the two chromatic components are
shared between each pixel. So the full set of YUV values of pixel one is:

```
Y1: 0x20
U1: 0x80
V1: 0x8a
```

and pixel two is:

```
Y2: 0x27
U2: 0x80
V2: 0x8a
```

The chromatic components in YUYV therefore have half the horizontal resolution than the intentsity
Y component.

I next use the gdb `continue` command to get the frame and investigate its contents.

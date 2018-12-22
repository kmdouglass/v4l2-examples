# Example 4

This example demonstrates how to allocate and setup buffers for [streaming
I/O](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/mmap.html#mmap). It is the same
example provided here: https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/mmap.html#mmap

## Summary

According to the
[docs](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/mmap.html#mmap),

> Streaming is an I/O method where only pointers to buffers are exchanged between application and
> driver, the data itself is not copied. Memory mapping is primarily intended to map buffers in
> device memory into the application’s address space. Device memory can be for example the video
> memory on a graphics card with a video capture add-on.

The buffers mentioned in this quote are described in more detail
[here](https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/buffer.html). Briefly, data is exchanged
through buffers by passing pointers to the buffers between the application and device driver. This
method (passing pointers rather than the data itself) uses memory maps to avoid costly copying of
data between the driver and application. There are other methods of passing buffers, but this is
one of the most efficient ways.

Buffers can be *enqueued*, in which they are waiting to receive data from the device, and
*dequeued*, in which they are waiting to be read by the application. There is a nice picture on
Stackoverflow that [illustrates the buffer state lifetime as a state
diagram](https://stackoverflow.com/questions/10634537/v4l2-difference-between-enque-deque-and-queueing-of-the-buffer). Once
a buffer is dequeued, it *must* be enqueued again for the driver to be able to write data to
it. And once data is written to a buffer, new data cannot be written to it.

## Output

No output

## Explanation

After some initial setup which creates a struct of type
[`v4l2_requestbuffers`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-reqbufs.html?highlight=v4l2_requestbuffers#c.v4l2_requestbuffers),
, we assign the constant
[`V4L2_MEMORY_MMAP`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/buffer.html#c.v4l2_memory)
to the `memory` member of the struct. This indicates that the buffers will be exchanged via memory
mapping. We also specify that we will be requesting 20 buffers.

After the request with the
[`VIDIOC_REQBUFS`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/kapi/v4l2-videobuf.html?highlight=vidioc_reqbuf)
ioctl, we verify that at least five buffers were created. This is because you may not get the
number of buffers you request, depending on the device memory.

Once the buffers are requested, we need a location to store the structs that contain the
information about them (the `buffers` struct). We allocate this memory dynamically with the
`calloc` call:

```
buffers = calloc(reqbuf.count, sizeof(*buffers));
```

For example, if we have 20 buffers, then we will allocate enough memory to hold 20 `buffers`
structs. `sizeof(*buffers)` returns the size (in bytes) of a single `buffers` struct instance.

Now that the buffer allocation has been performed, we need to map them into our application's
address space to be able access their data. We first declare a
[`v4l2_buffer`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/buffer.html?highlight=v4l2_buffer#c.v4l2_buffer)
struct to hold the buffers' information. The `index` member ranges between 0 and one less than the
number of buffers allocated by the call to `VIDIOC_REQBUFS`. Next, we retrieve the buffer's
information with [`VIDIOC_QUERY_BUF`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-querybuf.html?highlight=vidioc_querybuf):

```
    if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
      perror("VIDIOC_QUERYBUF");
      exit(EXIT_FAILURE);
    }
```

`buffer.length` is the length of the buffer in bytes. `buffer.m.offset` is the offset of the buffer
from the start of the device's memory. We create a memory map to a region of memory that starts at this
offset and is of length `buffer.length` using the `mmap()` method.

Finally, we clean up the memory when we are finished:

```
  for (i = 0; i < reqbuf.count; i++)
    munmap(buffers[i].start, buffers[i].length);
  free(buffers);
```

## See also

- https://lightbits.github.io/v4l2_real_time/

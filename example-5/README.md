# Example 5

Capture frames from a video stream.

## Summary

This example demonstrates the use of the following ioctls:

1. [`VIDIOC_STREAMON`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-streamon.html?highlight=vidioc_streamon)
2. [`VIDIOC_STREAMOFF`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-streamon.html?highlight=vidioc_streamon)
3. [`VIDIOC_QBUF`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-qbuf.html)
4. [`VIDIOC_DQBUF`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-qbuf.html)

These ioctls will enable/disable streaming and handle the enqueueing/dequeueing of the buffers.

## Output

```
$ ./example-5

Using format: Motion-JPEG
Set format:
 Width: 640
 Height: 480
 Pixel format: MJPG
 Field: 1

....................................................................................................

Done.
```

## Explanation

This example combines everything that was done in the previous examples and adds the streaming
ioctls to actually capture video data from the device.

First, the device and buffers are initialized in the `init_device()` method. This method largely
replicates the setup that was demonstrated in the previous examples. Following this, the
`start_capturing()` method is called:

```
static void start_capturing(void) {
  enum v4l2_buf_type type;

  struct v4l2_buffer buffer;
  for (unsigned int i = 0; i < num_buffers; i++) {
    /* Note that we set bytesused = 0, which will set it to the buffer length
     * See
     * - https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-qbuf.html?highlight=vidioc_qbuf#description
     * - https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/buffer.html#c.v4l2_buffer
     */
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = i;

    // Enqueue the buffer with VIDIOC_QBUF
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buffer)) {
      perror("VIDIOC_QBUF");
      exit(errno);
    }
  }

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
    perror("VIDIOC_STREAMON");
    exit(errno);
  }
  
}
```

First, each buffer is enqueued with the call to `VIDIOC_QBUF`. Following this, data streaming is
enabled with the call to the `VIDIOC_STREAMON` method.

The `main()` medthod then proceeds to the `mainloop()` function call. This method mostly uses Linux
systems calls to probe the device to find out when it is ready for reading. When it is, the
`read_frame()` method is called. If `read_frame()` returns 1, the infinite for loop is exited and
the frame count is reduced by one.

```
static int read_frame(void) {
  struct v4l2_buffer buffer;
  memset(&buffer, 0, sizeof(buffer));
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;

  // Dequeue a buffer
  if (-1 == xioctl(fd, VIDIOC_DQBUF, &buffer)) {
    switch(errno) {
    case EAGAIN:
      // No buffer in the outgoing queue
      return 0;
    case EIO:
      // fall through
    default:
      perror("VIDIOC_DQBUF");
      exit(errno);
    }
  }

  assert(buffer.index < num_buffers);

  process_image(buffers[buffer.index].start);

  // Enqueue the buffer again
  if (-1 == xioctl(fd, VIDIOC_QBUF, &buffer)) {
    perror("VIDIOC_QBUF");
    exit(errno);
  }

  return 1;
}
```

The `read_frame()` method first dequeues a buffer from the outgoing queue with the `VIDIOC_DQBUF`
ioctl. The data in the dequeued buffer is then passed to the `process_image()` method. This method
is more-or-less a dummy method that writes a `.` character to the screen each time this method is
called. Finally, the buffer is placed back into the incoming queue for the driver.

After all the frames are captured, streaming is disabled in the `stop_capturing()` method, and
finally the resources are cleaned up at the end of the `main` method.

```
  // Cleanup
  for (unsigned int i = 0; i < reqbuf.count; i++)
    munmap(buffers[i].start, buffers[i].length);
  free(buffers);
  close(fd);
```

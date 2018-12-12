# Example 2

Print device crop capabilities

## Summary

In this example, we use an ioctl named
[`VIDIOC_CROPCAP`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-cropcap.html?highlight=vidioc_cropcap)
to return the device's cropping capabilities. It is largely similar to
[example-1](https://github.com/kmdouglass/v4l2-examples/tree/master/example-1).

## Output

```
$ ./example-2
Camera Cropping:
  Bounds: 640x480+0+0
  Default: 640x480+0+0
  Aspect: 1/1
```

## Explanation

The program uses an ioctl named
[`VIDIOC_CROPCAP`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-cropcap.html?highlight=vidioc_cropcap)
to obtain the device's crop capabilities. These are stored in an instance of a
[`v4l2_cropcap`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-cropcap.html?highlight=v4l2_cropcap#c.v4l2_cropcap)
`struct`.

```
  struct v4l2_cropcap cropcaps = {0};

  cropcaps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ret = ioctl(fd, VIDIOC_CROPCAP, &cropcaps);
```

Infortmation about the device's capabilities are stored in the `cropcaps` struct after the function
returns. A subset of this information is printed to the console.

Before querying the device with `VIDIOC_CROPCAP` ioctl, we need to specify the type of the
datastream by setting the `type` member of the `cropcaps` struct. The value passed to the `type`
member specifies the [type of the data
stream](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/buffer.html#c.v4l2_buf_type). Allowed
values are

- V4L2_BUF_TYPE_VIDEO_CAPTURE
- V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE
- V4L2_BUF_TYPE_VIDEO_OUTPUT
- V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE
- V4L2_BUF_TYPE_VIDEO_OVERLAY

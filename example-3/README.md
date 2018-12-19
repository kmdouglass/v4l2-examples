# Example 3

Query the device for its format specs and change the values accordingly.

## Summary

This example is the first one in which we change the state of the device. First, we use the
[`VIDIOC_ENUM_FMT`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=vidioc_enum_fmt)
ioctl to list all the video formats supported by our device. Once we get the available formats, we
select the last one and use the
[`VIDIOC_S_FMT`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=vidioc_enum_fmt)
ioctl to set the number of pixels that we want to use.

## Output

```
$ ./example-3
   YUYV 4:2:2
C  Motion-JPEG

Using format: Motion-JPEG
Set format:
 Width: 320
 Height: 240
 Pixel format: MJPG
 Field: 1
```

## Explanation

As usual, we open the device file and send the file descriptor directly to the
`print_and_set_format` method. This method does two things: it displays the available formats and
sets the device to use a particular field of view. To print the format, we initialize an empty
[`v4l2_fmtdesc`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=v4l2_fmtdesc#c.v4l2_fmtdesc)
struct and set the `type` member to `V4L2_BUF_TYPE_VIDEO_CAPTURE`. We also declare two chars which
will hold
[`v4l2_fmtdesc.flags`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=v4l2_fmtdesc#fmtdesc-flags);
these flags indicate whether the format is compressed or emulated through software, respectively.

Next, we use the
[`VIDIOC_ENUM_FMT`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=vidioc_enum_fmt)
ioctl to iterate through all the formats supported by the device.

```
  while(0 == ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
    c = fmtdesc.flags & 1 ? 'C' : ' ';
    e = fmtdesc.flags & 2 ? 'E' : ' ';

    printf("%c%c %s\n", c, e, fmtdesc.description);
    fmtdesc.index++;
  }
```

The ioctl returns 0 when successful; it returns -1 and sets the error code to
[`EINVAL`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/gen-errors.html#id1) once the
index is greater than what is supported.

After reading out the device formats, we choose the last one by leaving `fmtdesc.index` set to its
last value. We then initialize an empty
[`v4l2_format`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-g-fmt.html?highlight=v4l2_format#c.v4l2_format)
struct and set various members to values we want:

```
  // Set the device format
  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  fmt.fmt.pix.width = 320;
  fmt.fmt.pix.height = 240;
  fmt.fmt.pix.pixelformat = fmtdesc.pixelformat; // The last discovered pixel format
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
```

The `V4L2_FIELD_NONE` value relates to V4L2's notion of [video
fields](https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/field-order.html). In this case, the
images are set to be captured in progressive format, which means that the rows of pixels are
acquired one after the other. (In other words, video output is in progressive scan, rather than
interlaced.)

The video format is set with the
[`VIDIOC_S_FMT`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-enum-fmt.html?highlight=vidioc_enum_fmt)
ioctl. 

```
  if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
      perror("Could not set format description");
      return -1;
  }
```

Finally, these settings are printed to screen.

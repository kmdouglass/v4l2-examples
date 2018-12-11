# Example 1

Print device capabilities

## Summary

In this example, we use an ioctl named
[`VIDIOC_QUERYCAP`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-querycap.html?highlight=vidioc_querycap)
to return the device's driver information.

## Output

```
$ ./example-1
Driver Caps:
  Driver: "uvcvideo"
  Card: "Lenovo EasyCamera: Lenovo EasyC"
  Bus: "usb-0000:00:1d.0-1.6"
  Version: 1.0
  Capabilities: 84a00001
```

## Explanation

The device file associated with camera is specified in the `DEVICE` variable:

```
static const char DEVICE[] = "/dev/video0";
```

This file should actually exist on the `/dev/` folder. The file is opened inside the program's
`main` method with read/write access:

```
  int fd;

  // Open the device file
  fd = open(DEVICE, O_RDWR);
  if (fd < 0) {
    perror(DEVICE);
    return errno;
  }
```

`open` returns `-1` if there is an error while opening the device; the system error code is
automatically assigned to the `errno` variable from `errno.h`.

Next, the file descriptor is passed to the `print_capabilities` function where we initialize a
[`v4l2_capability`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-querycap.html?highlight=v4l2_capability#c.v4l2_capability)
struct to hold the device's driver info:

```
  struct v4l2_capability caps = {0};
```

The members of this struct are filled by the
[`VIDIOC_QUERYCAP`](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-querycap.html?highlight=v4l2_capability)
ioctl.

```
  ret = ioctl(fd, VIDIOC_QUERYCAP, &caps);
  if (ret == -1) {
    perror("Querying device capabilities");
    return errno;
  }
```

`printf` is used to display some of the information in the struct.  

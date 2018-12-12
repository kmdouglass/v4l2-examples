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
  Version: 4.18.17
  Capabilities: 84a00001
```

## Explanation

The device file associated with the camera is specified in the `DEVICE` variable:

```
static const char DEVICE[] = "/dev/video0";
```

This file should actually exist in the `/dev/` folder. The file is opened inside the program's
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

```
  printf("Driver Caps:\n"
         "  Driver: \"%s\"\n"
         "  Card: \"%s\"\n"
         "  Bus: \"%s\"\n"
         "  Version: %u.%u.%u\n"
         "  Capabilities: %08x\n",
         caps.driver,
         caps.card,
         caps.bus_info,
         (caps.version >> 16) & 0xFF,
         (caps.version >> 8) & 0xFF,
         (caps.version) & 0XFF,
         caps.capabilities);
```

`caps.version` is an unsigned 32-bit integer. According to the
[docs](https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/vidioc-querycap.html?highlight=v4l2_capability#c.v4l2_capability)
it follows the same numbering scheme as the kernel (though the two versions may be different). In
the kernel numbering scheme, a single 32 bit integer holds the version information. Take `4.18.17`,
for example. The `17` is held in the first 8 bits, the `18` is held in bits 9 to 16, and the `4` is
in bits 17 to 24. To extract the major version `4`, we shift the number to the right by 16 bits and
then perform an AND operation with `0XFF`. The AND with `0XFF` filters out the least significant 8
bits from the right-shifted number; this leaves only the `4`.

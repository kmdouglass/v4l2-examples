/**
 * Video for Linux version 2 (V4L2) example 1 - print device capabilities
 *
 * Based on
 * - https://gist.github.com/Circuitsoft/1126411
 * - https://jayrambhia.com/blog/capture-v4l2
 *
 * Kyle M. Douglass, 2018
 * kyle.m.douglass@gmail.com
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

static const char DEVICE[] = "/dev/video0";

/**
 * Prints the capabilities of the device.
 */
int print_capabilities(int fd) {
  int ret;
  struct v4l2_capability caps = {0};
  
  ret = ioctl(fd, VIDIOC_QUERYCAP, &caps);
  if (ret == -1) {
    perror("Querying device capabilities");
    return errno;
  }

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
         (caps.version ) & 0XFF,
         caps.capabilities);
  
  return 0;
}

int main(void) {
  int fd;

  // Open the device file
  fd = open(DEVICE, O_RDWR);
  if (fd < 0) {
    perror(DEVICE);
    return errno;
  }

  print_capabilities(fd);
  
  close(fd);
  return 0;
}

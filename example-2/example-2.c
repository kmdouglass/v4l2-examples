/**
 * Video for Linux version 2 (V4L2) example 2 - print device crop capabilities
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
int print_crop_capabilities(int fd) {
  int ret;
  struct v4l2_cropcap cropcaps = {0};

  cropcaps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ret = ioctl(fd, VIDIOC_CROPCAP, &cropcaps);
  if (ret == -1) {
    perror("Querying device crop capabilities");
    return errno;
  }

  printf("Camera Cropping:\n"
         "  Bounds: %dx%d+%d+%d\n"
         "  Default: %dx%d+%d+%d\n"
         "  Aspect: %d/%d\n",
         cropcaps.bounds.width, cropcaps.bounds.height, cropcaps.bounds.left, cropcaps.bounds.top,
         cropcaps.defrect.width, cropcaps.defrect.height,
	 cropcaps.defrect.left, cropcaps.defrect.top,
         cropcaps.pixelaspect.numerator, cropcaps.pixelaspect.denominator);  
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

  print_crop_capabilities(fd);
  
  close(fd);
  return 0;
}

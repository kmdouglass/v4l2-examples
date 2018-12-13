/**
 * Video for Linux version 2 (V4L2) example 3 - print and set video device format description
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
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

static const char DEVICE[] = "/dev/video0";

/**
 * Prints the capabilities of the device.
 */
int print_and_set_format(int fd) {
  // Note that this initializes fmtdesc.index to 0
  struct v4l2_fmtdesc fmtdesc = {0};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  // These will correspond to the flags V4L2_FMT_FLAG_COMPRESSED and V4L2_FMT_FLAG_EMULATED
  char c, e;

  while(0 == ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
    c = fmtdesc.flags & 1 ? 'C' : ' ';
    e = fmtdesc.flags & 2 ? 'E' : ' ';

    printf("%c%c %s\n", c, e, fmtdesc.description);
    fmtdesc.index++;
  }

  printf("\nUsing format: %s\n", fmtdesc.description);

  // Set the device format
  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  fmt.fmt.pix.width = 320;
  fmt.fmt.pix.height = 240;
  fmt.fmt.pix.pixelformat = fmtdesc.pixelformat; // The last discovered pixel format
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
      perror("Could not set format description");
      return -1;
  }

  char format_code[5];
  strncpy(format_code, (char*)&fmt.fmt.pix.pixelformat, 5);
  printf("Set format:\n"
	 " Width: %d\n"
	 " Height: %d\n"
	 " Pixel format: %s\n"
	 " Field: %d\n",
	 fmt.fmt.pix.width,
	 fmt.fmt.pix.height,
	 format_code,
	 fmt.fmt.pix.field);
  
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

  print_and_set_format(fd);
  
  close(fd);
  return 0;
}

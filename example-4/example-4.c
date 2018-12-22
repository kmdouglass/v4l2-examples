/**
 * Video for Linux version 2 (V4L2) example 4 - buffer allocation and memory mapping
 *
 * Based on
 * - https://www.linuxtv.org/downloads/v4l-dvb-apis-new/uapi/v4l/mmap.html#example-mapping-buffers-in-the-single-planar-api
 *
 * Kyle M. Douglass, 2018
 * kyle.m.douglass@gmail.com
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

static const char DEVICE[] = "/dev/video0";

/**
 * Make the request to the device for single plane buffers.
 */
int request_buffers(int fd) {
  struct v4l2_requestbuffers reqbuf;
  struct {
    void *start;
    size_t length;
  } *buffers;
  unsigned int i;

  memset(&reqbuf, 0, sizeof(reqbuf));
  reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.memory = V4L2_MEMORY_MMAP;
  reqbuf.count = 20;

  if (-1 == ioctl(fd, VIDIOC_REQBUFS, &reqbuf)) {
    if (errno == EINVAL)
      printf("Video capturing or mmap-streaming is not supported\\n");
    else
      perror("VIDIOC_REQBUFS");

    exit(EXIT_FAILURE);
  }

  // Verify that we have at least five buffers
  if (reqbuf.count < 5){
    printf("Not enough buffer memory\n");
    exit(EXIT_FAILURE);
  }

  buffers = calloc(reqbuf.count, sizeof(*buffers));
  assert(buffers != NULL);

  struct v4l2_buffer buffer;
  for (i = 0; i < reqbuf.count; i++) {
    // Prepare the buffer variable for the next buffer
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = reqbuf.type;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = i;

    // Query the buffer for its information
    if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
      perror("VIDIOC_QUERYBUF");
      exit(EXIT_FAILURE);
    }

    buffers[i].length = buffer.length; // Remember the buffer length for unmapping later
    buffers[i].start = mmap(
      NULL,
      buffer.length,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      fd,
      buffer.m.offset
    );

    if (MAP_FAILED == buffers[i].start) {
      perror("mmap");
      exit(EXIT_FAILURE);
    }
  }

  // Cleanup
  for (i = 0; i < reqbuf.count; i++)
    munmap(buffers[i].start, buffers[i].length);
  free(buffers);

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

  request_buffers(fd);
  
  close(fd);
  return 0;
}

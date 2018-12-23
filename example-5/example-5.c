/**
 * Video for Linux version 2 (V4L2) example 5 - video capture
 *
 * Based on
 * - https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/capture.c.html
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
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

static const char DEVICE[] = "/dev/video0";

int fd;
struct {
  void *start;
  size_t length;
} *buffers;
unsigned int num_buffers;
struct v4l2_requestbuffers reqbuf = {0};

/**
 * Wrapper around ioctl calls.
 */
static int xioctl(int fd, int request, void *arg) {
  int r;

  do {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno);

  return r;
}

static void init_mmap(void) {
  reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.memory = V4L2_MEMORY_MMAP;
  reqbuf.count = 5;
  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &reqbuf)) {
    perror("VIDIOC_REQBUFS");
    exit(errno);
  }

  if (reqbuf.count < 2){
    printf("Not enough buffer memory\n");
    exit(EXIT_FAILURE);
  }

  buffers = calloc(reqbuf.count, sizeof(*buffers));
  assert(buffers != NULL);

  num_buffers = reqbuf.count;
  
  // Create the buffer memory maps
  struct v4l2_buffer buffer;
  for (unsigned int i = 0; i < reqbuf.count; i++) {
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = reqbuf.type;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = i;

    // Note: VIDIOC_QUERYBUF, not VIDIOC_QBUF, is used here!
    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
      perror("VIDIOC_QUERYBUF");
      exit(errno);
    }

    buffers[i].length = buffer.length;
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
      exit(errno);
    }
  }
}

static void init_device() {
  struct v4l2_fmtdesc fmtdesc = {0};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  // Get the format with the largest index and use it
  while(0 == xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
    fmtdesc.index++;
  }
  printf("\nUsing format: %s\n", fmtdesc.description);
  
  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  fmt.fmt.pix.width = 640;
  fmt.fmt.pix.height = 480;
  fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
    perror("VIDIOC_S_FMT");
    exit(errno);
  }

  char format_code[5];
  strncpy(format_code, (char*)&fmt.fmt.pix.pixelformat, 5);
  printf(
    "Set format:\n"
    " Width: %d\n"
    " Height: %d\n"
    " Pixel format: %s\n"
    " Field: %d\n\n",
    fmt.fmt.pix.width,
    fmt.fmt.pix.height,
    format_code,
    fmt.fmt.pix.field
  );

  init_mmap();
}

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

static void stop_capturing(void) {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
    perror("VIDIOC_STREAMOFF");
    exit(errno);
  }
}

/**
 * Draws a dot on the screen.
 *
 * Normally, the buffer would be processed here.
 */
static void process_image(const void * pBuffer) {
  fputc('.', stdout);
  fflush(stdout);
}

/**
 * Readout a frame from the buffers.
 */
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

/**
 * Poll the device until it is ready for reading.
 *
 * See https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
 */
static void main_loop(void) {
  unsigned int count = 100; // Record 100 frames
  while(count-- > 0) {

    fd_set fds;
    struct timeval tv;
    int r;
    for (;;) {
      // Clear the set of file descriptors to monitor, then add the fd for our device
      FD_ZERO(&fds);
      FD_SET(fd, &fds);

      // Set the timeout
      tv.tv_sec = 2;
      tv.tv_usec = 0;

      /**
       * Arguments are
       * - number of file descriptors
       * - set of read fds
       * - set of write fds
       * - set of except fds
       * - timeval struct
       * 
       * According to the man page for select, "nfds should be set to the highest-numbered file
       * descriptor in any of the three sets, plus 1."
       */
      r = select(fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
        if (EINTR == errno)
          continue;

	perror("select");
	exit(errno);
      }

      if (0 == r) {
        fprintf (stderr, "select timeout\n");
        exit(EXIT_FAILURE);
      }

      if (read_frame())
	// Go to next iterartion of fhe while loop; 0 means no frame is ready in the outgoing queue.
	break;
    }
  }
  
}

int main(void) {
  // Open the device file
  fd = open(DEVICE, O_RDWR);
  if (fd < 0) {
    perror(DEVICE);
    return errno;
  }

  init_device();

  start_capturing();

  main_loop();
  
  stop_capturing();  
  
  // Cleanup
  for (unsigned int i = 0; i < reqbuf.count; i++)
    munmap(buffers[i].start, buffers[i].length);
  free(buffers);
  close(fd);

  printf("\n\nDone.\n");
  return 0;
}

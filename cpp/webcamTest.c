/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see https://linuxtv.org/docs.php for more information
 */




// to test webcam frequency first:
// v4l2-ctl --stream-mmap=10 --stream-count=200 --stream-to=/dev/null
// to list possible resolutions:
// v4l2-ctl --list-formats-ext
// techniques below uses mmap and streaming mode (not always available)
// full output for device :
// v4l2-ctl --all -d /dev/video[0-XX]
// all devices :
// v4l2-ctl --list-devices

/* sources (simplified version of)
https://linuxtv.org/downloads/v4l-dvb-apis/userspace-api/v4l/capture.c.html (no licence)
https://github.com/severin-lemaignan/webcam-v4l2 (partly LGPL)
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
  void   *start;
  size_t  length;
};

static char            *dev_name;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              force_format;
static int              frame_count = 100;

static int xres = 640, yres = 480, stride = 1280; // 640 x 480

static void errno_exit(const char *s)
{
  fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
  exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
  int r;

  do {
    r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);

  return r;
}


#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

static void v4lconvert_yuyv_to_rgb24(const unsigned char *src, 
                                     unsigned char *dest,
                                     int width, int height, 
                                     int stride)
{
  int j;

  while (--height >= 0) {
    for (j = 0; j + 1 < width; j += 2) {
      int u = src[1];
      int v = src[3];
      int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
      int rg = (((u - 128) << 1) +  (u - 128) +
		((v - 128) << 2) + ((v - 128) << 1)) >> 3;
      int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

      *dest++ = CLIP(src[0] + v1);
      *dest++ = CLIP(src[0] - rg);
      *dest++ = CLIP(src[0] + u1);

      *dest++ = CLIP(src[2] + v1);
      *dest++ = CLIP(src[2] - rg);
      *dest++ = CLIP(src[2] + u1);
      src += 4;
    }
    src += stride - (width * 2);
  }
}

static void process_image(const void *p, int size)
{

  unsigned char rgbData[xres*yres*3];
  
  v4lconvert_yuyv_to_rgb24(p,
  			   rgbData,
  			   xres,
  			   yres,
  			   stride);


  FILE* out_file = fopen("image.ppm", "wb");
  if (! out_file){
    fprintf(stderr, "Fail to open image file.\n");
  } else {

    fprintf(out_file, "P6\n");
    fprintf(out_file, "%d %d 255\n", xres, yres);

    fwrite(rgbData, sizeof(unsigned char), xres*yres*3, out_file);

    fclose(out_file);
  }
  
  
  fflush(stderr);
  fprintf(stderr, ".");
  fflush(stdout);
}

static int read_frame(void)
{
  struct v4l2_buffer buf;

  CLEAR(buf);

  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
    switch (errno) {
    case EAGAIN:
      return 0;

    case EIO:
      /* Could ignore EIO, see spec. */

      /* fall through */

    default:
      errno_exit("VIDIOC_DQBUF");
    }
  }

  assert(buf.index < n_buffers);

  process_image(buffers[buf.index].start, buf.bytesused);
  
  if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    errno_exit("VIDIOC_QBUF");
  
  return 1;
}

static void mainloop(void)
{
  unsigned int count;

  count = frame_count;

  while (count-- > 0) {
    for (;;) {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO(&fds);
      FD_SET(fd, &fds);

      /* Timeout. */
      tv.tv_sec = 1;
      tv.tv_usec = 0;

      r = select(fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
	if (EINTR == errno)
	  continue;
	errno_exit("select");
      }

      if (0 == r) {
	fprintf(stderr, "select timeout\n");
	exit(EXIT_FAILURE);
      }

      if (read_frame())
	break;
      /* EAGAIN - continue select loop. */
    }
  }
}

static void stop_capturing(void)
{
  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
    errno_exit("VIDIOC_STREAMOFF");
}

static void start_capturing(void)
{
  unsigned int i;
  enum v4l2_buf_type type;

  for (i = 0; i < n_buffers; ++i) {
    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
      errno_exit("VIDIOC_QBUF");
  }
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
    errno_exit("VIDIOC_STREAMON");
}

static void uninit_device(void)
{
  unsigned int i;

  for (i = 0; i < n_buffers; ++i)
    if (-1 == munmap(buffers[i].start, buffers[i].length))
      errno_exit("munmap");

  free(buffers);
}

static void init_mmap(void)
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf(stderr, "%s does not support memory mapping\n", dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2) {
    fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
    exit(EXIT_FAILURE);
  }

  buffers = calloc(req.count, sizeof(*buffers));

  if (!buffers) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = n_buffers;

    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
      errno_exit("VIDIOC_QUERYBUF");

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
      mmap(NULL /* start anywhere */,
	   buf.length,
	   PROT_READ | PROT_WRITE /* required */,
	   MAP_SHARED /* recommended */,
	   fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start)
      errno_exit("mmap");
  }
}


static void init_device(void)
{
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;

  if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      fprintf(stderr, "%s is no V4L2 device\n", dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "%s is no video capture device\n", dev_name);
    exit(EXIT_FAILURE);
  }

  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
    exit(EXIT_FAILURE);
  }


  /* Select video input, video standard and tune here. */


  CLEAR(cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect; /* reset to default */

    if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
      switch (errno) {
      case EINVAL:
	/* Cropping not supported. */
	break;
      default:
	/* Errors ignored. */
	break;
      }
    }
  } else {
    /* Errors ignored. */
  }


  CLEAR(fmt);

  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (force_format) {
    fmt.fmt.pix.width       = xres;
    fmt.fmt.pix.height      = yres;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_NONE; // _INTERLACED

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
      errno_exit("VIDIOC_S_FMT");
    
    /* Note VIDIOC_S_FMT may change width and height. */
    
  } else {
    /* Preserve original settings as set by v4l2-ctl for example */
    if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
      errno_exit("VIDIOC_G_FMT");
  }

  xres = fmt.fmt.pix.width;
  yres = fmt.fmt.pix.height;
  stride = fmt.fmt.pix.bytesperline;

  fprintf(stderr, "Resolution: %d x %d, Stride: %d\n", xres, yres, stride);
  
  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;

  init_mmap();
}

static void close_device(void)
{
  if (-1 == close(fd))
    errno_exit("close");

  fd = -1;
}

static void open_device(void)
{
  struct stat st;

  if (-1 == stat(dev_name, &st)) {
    fprintf(stderr, "Cannot identify '%s': %d, %s\n",
	    dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (!S_ISCHR(st.st_mode)) {
    fprintf(stderr, "%s is no device\n", dev_name);
    exit(EXIT_FAILURE);
  }

  fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

  if (-1 == fd) {
    fprintf(stderr, "Cannot open '%s': %d, %s\n",
	    dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

static void usage(FILE *fp, int argc, char **argv)
{
  fprintf(fp,
	  "Usage: %s [options]\n\n"
	  "Version 1.3\n"
	  "Options:\n"
	  "-d | --device name   Video device name [%s]\n"
	  "-h | --help          Print this message\n"
	  "-f | --format        Force format to 640x480 YUYV\n"
	  "-c | --count         Number of frames to grab [%i]\n"
	  "",
	  argv[0], dev_name, frame_count);
}

static const char short_options[] = "d:hfc:";

static const struct option
long_options[] = {
  { "device", required_argument, NULL, 'd' },
  { "help",   no_argument,       NULL, 'h' },
  { "format", no_argument,       NULL, 'f' },
  { "count",  required_argument, NULL, 'c' },
  { 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
  dev_name = "/dev/video0";

  for (;;) {
    int idx;
    int c;

    c = getopt_long(argc, argv,
		    short_options, long_options, &idx);

    if (-1 == c)
      break;

    switch (c) {
    case 0: /* getopt_long() flag */
      break;

    case 'd':
      dev_name = optarg;
      break;

    case 'h':
      usage(stdout, argc, argv);
      exit(EXIT_SUCCESS);

    case 'f':
      force_format++;
      break;

    case 'c':
      errno = 0;
      frame_count = strtol(optarg, NULL, 0);
      if (errno)
	errno_exit(optarg);
      break;

    default:
      usage(stderr, argc, argv);
      exit(EXIT_FAILURE);
    }
  }

  open_device();
  init_device();
  start_capturing();
  mainloop();
  stop_capturing();
  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}

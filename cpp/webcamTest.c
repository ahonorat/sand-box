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
   https://github.com/preesm/preesm-apps (for SDL display)
*/


// To Compile:
// gcc webcamTest.c -I/usr/include/SDL2 -L/usr/lib/x86_64-linux-gnu/ -lSDL2 -lSDL2_ttf -lpthread



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


/*
 * BEGIN OF SDL DISPLAY
 **/


#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>

#define PATH_TTF_FULL "/usr/local/share/fonts/DejaVuSans.ttf"
#define FPS_MEAN 10


// extern int preesmStopThreads;


static struct timeval startTimes[FPS_MEAN];

// Starting to record time for a given stamp
void startTiming(int stamp){
  gettimeofday(&startTimes[stamp], NULL);
}

// Stoping to record time for a given stamp. Returns the time in us
double stopTiming(int stamp){
  struct timeval t2;

  gettimeofday(&t2, NULL);

  // compute and print the elapsed time in millisec
  double elapsedusd = (t2.tv_sec - startTimes[stamp].tv_sec) * 1000000.0;      // sec to us
  elapsedusd += (t2.tv_usec - startTimes[stamp].tv_usec);   // us
  return elapsedusd;
}



/**
 * Structure representing one display
 */
typedef struct RGBDisplay
{
  SDL_Texture* textures; // One overlay per frame
  SDL_Window *screen; // SDL window where to display
  SDL_Surface* surface;
  SDL_Renderer *renderer;
  TTF_Font *text_font;
  int initialized; // Initialization done ?
  int stampId;
} RGBDisplay;


// Initialize
static RGBDisplay display ;

int exitCallBack(void* userdata, SDL_Event* event){
  if (event->type == SDL_QUIT){
    printf("Exit request from GUI.\n");
    // preesmStopThreads = 1;
    return 0;
  }

  return 1;
}

void displayRGBInit(int height, int width){
	
  if (display.initialized == 0)
    {
      // Generating window name
      char* name = "Display";
      display.initialized = 1;

      SDL_SetEventFilter(exitCallBack, NULL);


      int sdlInitRes = 1, cpt = 10;
      while (sdlInitRes && cpt > 0) {
	sdlInitRes = SDL_Init(SDL_INIT_VIDEO);
	cpt--;
	if (sdlInitRes && cpt > 10) {
	  printf(" fail SDL init ... retrying\n");
	}
      }
      if (sdlInitRes){
	fflush(stdout);
	fprintf(stderr, "%d - %d -- Could not initialize SDL - %s\n", cpt, sdlInitRes, SDL_GetError());
	exit(1);
      }

      printf("SDL_Init_end\n");

      /* Initialize SDL TTF for text display */
      if (TTF_Init())
	{
	  printf("TTF initialization failed: %s\n", TTF_GetError());
	}

      printf("TTF_Init\n");

      /* Initialize Font for text display */
      display.text_font = TTF_OpenFont(PATH_TTF_FULL, 20);
      if (!display.text_font)
	{
	  printf("TTF_OpenFont: %s\n", TTF_GetError());
	}

      display.screen = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					width, height, SDL_WINDOW_SHOWN);
      if (!display.screen)
	{
	  fprintf(stderr, "SDL: could not set video mode - exiting\n");
	  exit(1);
	}

      display.renderer = SDL_CreateRenderer(display.screen, -1, SDL_RENDERER_ACCELERATED);
      if (!display.renderer)
	{
	  fprintf(stderr, "SDL: could not create renderer - exiting\n");
	  exit(1);
	}


      if (display.textures == NULL)
	{

	  display.textures = SDL_CreateTexture(display.renderer,
					       SDL_PIXELFORMAT_RGB888,
					       SDL_TEXTUREACCESS_STREAMING,
					       width, height);

	  if (!display.textures)
	    {
	      fprintf(stderr, "SDL: could not create texture - exiting\n");
	      exit(1);
	    }

	}

      if (display.surface == NULL){
	display.surface = SDL_CreateRGBSurface(0, width, height, 32,0,0,0,0);
	if (!display.surface)
	  {
	    fprintf(stderr, "SDL: could not create surface - exiting\n");
	    exit(1);
	  }
      }


      display.stampId = 0;
      for (int i = 1; i<FPS_MEAN; i++){
	startTiming(i);
      }

    }

}


void refreshDisplayRGB()
{
  SDL_Texture* texture = display.textures;
  SDL_Event event;
  SDL_Rect screen_rect;

  SDL_QueryTexture(texture, NULL, NULL, &(screen_rect.w), &(screen_rect.h));

  SDL_UpdateTexture(texture, NULL, display.surface->pixels, screen_rect.w*4);

  screen_rect.x = 0;
  screen_rect.y = 0;

  SDL_RenderCopy(display.renderer, texture, NULL, &screen_rect);

  /* Draw FPS text */
  char fps_text[20];
  SDL_Color colorWhite = { 255, 255, 255, 255 };

  double time = stopTiming(display.stampId);
  sprintf(fps_text, "FPS: %.1lf", FPS_MEAN / (time / 1000000.));
  startTiming(display.stampId);
  display.stampId = (display.stampId + 1) % FPS_MEAN;

  SDL_Surface* fpsText = TTF_RenderText_Blended(display.text_font, fps_text, colorWhite);
  SDL_Texture* fpsTexture = SDL_CreateTextureFromSurface(display.renderer, fpsText);

  int fpsWidth, fpsHeight;
  SDL_QueryTexture(fpsTexture, NULL, NULL, &fpsWidth, &fpsHeight);
  SDL_Rect fpsTextRect;

  fpsTextRect.x = 0;
  fpsTextRect.y = 0;
  fpsTextRect.w = fpsWidth;
  fpsTextRect.h = fpsHeight;
  SDL_RenderCopy(display.renderer, fpsTexture, NULL, &fpsTextRect);

  /* Free resources */
  SDL_FreeSurface(fpsText);
  SDL_DestroyTexture(fpsTexture);

  SDL_RenderPresent(display.renderer);

  // Grab all the events off the queue.
  while (SDL_PollEvent(&event))
    {
      switch (event.type)
	{
	default:
	  break;
	}
    }
}


void displayRGB(unsigned char *rgb){

  int idxPxl, idxPxlTimes3 = 0, idxPxlTimes4 = 0;
  int w, h;
  SDL_Texture* texture = display.textures;
  SDL_Surface *surface = display.surface;

  // Prepare RGB texture
  // Retrieve texture attribute
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  int size = w*h;
     
  for (idxPxl = 0; idxPxl < size; idxPxl++) {
    *(((char*)(surface->pixels)) + idxPxlTimes4 + 0) = *(rgb + idxPxlTimes3 + 2);
    *(((char*)(surface->pixels)) + idxPxlTimes4 + 1) = *(rgb + idxPxlTimes3 + 1);
    *(((char*)(surface->pixels)) + idxPxlTimes4 + 2) = *(rgb + idxPxlTimes3 + 0);

    idxPxlTimes3 += 3;
    idxPxlTimes4 += 4;
  }

  refreshDisplayRGB();
}


void finalizeRGB()
{
  SDL_FreeSurface(display.surface);
  SDL_DestroyTexture(display.textures);
  SDL_DestroyRenderer(display.renderer);
  SDL_DestroyWindow(display.screen);
}

/*
 * END OF SDL DISPLAY
 **/



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


  displayRGB(rgbData);
  
  /* FILE* out_file = fopen("image.ppm", "wb"); */
  /* if (! out_file){ */
  /*   fprintf(stderr, "Fail to open image file.\n"); */
  /* } else { */

  /*   fprintf(out_file, "P6\n"); */
  /*   fprintf(out_file, "%d %d 255\n", xres, yres); */

  /*   fwrite(rgbData, sizeof(unsigned char), xres*yres*3, out_file); */

  /*   fclose(out_file); */
  /* } */
  
  
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
  
  displayRGBInit(yres, xres); // xres and yres set by init_device()
  mainloop();
  finalizeRGB();
    
  stop_capturing();
  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}

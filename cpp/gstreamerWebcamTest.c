// https://stackoverflow.com/questions/66171219/make-gstreamer-appsink-receive-fps-same-like-streaming-video-or-rtsp-stream
// http://gstreamer-devel.966125.n4.nabble.com/Compilation-error-undefined-reference-to-gst-app-sink-pull-sample-td4683067.html
// https://medium.com/lifesjourneythroughalens/implementing-gstreamer-webcam-usb-internal-streaming-mac-c-clion-76de0fdb8b34

// DEMO OF VIDEO CAPTURE OF Nvidia Jetson TX2 onboard CSI CAMERA
// tested with L4T version 32.1.0, no need for extra packages

// compile with: gcc gstreamerWebcamTest.c `pkg-config --cflags --libs gstreamer-app-1.0` -o gstreamerTest


#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <stdio.h>
#include <string.h>

#define IMG_WIDTH 1280
#define IMG_HEIGHT 720
#define IMG_FPS 30

#define IMG_IMG_MEAN 100

static ssize_t tot_size = IMG_WIDTH*IMG_HEIGHT;
static ssize_t nv12_frame_size = ((3*IMG_WIDTH*IMG_HEIGHT)/2);
static ssize_t current_frame_offset = 0;
static unsigned char* current_frame_data = NULL;
static ssize_t frame_count = 0;

#define capsFilter1_format "video/x-raw(memory:NVMM),format=NV12,width=%d,height=%d,framerate=%d/1"
#define img_file_format "frame%lu.ppm"

#define CLIP_NV12toRGB24(color) ((color)>0) ? ((color)<65535 ? (unsigned char)((color)>>8):0xff):0

// https://gist.github.com/crearo/0d50442145b63c6c288d1c1675909990
// https://stackoverflow.com/questions/10244657/nv12-to-rgb24-conversion-code-in-c
// https://github.com/andrechen/yuv2rgb/blob/master/yuv2rgb.cpp

static void convert_NV12_to_rgb24(unsigned char *src, unsigned char *dst) {

  unsigned char * offsetUV = src+tot_size;
  unsigned char * nextline_src = src + IMG_WIDTH;
  unsigned char * nextline_dst = dst + IMG_WIDTH*3;
  
  int y00, y01, y10, y11;
  int u, v;
  int tR,tG,tB;
  for (int row = 0; row < IMG_HEIGHT; row += 2) {
    for (int col = 0; col < IMG_WIDTH; col += 2) {
      y00 = *(src++) - 16;
      y01 = *(src++) - 16;
      y10 = *(nextline_src++) - 16;
      y11 = *(nextline_src++) - 16;
      
      u = *(offsetUV++) - 128;
      v = *(offsetUV++) - 128;

     y00 = (y00>0) ? (298*y00):0;
     y01 = (y01>0) ? (298*y01):0;
     y10 = (y10>0) ? (298*y10):0;
     y11 = (y11>0) ? (298*y11):0;
     tR = 128 + 409*v;
     tG = 128 - 100*u - 208*v;
     tB = 128 + 516*u;

     *dst++ = CLIP_NV12toRGB24(y00+tR);
     *dst++ = CLIP_NV12toRGB24(y00+tG);
     *dst++ = CLIP_NV12toRGB24(y00+tB);
     *dst++ = CLIP_NV12toRGB24(y01+tR);
     *dst++ = CLIP_NV12toRGB24(y01+tG);
     *dst++ = CLIP_NV12toRGB24(y01+tB);

     *nextline_dst++ = CLIP_NV12toRGB24(y10+tR);
     *nextline_dst++ = CLIP_NV12toRGB24(y10+tG);
     *nextline_dst++ = CLIP_NV12toRGB24(y10+tB);
     *nextline_dst++ = CLIP_NV12toRGB24(y11+tR);
     *nextline_dst++ = CLIP_NV12toRGB24(y11+tG);
     *nextline_dst++ = CLIP_NV12toRGB24(y11+tB);
    }
    src = nextline_src; 
    nextline_src += IMG_WIDTH;

    dst = nextline_dst;
    nextline_dst += IMG_WIDTH*3;
  }
}

void output_PPM6(unsigned char * data_NV12) {
    unsigned char * dst = (unsigned char *) malloc(tot_size*3 * sizeof(unsigned char));
    convert_NV12_to_rgb24(data_NV12, dst);
   
    ssize_t bufsz = snprintf(NULL, 0, img_file_format, frame_count);
    char buf[bufsz + 1];
    snprintf(buf, bufsz + 1, img_file_format, frame_count);

   FILE* out_file = fopen(buf, "wb");
   if (! out_file){
     fprintf(stderr, "Fail to open image file.\n");
   } else {
     fprintf(out_file, "P6\n");
     fprintf(out_file, "%d %d 255\n", IMG_WIDTH, IMG_HEIGHT);
     fwrite(dst, sizeof(unsigned char), tot_size*3, out_file);
     fclose(out_file);
  }
    
    free(dst);
}


static GstFlowReturn have_frame (GstElement * appsink, gpointer user_data) {

  GstBuffer *buffer;
  GstSample *sample;
  
   /* Retrieve the buffer */
   // for some reason, try-pull-sample segfaults
//  g_signal_emit_by_name (appsink, "pull-sample", &sample);
   sample = gst_app_sink_pull_sample((GstAppSink *) appsink);

  if (sample) {
    /* The only thing we do in this example is print a * to indicate a received buffer */
    //fprintf(stderr, "Appsink: Sample received.\n");
    
//    GstBufferList * bufList =  gst_sample_get_buffer_list(sample);
//    if (bufList) {
//            fprintf(stderr, "Buffers total size in sample: %lu.\n", gst_buffer_list_calculate_size (bufList));   
//    } else {
//        fprintf(stderr, "Single buffer in sample.\n");
//    }
//    const GstStructure * sampleInfos = gst_sample_get_info (sample);
//    if (sampleInfos) {
//        fprintf(stderr, "%s\n", gst_structure_to_string (sampleInfos));
//    }  else {
//        fprintf(stderr, "No infos.\n");
//    }
//    GstSegment * sampleSegment = gst_sample_get_segment (sample);
//    if (sampleSegment) {
//        fprintf(stderr, "A segment is here.\n");
//    } else {
//            fprintf(stderr, "No segment.\n");
//    }
    
    GstMapInfo map;
    buffer = gst_sample_get_buffer (sample);
    
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        //fprintf(stderr,"Appsink: Buffer Received: Content = %lu (up to %lu)\n", map.size, map.maxsize);
        memcpy(current_frame_data+current_frame_offset, (unsigned char*)map.data, map.size);
        current_frame_offset += map.size;
        if (current_frame_offset >= nv12_frame_size) {
             frame_count++;
             //fprintf(stderr, "Received all data from frame %lu!\n", frame_count);
             current_frame_offset = 0;
             
             if (frame_count % IMG_IMG_MEAN == 0) {
                fprintf(stderr, "Outputs frame %lu!\n", frame_count);
                output_PPM6(current_frame_data);
             }
        }   
        gst_buffer_unmap (buffer, &map);
    }
        
    gst_sample_unref (sample);
    return GST_FLOW_OK;
  }

  return GST_FLOW_ERROR;
}


int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *sink, *capsFilter1, *capsFilter2, *nvvidconv;// *videoconvert;
    GstCaps *capsSource, *capsSink;
    GstBus *bus;
    GstMessage *msg;
    GMainLoop *loop;
    GstStateChangeReturn ret;    //initialize all elements
    gst_init(&argc, &argv);
    pipeline = gst_pipeline_new ("pipeline");
    source = gst_element_factory_make ("nvarguscamerasrc", "source");
    nvvidconv = gst_element_factory_make ("nvvidconv", "nvidiavideoconv");
    // https://forums.developer.nvidia.com/t/tx2-onboard-camera-command-reference-for-opencv/75724
    // videoconvert not needed since we read in NV12 directly
    //videoconvert = gst_element_factory_make ("videoconvert", "videoconverter");
    //sink = gst_element_factory_make ("autovideosink", "sink");
    sink = gst_element_factory_make ("appsink", "app_sink");

	//CAPS Filter Source 
	capsFilter1 = gst_element_factory_make ("capsfilter", NULL);
	ssize_t bufsz = snprintf(NULL, 0, capsFilter1_format, IMG_WIDTH, IMG_HEIGHT, IMG_FPS);
    char buf[bufsz];
    snprintf(buf, bufsz + 1, capsFilter1_format, IMG_WIDTH, IMG_HEIGHT, IMG_FPS);
    // https://forums.developer.nvidia.com/t/using-x-raw-memory-nvmm-in-gstreamer-program/42654 
    capsSource = gst_caps_from_string(buf);
	g_object_set(capsFilter1, "caps", capsSource, NULL);

    // CAPS Filter Sink
	capsFilter2 = gst_element_factory_make ("capsfilter", NULL);
	capsSink = gst_caps_new_simple ("video/x-raw",
          "format",  G_TYPE_STRING, "NV12",
//          "width", GST_TYPE_INT_RANGE, IMG_WIDTH, IMG_WIDTH,
//          "height", GST_TYPE_INT_RANGE, IMG_HEIGHT, IMG_HEIGHT,
//          "width", GST_TYPE_FRACTION_RANGE, IMG_FPS, 1, IMG_FPS, 1,
          "width", G_TYPE_INT, IMG_WIDTH,
          "height", G_TYPE_INT, IMG_HEIGHT,
    	  "framerate", GST_TYPE_FRACTION, IMG_FPS, 1,
    	  NULL);
	g_object_set(capsFilter2, "caps", capsSink, NULL);

    // properties appsink
    /* Configure appsink */
    g_object_set (G_OBJECT (sink), "emit-signals", TRUE, NULL);
    g_object_set(G_OBJECT (sink), "drop", TRUE, NULL);
    g_object_set(G_OBJECT (sink), "max-buffers", 2, NULL);
    g_signal_connect (sink, "new-sample", G_CALLBACK (have_frame), NULL);

    //check for null objects
    if (!pipeline || !source || !sink || !capsFilter1 || !capsFilter2 || !nvvidconv /*|| !videoconvert*/) {
        fprintf(stderr, "Not all elements created.\n");
        return -1;
    }

    //set video source --> useless for nvarguscamerasrc
    //g_object_set(G_OBJECT (source), "location", argv[1], NULL);
    //fprintf(stderr, "==>Set video source.\n");
    //set video sink --> done earlier for appsink
    //g_object_set(G_OBJECT (sink), "sync", FALSE, NULL);
    //fprintf(stderr, "==>Set video sink.\n");
    
    current_frame_data = (unsigned char *) malloc(nv12_frame_size * sizeof(unsigned char));
    fprintf(stderr, "==>Init completed.\n");
    

    //add all elements together
    gst_bin_add_many (GST_BIN (pipeline), source, capsFilter1, nvvidconv, capsFilter2, /*videoconvert,*/ sink, NULL);
    if (gst_element_link_many (source, capsFilter1, nvvidconv, capsFilter2, /*videoconvert,*/ sink, NULL) != TRUE) {
        fprintf(stderr, "Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }
    fprintf(stderr, "==>Link elements.\n");

    //set the pipeline state to playing
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        fprintf(stderr, "Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }
    fprintf(stderr, "==>Set video to play.\n");

    //get pipeline's bus
    bus = gst_element_get_bus (pipeline);
    fprintf(stderr, "==>Setup bus.\n");

    loop = g_main_loop_new(NULL, FALSE);
    fprintf(stderr, "==>Begin stream.\n");
    g_main_loop_run(loop);

    
    fprintf(stderr, "==>End stream.\n");
    free(current_frame_data);
    g_main_loop_unref(loop);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}

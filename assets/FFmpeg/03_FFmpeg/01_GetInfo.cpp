#include<stdio.h>
#include <iostream>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/log.h"
#include "libavutil/timestamp.h"
}

int
main (int argc, char *argv[]) {

  AVFormatContext *fmt_ctx = NULL;

  avformat_open_input (&fmt_ctx, "/home/lieryang/Videos/sample_720p.mp4", NULL, NULL);


  return EXIT_SUCCESS;
}


#include<stdio.h>
#include <iostream>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/log.h"
#include "libavutil/timestamp.h"
}

using namespace std;

int
main (int argc, char *argv[]) {

  AVFormatContext *fmt_ctx = NULL;

  if (avformat_open_input (&fmt_ctx, "/home/lieryang/Videos/sample_720p.mp4", NULL, NULL) != 0) {
    cerr << "无法打开文件" << endl;
    return EXIT_FAILURE;
  }

  if (avformat_find_stream_info (fmt_ctx, NULL) < 0) {
    cerr << "无法获取流信息" << endl;
    return EXIT_FAILURE;
  }

  av_dump_format (fmt_ctx, 0, "/home/lieryang/Videos/sample_720p.mp4", false);


  // std::cout << "流数量 = " << fmt_ctx->nb_streams << std::endl;
  // std::cout << "媒体文件的持续时间 = " << fmt_ctx->duration << std::endl;
  // std::cout << "媒体文件的起始时间戳 = " << fmt_ctx->start_time << std::endl;
  // std::cout << "媒体文件的比特率 = " << fmt_ctx->bit_rate << std::endl;

  avformat_close_input (&fmt_ctx);

  return EXIT_SUCCESS;
}


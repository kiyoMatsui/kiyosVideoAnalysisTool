#ifndef FFMPEGUPTR_H
#define FFMPEGUPTR_H

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libavformat/avio.h>
  #include <libavdevice/avdevice.h>
  #include <libavutil/time.h>
  #include <libavutil/imgutils.h>
  #include <libswscale/swscale.h>
  #include <libswresample/swresample.h>
}

#include <memory>
#include <functional>

using uPtrAVFormatContext2 = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext*)> > ;
// constructor avformat_alloc_context(),
auto AVFormatContextDeleter2 = [](AVFormatContext* fc){ av_free(fc); };

struct AVFormatContextDeleter {
  void operator()(AVFormatContext* fc){   av_free(fc); }
};

using uPtrAVFormatContext = std::unique_ptr<AVFormatContext, AVFormatContextDeleter> ;

#endif

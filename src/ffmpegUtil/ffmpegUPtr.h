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


struct AVFormatContextDestructor {
  void operator()(AVFormatContext* fc){  avformat_close_input(&fc); av_free(fc);}
};
auto AVFormatContextConstructor = [](){  return avformat_alloc_context(); };
using uPtrAVFormatContext = std::unique_ptr<AVFormatContext, AVFormatContextDestructor> ;

using uPtrAVFormatContext2 = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext*)> > ;
// constructor avformat_alloc_context(),
auto AVFormatContextDeleter2 = [](AVFormatContext* fc){ avformat_close_input(&fc); av_free(fc); };

struct AVCodecContextDestructor {
  void operator()(AVCodecContext* cc){  avcodec_free_context(&cc); av_free(cc);}
};
auto AVCodecContextConstructor = [](AVCodec* avCodecPtr){  return avcodec_alloc_context3(avCodecPtr); };
using uPtrAVCodecContext = std::unique_ptr<AVCodecContext, AVCodecContextDestructor> ;

struct AVFrameDestructor {
  void operator()(AVFrame* f){   av_frame_free(&f); av_free(f);}
};
auto AVFrameConstructor = [](){  return av_frame_alloc(); };
using uPtrAVFrame = std::unique_ptr<AVFrame, AVFrameDestructor> ;

struct AVPacketDestructor {
  void operator()(AVPacket* p){    av_packet_free(&p); }
};
auto AVPacketConstructor = [](){ return av_packet_alloc(); };
using uPtrAVPacket = std::unique_ptr<AVPacket, AVPacketDestructor> ;

struct SwsContextDestructor {
  void operator()(SwsContext* sc){ sws_freeContext(sc); }
};
auto SwsContextConstructor = [](SwsContext* ptr, AVCodecContext* uPtr, AVPixelFormat out){
    return sws_getCachedContext(ptr,
                uPtr->width, uPtr->height, uPtr->pix_fmt,
                uPtr->width, uPtr->height, out,
                SWS_BICUBIC, nullptr, nullptr, nullptr); };
using uPtrSwsContext = std::unique_ptr<SwsContext, SwsContextDestructor> ;

struct SwrContextDestructor {
  void operator()(SwrContext* rc){ swr_free(&rc);  }
};
auto SwrContextConstructor = [](){
    return swr_alloc();
};
using uPtrSwrContext = std::unique_ptr<SwrContext, SwrContextDestructor> ;


#endif

#ifndef MK03_PLAYERVIDEODEC_H
#define MK03_PLAYERVIDEODEC_H


#include "appinfo.h"
#include "Mk03/engineUtil.h"
#include "ffmpegUPtr.h"
#include "mySpsc/boost/lockfree/spsc_queue.hpp"
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>

namespace Mk03 {

class playerVideoDec {

friend class engineContainer;

public:
  playerVideoDec(engineJointData& ejd, uPtrAVFormatContext& afc, AVPixelFormat pFmt, bool flushFlag)
    : jointData(ejd)
    , timeBase(afc->streams[jointData.videoStreamIndex]->time_base)
    , pixelBytes(av_get_bits_per_pixel(av_pix_fmt_desc_get(pFmt))/jointData.bitsPerByte)
    , videoCodecContext(nullptr)
    , scaleContext(nullptr)
    , videoFrame(nullptr)
    , videoQueueSend{jointData.videoBufferQueue}
    , videoQueueReturn{jointData.videoBufferQueue}
    , exceptionPtr(nullptr)
    , exceptionFlag(false) {
    assert(pktQueue > audioBufferQueue + videoBufferQueue);
    const auto videoCodec = avcodec_find_decoder(afc->streams[jointData.videoStreamIndex]->codecpar->codec_id);
    videoCodecContext.reset(AVCodecContextConstructor(videoCodec));
    avcodec_parameters_to_context(videoCodecContext.get(), afc->streams[jointData.videoStreamIndex]->codecpar);
    avcodec_open2(videoCodecContext.get(), videoCodec, nullptr);
    scaleContext.reset(SwsContextConstructor(nullptr, videoCodecContext.get(), pFmt));
    av_image_fill_linesizes(pFmtLineSize, pFmt, videoCodecContext->width);
    videoFrame.reset(AVFrameConstructor());
    if(flushFlag) avcodec_flush_buffers(videoCodecContext.get());

    for (unsigned int n = 0; n < jointData.videoBufferQueue ; ++n)
      videoBuffers.push_back({std::vector<std::byte>(
                               static_cast<unsigned int>(
                                 videoCodecContext->height*videoCodecContext->width*pixelBytes)),0});

    for( auto& buffer : videoBuffers)
      pushToQueue(&buffer, videoQueueReturn);
  }

private:
  void decodeVideo() {
    try {
      videoBuffer *finalFrame = nullptr;
      AVPacket* pkt = nullptr;
      while (1) {
        if (!finalFrame) finalFrame = popFromQueue(videoQueueReturn);
        if (!finalFrame) threadWait(videoQueueReturnMutex, videoQueueReturnCV, videoQueueReturn, jointData.endFlag);
        if(jointData.endFlag.load()) break;
        if (!pkt) pkt = popFromQueue<AVPacket*>(jointData.packetVideoQueueSend);
        if (!pkt) threadWait(jointData.packetVideoQueueSendMutex, jointData.packetVideoQueueSendCV, jointData.packetVideoQueueSend, jointData.endFlag);
        if(jointData.endFlag.load()) break;
        if(pkt != nullptr && finalFrame != nullptr) {
            avcodec_send_packet(videoCodecContext.get(), pkt);
            pushToQueue<AVPacket*>(pkt,jointData.packetQueueReturn);
            pkt = nullptr;
            threadNotify(jointData.packetQueueReturnMutex, jointData.packetQueueReturnCV);
            while (1) {
                int ret = avcodec_receive_frame(videoCodecContext.get(), videoFrame.get());
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
                uint8_t* buf = reinterpret_cast<uint8_t*>(finalFrame->buffer.data());
                sws_scale(scaleContext.get(), videoFrame->data, videoFrame->linesize, 0,
                          videoCodecContext->height, &buf,
                          pFmtLineSize);
                finalFrame->ptsXtimeBase_ms = (videoFrame->pts*timeBase.num*1000)/timeBase.den;
                pushToQueue(finalFrame, videoQueueSend);
                finalFrame = nullptr;
          }
        }
      }
    } catch (...) {
      const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
      if(!exceptionFlag.load()) {
        exceptionFlag.exchange(true);
        if(!exceptionPtr) exceptionPtr = std::current_exception();
        }
      end();
    }
  }

  void end() {
      jointData.endFlag.exchange(true);
  }
public:
  std::exception_ptr getExceptionPtr() const {
    if(!exceptionFlag.load()) return nullptr;
    const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
    if(exceptionPtr) return exceptionPtr;
    else return nullptr;
  }

  playerVideoDec(const playerVideoDec &other) = delete;
  playerVideoDec &operator=(const playerVideoDec &other) = delete;
  playerVideoDec(playerVideoDec &&other) noexcept = delete;
  playerVideoDec &operator=(playerVideoDec &&other) noexcept = delete;
  ~playerVideoDec() noexcept {
    end();
  }

private:
  engineJointData& jointData;
  AVRational timeBase;
  int pixelBytes;
  uPtrAVCodecContext videoCodecContext;
  uPtrSwsContext scaleContext;
  int pFmtLineSize[4];
  uPtrAVFrame videoFrame;
  std::vector<videoBuffer> videoBuffers;
  boost::lockfree::spsc_queue<videoBuffer*> videoQueueSend;
  boost::lockfree::spsc_queue<videoBuffer*> videoQueueReturn;
  mutable std::mutex videoQueueReturnMutex;
  std::condition_variable videoQueueReturnCV;
  std::exception_ptr exceptionPtr;
  mutable std::mutex exceptionPtrMutex;
  std::atomic<bool> exceptionFlag;
};

} // namespace Mk03

#endif

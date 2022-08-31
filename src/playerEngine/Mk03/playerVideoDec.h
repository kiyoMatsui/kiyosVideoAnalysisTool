#ifndef MK03_PLAYERVIDEODEC_H
#define MK03_PLAYERVIDEODEC_H

#include <atomic>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include "Mk03/engineUtil.h"
#include "appinfo.h"
#include "ffmpegUPtr.h"
#include "mySpsc/boost/lockfree/spsc_queue.hpp"

//
extern "C" {
#include <libavutil/encryption_info.h>
}
#include <iostream>
//

namespace Mk03 {

template <typename T>
class playerVideoDec {
  template <typename Tf>
  friend class engineContainer;

 public:
  playerVideoDec(engineJointData& ejd, uPtrAVFormatContext& afc, AVPixelFormat pFmt, bool flushFlag)
      : jointData(ejd),
        timeBase(afc->streams[jointData.videoStreamIndex]->time_base),
        pixelBytes(av_get_bits_per_pixel(av_pix_fmt_desc_get(pFmt)) / jointData.bitsPerByte),
        videoCodecContext(nullptr),
        scaleContext(nullptr),
        videoFrame(nullptr),
        videoQueueSend{jointData.videoBufferQueue},
        videoQueueReturn{jointData.videoBufferQueue},
        exceptionPtr(nullptr),
        exceptionFlag(false),
        pktSizeSum(0) {
    assert(pktQueue > audioBufferQueue + videoBufferQueue);
    const auto videoCodec = avcodec_find_decoder(afc->streams[jointData.videoStreamIndex]->codecpar->codec_id);
    videoCodecContext.reset(AVCodecContextConstructor(videoCodec));
    avcodec_parameters_to_context(videoCodecContext.get(), afc->streams[jointData.videoStreamIndex]->codecpar);
    avcodec_open2(videoCodecContext.get(), videoCodec, nullptr);
    scaleContext.reset(SwsContextConstructor(nullptr, videoCodecContext.get(), pFmt));
    av_image_fill_linesizes(pFmtLineSize, pFmt, videoCodecContext->width);
    videoFrame.reset(AVFrameConstructor());
    if (flushFlag) avcodec_flush_buffers(videoCodecContext.get());

    for (unsigned int n = 0; n < jointData.videoBufferQueue; ++n)
      videoBuffers.push_back({std::vector<std::byte>(static_cast<unsigned int>(videoCodecContext->height *
                                                                               videoCodecContext->width * pixelBytes)),
                              0, 0});

    for (auto& buffer : videoBuffers) pushToQueue(&buffer, videoQueueReturn);
  }

 private:
  void decodeVideo() {
    try {
      videoBuffer* finalFrame = nullptr;
      AVPacket* pkt = nullptr;
      while (1) {
        if (!finalFrame) finalFrame = popFromQueue(videoQueueReturn);
        if (!finalFrame) threadWait(videoQueueReturnMutex, videoQueueReturnCV, videoQueueReturn, jointData.endFlag);
        if (jointData.endFlag.load()) break;
        if (!pkt) pkt = popFromQueue<AVPacket*>(jointData.packetVideoQueueSend);
        if (!pkt)
          threadWait(jointData.packetVideoQueueSendMutex, jointData.packetVideoQueueSendCV,
                     jointData.packetVideoQueueSend, jointData.endFlag);
        if (jointData.endFlag.load()) break;
        if (pkt != nullptr && finalFrame != nullptr) {
          //
          //
          // print video AVEncryptionInfo
          //

          try {
            std::cout << "pkt_pos: " << pkt->pos << std::endl;
            if(!pkt->side_data) throw 1;
            const uint8_t* side_data = av_packet_get_side_data(
                pkt, AV_PKT_DATA_ENCRYPTION_INFO, &pkt->side_data->size);
            if(!side_data) throw 1;
            unsigned int currentByte = 0;
            AVEncryptionInfo* encInfo = av_encryption_info_get_side_data(side_data, static_cast<size_t>(pkt->side_data->size));
            if(!encInfo) throw 1;
            AVSubsampleEncryptionInfo* thePtr = &encInfo->subsamples[0];
            for(uint32_t num = 0; num < encInfo->subsample_count; ++num) {
              std::cout << num <<" bytes_of_clear_data pos: " << currentByte <<"\n" ;
              std::cout << num <<" bytes_of_clear_data: " << thePtr->bytes_of_clear_data <<"\n" ; currentByte+=thePtr->bytes_of_clear_data;
              std::cout << num <<" bytes_of_protected_data pos: " << currentByte << "\n";
              std::cout << num <<" bytes_of_protected_data: " << thePtr->bytes_of_protected_data <<"\n" << std::endl; currentByte+=thePtr->bytes_of_protected_data;
              thePtr++;
            }
            av_encryption_info_free(encInfo);
          }  catch (...) {
            std::cout << "AVEncryptionInfo print error or pkt not encrypted" << "\n" << std::endl;
          }



          if constexpr (analysis.bitrate) pktSizeSum += pkt->size;
          avcodec_send_packet(videoCodecContext.get(), pkt);
          pushToQueue<AVPacket*>(pkt, jointData.packetQueueReturn);
          pkt = nullptr;
          threadNotify(jointData.packetQueueReturnMutex, jointData.packetQueueReturnCV);
          while (1) {
            int ret = avcodec_receive_frame(videoCodecContext.get(), videoFrame.get());
            if (ret == AVERROR(EAGAIN)) {
              break;
            } else if (ret == AVERROR_EOF) {
              throw std::runtime_error("AVERROR_EOF");
            }
            uint8_t* buf = reinterpret_cast<uint8_t*>(finalFrame->buffer.data());
            sws_scale(scaleContext.get(), videoFrame->data, videoFrame->linesize, 0, videoCodecContext->height, &buf,
                      pFmtLineSize);
            finalFrame->ptsXtimeBase_ms = (videoFrame->pts * timeBase.num * 1000) / timeBase.den;
            if constexpr (analysis.bitrate) {
              finalFrame->pktPerFrame = pktSizeSum;
              pktSizeSum = 0;
            }
            pushToQueue(finalFrame, videoQueueSend);
            finalFrame = nullptr;
          }
        }
      }
    } catch (...) {
      const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
      if (!exceptionFlag.load()) {
        exceptionFlag.exchange(true);
        if (!exceptionPtr) exceptionPtr = std::current_exception();
      }
      end();
    }
  }

  void end() { jointData.endFlag.exchange(true); }

 public:
  std::exception_ptr getExceptionPtr() const {
    if (!exceptionFlag.load()) return nullptr;
    const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
    if (exceptionPtr)
      return exceptionPtr;
    else
      return nullptr;
  }

  playerVideoDec(const playerVideoDec& other) = delete;
  playerVideoDec& operator=(const playerVideoDec& other) = delete;
  playerVideoDec(playerVideoDec&& other) noexcept = delete;
  playerVideoDec& operator=(playerVideoDec&& other) noexcept = delete;
  ~playerVideoDec() noexcept { end(); }

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
  int pktSizeSum;

  inline static constexpr T analysis{};
};

}  // namespace Mk03

#endif

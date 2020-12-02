#ifndef MK03_PLAYERAUDIODEC_H
#define MK03_PLAYERAUDIODEC_H

#include <atomic>
#include <cstddef>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "Mk03/engineUtil.h"
#include "appinfo.h"
#include "ffmpegUPtr.h"
#include "mySpsc/boost/lockfree/spsc_queue.hpp"

namespace Mk03 {

template <typename T>
class playerAudioDec {
  template <typename Tf>
  friend class engineContainer;

 public:
  playerAudioDec(engineJointData& ejd, uPtrAVFormatContext& afc, AVSampleFormat sFmt, bool flushFlag)
      : jointData(ejd),
        timeBase(afc->streams[jointData.audioStreamIndex]->time_base),
        sampleBytes(av_get_bytes_per_sample(sFmt)),
        audioCodecContext(nullptr),
        resampleContext(nullptr),
        audioFrame(nullptr),
        audioQueueSend{jointData.audioBufferQueue},
        audioQueueReturn{jointData.audioBufferQueue},
        exceptionPtr(nullptr),
        exceptionFlag(false) {
    assert(pktQueue > audioBufferQueue + videoBufferQueue);
    const auto audioCodec = avcodec_find_decoder(afc->streams[jointData.audioStreamIndex]->codecpar->codec_id);
    audioCodecContext.reset(AVCodecContextConstructor(audioCodec));
    avcodec_parameters_to_context(audioCodecContext.get(), afc->streams[jointData.audioStreamIndex]->codecpar);
    avcodec_open2(audioCodecContext.get(), audioCodec, nullptr);
    resampleContext.reset(SwrContextConstructor());
    auto channelLayout = fixChannelLayout();
    av_opt_set_int(resampleContext.get(), "in_channel_layout", channelLayout, 0);
    av_opt_set_int(resampleContext.get(), "out_channel_layout", channelLayout, 0);
    av_opt_set_int(resampleContext.get(), "in_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_int(resampleContext.get(), "out_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(resampleContext.get(), "in_sample_fmt", audioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(resampleContext.get(), "out_sample_fmt", sFmt, 0);
    swr_init(resampleContext.get());
    audioFrame.reset(AVFrameConstructor());
    if (flushFlag) avcodec_flush_buffers(audioCodecContext.get());
    for (unsigned int n = 0; n < jointData.audioBufferQueue; ++n)
      audioBuffers.push_back({std::vector<std::byte>(static_cast<unsigned int>(
                                jointData.maxAudioFrameSampleNo * audioCodecContext->channels * sampleBytes)),
                              0, 0});
    for (auto& buffer : audioBuffers) pushToQueue(&buffer, audioQueueReturn);
  }

 private:
  void decodeAudio() {
    try {
      audioBuffer* temp = nullptr;
      AVPacket* pkt = nullptr;
      while (1) {
        if (!temp) temp = popFromQueue(audioQueueReturn);
        if (!temp) threadWait(audioQueueReturnMutex, audioQueueReturnCV, audioQueueReturn, jointData.endFlag);
        if (jointData.endFlag.load()) break;
        if (!pkt) pkt = popFromQueue<AVPacket*>(jointData.packetAudioQueueSend);
        if (!pkt)
          threadWait(jointData.packetAudioQueueSendMutex, jointData.packetAudioQueueSendCV,
                     jointData.packetAudioQueueSend, jointData.endFlag);
        if (jointData.endFlag.load()) break;
        if (pkt != nullptr && temp != nullptr) {
          avcodec_send_packet(audioCodecContext.get(), pkt);
          pushToQueue<AVPacket*>(pkt, jointData.packetQueueReturn);
          pkt = nullptr;
          threadNotify(jointData.packetQueueReturnMutex, jointData.packetQueueReturnCV);
          while (1) {
            int ret = avcodec_receive_frame(audioCodecContext.get(), audioFrame.get());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
            uint8_t* buf = reinterpret_cast<uint8_t*>(temp->buffer.data());
            swr_convert(resampleContext.get(), &buf, audioFrame->nb_samples, (const uint8_t**)(audioFrame->data),
                        audioFrame->nb_samples);
            temp->ptsXtimeBase_ms = (audioFrame->pts * timeBase.num * 1000) / timeBase.den;
            temp->nbSamples = audioFrame->nb_samples;
            pushToQueue(temp, audioQueueSend);
            temp = nullptr;
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

 public:
  void end() { jointData.endFlag.exchange(true); }

  std::exception_ptr getExceptionPtr() const {
    if (!exceptionFlag.load()) return nullptr;
    const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
    if (exceptionPtr) {
      return exceptionPtr;
    } else {
      return nullptr;
    }
  }

  uint64_t fixChannelLayout() const {
    if (audioCodecContext->channel_layout == 0) {
      return av_get_default_channel_layout(audioCodecContext->channels);
    } else {
      return audioCodecContext->channel_layout;
    }
  }

  playerAudioDec(const playerAudioDec& other) = delete;
  playerAudioDec& operator=(const playerAudioDec& other) = delete;
  playerAudioDec(playerAudioDec&& other) noexcept = delete;
  playerAudioDec& operator=(playerAudioDec&& other) noexcept = delete;
  ~playerAudioDec() noexcept { end(); }

 private:
  engineJointData& jointData;
  AVRational timeBase;
  int sampleBytes;
  uPtrAVCodecContext audioCodecContext;
  uPtrSwrContext resampleContext;
  uPtrAVFrame audioFrame;
  std::vector<audioBuffer> audioBuffers;
  boost::lockfree::spsc_queue<audioBuffer*> audioQueueSend;
  boost::lockfree::spsc_queue<audioBuffer*> audioQueueReturn;
  mutable std::mutex audioQueueReturnMutex;
  std::condition_variable audioQueueReturnCV;
  std::exception_ptr exceptionPtr;
  mutable std::mutex exceptionPtrMutex;
  std::atomic<bool> exceptionFlag;

  inline static constexpr T analysis{};
};

}  // namespace Mk03

#endif

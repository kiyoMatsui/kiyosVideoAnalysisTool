#ifndef MK03_ENGINECONTAINER_H
#define MK03_ENGINECONTAINER_H

#include <atomic>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include "Mk03/engineUtil.h"
#include "Mk03/playerAudioDec.h"
#include "Mk03/playerDemuxer.h"
#include "Mk03/playerVideoDec.h"
#include "appinfo.h"
#include "ffmpegUPtr.h"
#include "mySpsc/boost/lockfree/spsc_queue.hpp"

class playerenginetest;

namespace Mk03 {

template <typename T = Mk03::defaultAnalysis>
class engineContainer {
  friend class ::playerenginetest;

 public:
  engineContainer(std::string mediaSourceString, AVPixelFormat aPFmt, AVSampleFormat aSFmt, int useAudioIndex = -42,
                  int useVideoIndex = -42, int64_t seek_pts = -1)
      : jointData(),
        mediaSource(mediaSourceString),
        pFmt(aPFmt),
        sFmt(aSFmt),
        fc(AVFormatContextConstructor()),
        mPlayerDemuxerPtr(nullptr),
        mPlayerAudioDecPtr(nullptr),
        mPlayerVideoDecPtr(nullptr) {
    init(useAudioIndex, useVideoIndex, seek_pts);
  }

  void startThreads() {
    threads.emplace_back(&Mk03::playerDemuxer<T>::demux, mPlayerDemuxerPtr.get());
    threads.emplace_back(&Mk03::playerVideoDec<T>::decodeVideo, mPlayerVideoDecPtr.get());
    if (mPlayerAudioDecPtr) threads.emplace_back(&Mk03::playerAudioDec<T>::decodeAudio, mPlayerAudioDecPtr.get());
  }

 private:
  void init(int useAudioIndex = -42, int useVideoIndex = -42, int64_t seek_pts = -1) {
    assert(pktQueue > audioBufferQueue + videoBufferQueue);
    AVFormatContext* temp = fc.get();
    if (avformat_open_input(&temp, mediaSource.c_str(), nullptr, nullptr) < 0) {
      // so painfull but best, avformat_open_input deallocs fc
      fc.release();  // release uPtr
      throw std::invalid_argument("invalidUrl");
    }
    avformat_find_stream_info(fc.get(), nullptr);
    if (useAudioIndex == -42) {
      jointData.audioStreamIndex = av_find_best_stream(fc.get(), AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    } else if (useAudioIndex >= 0) {
      if (fc->streams[useAudioIndex]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
        throw std::invalid_argument("streamNotAudio");
      jointData.audioStreamIndex = useAudioIndex;
    } else {
      jointData.audioStreamIndex = -1;
    }
    if (useVideoIndex == -42) {
      jointData.videoStreamIndex = av_find_best_stream(fc.get(), AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    } else if (useVideoIndex >= 0) {
      if (fc->streams[useVideoIndex]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
        throw std::invalid_argument("streamNotVideo");
      jointData.videoStreamIndex = useVideoIndex;
    } else {
      jointData.videoStreamIndex = -1;
    }
    if (jointData.videoStreamIndex < 0) {
      throw std::invalid_argument("noVideoStream");
    }
    bool flushFlag = false;
    if (seek_pts >= 0) {
      av_seek_frame(fc.get(), jointData.videoStreamIndex, seek_pts, AVSEEK_FLAG_ANY);
      flushFlag = true;
    }
    mPlayerDemuxerPtr.reset(new playerDemuxer<T>(jointData, fc));
    if (jointData.audioStreamIndex >= 0) {
      mPlayerAudioDecPtr.reset(new playerAudioDec<T>(jointData, fc, sFmt, flushFlag));
    }
    mPlayerVideoDecPtr.reset(new playerVideoDec<T>(jointData, fc, pFmt, flushFlag));

    threads.reserve(3);
  }

 public:
  void end() {
    jointData.endFlag.exchange(true);
    {
      std::lock_guard<std::mutex> lck(jointData.playMutex);
      jointData.playCV.notify_all();
    }
    {
      std::lock_guard<std::mutex> lck(jointData.packetQueueReturnMutex);
      jointData.packetQueueReturnCV.notify_all();
    }
    if (mPlayerAudioDecPtr) {
      {
        std::lock_guard<std::mutex> lck(mPlayerAudioDecPtr->audioQueueReturnMutex);
        mPlayerAudioDecPtr->audioQueueReturnCV.notify_all();
      }
      {
        std::lock_guard<std::mutex> lck(jointData.packetAudioQueueSendMutex);
        jointData.packetAudioQueueSendCV.notify_all();
      }
    }
    {
      std::lock_guard<std::mutex> lck(mPlayerVideoDecPtr->videoQueueReturnMutex);
      mPlayerVideoDecPtr->videoQueueReturnCV.notify_all();
    }
    {
      std::lock_guard<std::mutex> lck(jointData.packetVideoQueueSendMutex);
      jointData.packetVideoQueueSendCV.notify_all();
    }
  }

  videoBuffer* getVideoBuffer() { return popFromQueue(mPlayerVideoDecPtr->videoQueueSend); }
  void returnVideoBuffer(videoBuffer* ptr) {
    pushToQueue(ptr, mPlayerVideoDecPtr->videoQueueReturn);
    threadNotify(mPlayerVideoDecPtr->videoQueueReturnMutex, mPlayerVideoDecPtr->videoQueueReturnCV);
  }
  audioBuffer* getAudioBuffer() {
    if (mPlayerAudioDecPtr) {
      return popFromQueue(mPlayerAudioDecPtr->audioQueueSend);
    }
    return nullptr;
  }
  void returnAudioBuffer(audioBuffer* ptr) {
    if (mPlayerAudioDecPtr) {
      pushToQueue(ptr, mPlayerAudioDecPtr->audioQueueReturn);
      threadNotify(mPlayerAudioDecPtr->audioQueueReturnMutex, mPlayerAudioDecPtr->audioQueueReturnCV);
    }
  }
  void play() {
    std::lock_guard<std::mutex> lck(jointData.playMutex);
    mPlayerDemuxerPtr->playFlag.exchange(true);
    jointData.playCV.notify_one();
  }
  void pause() {
    std::lock_guard<std::mutex> lck(jointData.playMutex);
    mPlayerDemuxerPtr->playFlag.exchange(false);
    jointData.playCV.notify_one();
  }

  int getStreams() const { return fc->nb_streams; }
  int getAudioStreamIndex() const { return jointData.audioStreamIndex; }
  int getVideoStreamIndex() const { return jointData.videoStreamIndex; }
  int getDuration() const { return fc->duration; }
  double getFps() const {
    return static_cast<double>((fc->streams[jointData.videoStreamIndex]->r_frame_rate).num /
                               (fc->streams[jointData.videoStreamIndex]->r_frame_rate).den);
  }
  int getVideoBitrate() const {
    if (fc->bit_rate < 1000) {
      return 10000;
    } else {
      return fc->bit_rate;
    }
  }
  int getWidth() const { return mPlayerVideoDecPtr->videoCodecContext->width; }
  int getHeight() const { return mPlayerVideoDecPtr->videoCodecContext->height; }
  int getPixelBytes() const { return mPlayerVideoDecPtr->pixelBytes; }
  int getPixelBits() const { return mPlayerVideoDecPtr->pixelBytes * jointData.bitsPerByte; }
  AVRational getVideoTimeBase() const { return mPlayerVideoDecPtr->timeBase; }

  int getSampleRate() const {
    if (mPlayerAudioDecPtr) return mPlayerAudioDecPtr->audioCodecContext->sample_rate;
    return 0;
  }
  int getChannelCount() const {
    if (mPlayerAudioDecPtr) return mPlayerAudioDecPtr->audioCodecContext->channels;
    return 0;
  }
  int getSampleBytes() const {
    if (mPlayerAudioDecPtr) return mPlayerAudioDecPtr->sampleBytes;
    return 0;
  }
  int getSampleBits() const {
    if (mPlayerAudioDecPtr) return mPlayerAudioDecPtr->sampleBytes * jointData.bitsPerByte;
    return 0;
  }

  std::exception_ptr getExceptionPtr() const {
    if (!exceptionFlag.load()) return nullptr;
    const std::lock_guard<std::mutex> lock(exceptionPtrMutex);
    if (exceptionPtr)
      return exceptionPtr;
    else
      return nullptr;
  }

  int checkStreamType(int streamIndex) const {
    assert(streamIndex <= getStreams());
    if (streamIndex < 0) return AVMEDIA_TYPE_UNKNOWN;
    if (fc->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
      return AVMEDIA_TYPE_VIDEO;
    else if (fc->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
      return AVMEDIA_TYPE_AUDIO;
    else
      return AVMEDIA_TYPE_UNKNOWN;
  }

  void seek(int64_t ptsXtimeBase) {
    int64_t seekTargetVideo =
      (ptsXtimeBase * mPlayerVideoDecPtr->timeBase.den) / (mPlayerVideoDecPtr->timeBase.num * 1000);
    av_seek_frame(fc.get(), jointData.videoStreamIndex, seekTargetVideo, AVSEEK_FLAG_ANY);
  }

 public:
  engineContainer(const engineContainer& other) = delete;
  engineContainer& operator=(const engineContainer& other) = delete;
  engineContainer(engineContainer&& other) noexcept = delete;
  engineContainer& operator=(engineContainer&& other) noexcept = delete;
  ~engineContainer() noexcept {
    end();
    for (auto& thread : threads) {
      thread.join();
    }
  }

 protected:
 private:
  engineJointData jointData;
  std::string mediaSource;
  AVPixelFormat pFmt;
  AVSampleFormat sFmt;
  uPtrAVFormatContext fc;
  std::unique_ptr<playerDemuxer<T>> mPlayerDemuxerPtr;
  std::unique_ptr<playerAudioDec<T>> mPlayerAudioDecPtr;
  std::unique_ptr<playerVideoDec<T>> mPlayerVideoDecPtr;
  std::vector<std::thread> threads;
  std::exception_ptr exceptionPtr;
  mutable std::mutex exceptionPtrMutex;
  std::atomic<bool> exceptionFlag;

  inline static constexpr T analysis{};
};

}  // namespace Mk03

#endif

#ifndef MK03_PLAYERDEMUXER_H
#define MK03_PLAYERDEMUXER_H


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
#include <condition_variable>

namespace Mk03 {

class playerDemuxer {

friend class engineContainer;

public:
  playerDemuxer(engineJointData& ejd, uPtrAVFormatContext& afc)
    : jointData(ejd)
    , fc(afc)
    , exceptionPtr(nullptr)
    , exceptionFlag(false)
    , playFlag(false) {
    assert(pktQueue > audioBufferQueue + videoBufferQueue);
    jointData.packetQueueReturn.reset();
    for (unsigned int n = 0; n < jointData.pktQueue; ++n) {
        packetBuffers.emplace_back(AVPacketConstructor());
      }
    AVPacket* pkt = nullptr;
    for (auto& pktUPtr : packetBuffers) {
        pkt = pktUPtr.get();
        int ret = av_read_frame(fc.get(), pkt);
        pushPktToCorrectQueue(ret, pkt);
        pkt = nullptr;
      }
  }

private:
  void demux() {
    try {
      AVPacket* pkt = nullptr;
      int ret = -1;
      for (;;) {
          { // pause
            std::unique_lock<std::mutex> lck(jointData.playMutex);
            jointData.playCV.wait(lck, [=]{ return playFlag.load() || jointData.endFlag.load(); });
          }
          if(jointData.endFlag.load()) break;
          if(!pkt) pkt = popFromQueue<AVPacket*>(jointData.packetQueueReturn);
          if(pkt) {
              ret = av_read_frame(fc.get(), pkt);
              if(!pushPktToCorrectQueue(ret, pkt)) break;
              pkt = nullptr;
            } else {
              threadWait(jointData.packetQueueReturnMutex, jointData.packetQueueReturnCV, jointData.packetQueueReturn, jointData.endFlag);
            }
          if(jointData.endFlag.load()) break;
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
  bool pushPktToCorrectQueue(int& ret, AVPacket* pkt) {
    if (ret >= 0) {
        if (pkt->stream_index == jointData.videoStreamIndex) {
            pushToQueue<AVPacket*>(pkt, jointData.packetVideoQueueSend);
            threadNotify(jointData.packetVideoQueueSendMutex, jointData.packetVideoQueueSendCV);
          } else if (pkt->stream_index == jointData.audioStreamIndex) {
            pushToQueue<AVPacket*>(pkt, jointData.packetAudioQueueSend);
            threadNotify(jointData.packetAudioQueueSendMutex, jointData.packetAudioQueueSendCV);
          } else {
            pushToQueue<AVPacket*>(pkt, jointData.packetQueueReturn);
          }
      } else {
        pushToQueue<AVPacket*>(pkt, jointData.packetQueueReturn);
        return false;
      }
    return true;
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

  playerDemuxer(const playerDemuxer &other) = delete;
  playerDemuxer &operator=(const playerDemuxer &other) = delete;
  playerDemuxer(playerDemuxer &&other) noexcept = delete;
  playerDemuxer &operator=(playerDemuxer &&other) noexcept = delete;
  ~playerDemuxer() noexcept {}

private:
  engineJointData& jointData;
  uPtrAVFormatContext& fc;
  std::vector<uPtrAVPacket> packetBuffers;
  std::exception_ptr exceptionPtr;
  mutable std::mutex exceptionPtrMutex;
  std::atomic<bool> exceptionFlag;
  std::atomic<bool> playFlag;
};

} // namespace Mk03

#endif

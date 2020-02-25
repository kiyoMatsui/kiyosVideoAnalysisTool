#ifndef MK03_ENGINEUTIL_H
#define MK03_ENGINEUTIL_H

#include "appinfo.h"
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

  struct engineJointData {
    friend class engineContainer;
    friend class playerDemuxer;
    friend class playerAudioDec;
    friend class playerVideoDec;

  private:
    int audioStreamIndex = -1;
    int videoStreamIndex = -1;
    int bitsPerByte = 8;
    unsigned int videoBufferQueue = 2;
    unsigned int audioBufferQueue = 6;
    unsigned int pktQueue = 512;
    int maxAudioFrameSampleNo = 8192;
    std::atomic<bool> endFlag = false;
    boost::lockfree::spsc_queue<AVPacket*> packetQueueReturn{pktQueue};
    boost::lockfree::spsc_queue<AVPacket*> packetVideoQueueSend{pktQueue};
    boost::lockfree::spsc_queue<AVPacket*> packetAudioQueueSend{pktQueue};
    mutable std::mutex packetAudioQueueSendMutex;
    std::condition_variable packetAudioQueueSendCV;
    mutable std::mutex packetVideoQueueSendMutex;
    std::condition_variable packetVideoQueueSendCV;
    mutable std::mutex packetQueueReturnMutex;
    std::condition_variable packetQueueReturnCV;
    mutable std::mutex playMutex;
    std::condition_variable playCV;
    mutable std::mutex demuxerMutex;
    std::condition_variable demuxerCV;
  };

using Buffer = std::vector<std::byte>;

  struct videoBuffer {
    Buffer buffer;
    int64_t ptsXtimeBase_ms;
  };

  struct audioBuffer {
    Buffer buffer;
    int64_t ptsXtimeBase_ms;
    int nbSamples;
  };

  inline void threadNotify(std::mutex& m, std::condition_variable& cv) {
    const std::lock_guard<std::mutex> lock(m);
    cv.notify_one();
  }

  template <typename T>
  void threadWait(std::mutex& m, std::condition_variable& cv, boost::lockfree::spsc_queue<T>& queue, std::atomic<bool>& endFlag) {
    static_assert (std::is_pointer<T>::value, "Template parameter must be a pointer");
    std::unique_lock<std::mutex> lck(m);
    cv.wait(lck, [&]{ return (queue.read_available() != 0 || endFlag.load()); });
  }

  template <typename T>
  void pushToQueue(T ptr, boost::lockfree::spsc_queue<T>& queue) {
    static_assert (std::is_pointer<T>::value, "Template parameter must be a pointer");
    queue.push(ptr);
  }

  template <typename T>
  T popFromQueue(boost::lockfree::spsc_queue<T>& queue) {
    static_assert (std::is_pointer<T>::value, "Template parameter must be a pointer");
    if (queue.empty()) {
        return nullptr;
    }
    T ptr = queue.front();
    queue.pop();
    return ptr;
  }

} // namespace Mk03

#endif

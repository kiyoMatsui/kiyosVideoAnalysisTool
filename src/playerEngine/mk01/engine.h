// Player Engine Mk1this api is quite vulnerable you must return the buffers you get so that they can be reused.
//

#ifndef MK01_ENGINE_H
#define MK01_ENGINE_H

#include "ffmpegUPtr.h"
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>

namespace Mk01 {

const int bitsPerByte = 8;
const unsigned int videoBufferQueue = 6;
const unsigned int audioBufferQueue = 6;
const unsigned int pktQueue = 24;

using Buffer = std::vector<std::byte>;

class engine {

public:
  engine(std::string mediaSourceString, AVPixelFormat pFmt, AVSampleFormat sFmt)
    : audioStreamIndex(-1)
    , videoStreamIndex(-1)
    , sampleBytes(av_get_bytes_per_sample(sFmt))
    , pixelBytes(av_get_bits_per_pixel(av_pix_fmt_desc_get(pFmt))/bitsPerByte)
    , fc(AVFormatContextConstructor())
    , videoCodecContext(nullptr)
    , audioCodecContext(nullptr)
    , scaleContext(nullptr)
    , resampleContext(nullptr)
    , videoFrame(nullptr)
    , audioFrame(nullptr)
    , endFlag(false) {

    AVFormatContext *temp = fc.get();
    if (avformat_open_input(&temp, mediaSourceString.c_str(), nullptr, nullptr) < 0) {
      // so painfull but best, avformat_open_input deallocs fc
      fc.release(); // release uptr
      throw std::invalid_argument("invalidUrl");
    }
    avformat_find_stream_info(fc.get(), nullptr);
    audioStreamIndex = av_find_best_stream(fc.get(), AVMEDIA_TYPE_AUDIO,-1, -1, nullptr, 0);
    videoStreamIndex = av_find_best_stream(fc.get(), AVMEDIA_TYPE_VIDEO,-1, -1, nullptr, 0);

    // audio frame
    const auto audioCodec = avcodec_find_decoder(fc->streams[audioStreamIndex]->codecpar->codec_id);
    audioCodecContext.reset(AVCodecContextConstructor(audioCodec));
    avcodec_parameters_to_context(audioCodecContext.get(), fc->streams[audioStreamIndex]->codecpar);
    avcodec_open2(audioCodecContext.get(), audioCodec, nullptr);
    resampleContext.reset(SwrContextConstructor());
    av_opt_set_int(resampleContext.get(), "in_channel_layout",  audioCodecContext->channel_layout, 0);
    av_opt_set_int(resampleContext.get(), "out_channel_layout", audioCodecContext->channel_layout,  0);
    av_opt_set_int(resampleContext.get(), "in_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_int(resampleContext.get(), "out_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(resampleContext.get(), "in_sample_fmt", audioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(resampleContext.get(), "out_sample_fmt", sFmt,  0);
    swr_init(resampleContext.get());
    audioFrame.reset(AVFrameConstructor());
    for (unsigned int n = 0; n < audioBufferQueue ; ++n)
        audioBuffers.push_back(std::vector<std::byte>(static_cast<unsigned int>(getAudioFrameSize())));
    for (auto& a : audioBuffers)
      audioQueueSend.push_back(&a);
    audioQueueReturn = audioQueueSend;
    audioQueueSend.clear();

    // video frame
    const auto videoCodec = avcodec_find_decoder(fc->streams[videoStreamIndex]->codecpar->codec_id);
    videoCodecContext.reset(AVCodecContextConstructor(videoCodec));
    avcodec_parameters_to_context(videoCodecContext.get(), fc->streams[videoStreamIndex]->codecpar);
    avcodec_open2(videoCodecContext.get(), videoCodec, nullptr);
    scaleContext.reset(SwsContextConstructor(nullptr, videoCodecContext.get(), pFmt));
    av_image_fill_linesizes(pFmtLineSize, pFmt, videoCodecContext->width);
    videoFrame.reset(AVFrameConstructor());
    for (unsigned int n = 0; n < videoBufferQueue; ++n)
        videoBuffers.push_back(std::vector<std::byte>(static_cast<unsigned int>(getVideoFrameSize())));
    for (auto& a : videoBuffers)
      videoQueueSend.push_back(&a);
    videoQueueReturn = videoQueueSend;
    videoQueueSend.clear();

    // packets
    for (unsigned int n = 0; n < pktQueue; ++n)
        packetBuffers.emplace_back(AVPacketConstructor());
    for (auto& a : packetBuffers)
      packetAudioQueueSend.push_back(a.get());
    packetVideoQueueSend = packetAudioQueueSend;
    packetQueueReturn = packetAudioQueueSend;
    packetAudioQueueSend.clear();
    packetVideoQueueSend.clear();
  }

  void init() {
    std::thread audioThread(&engine::decodeAudio, this);
    std::thread videoThread(&engine::decodeVideo, this);
    AVPacket* pkt = nullptr;
    while (1) {
      if(endFlag.load() == true) break;
      if(!pkt) pkt = popFromQueue<AVPacket*>(packetQueueReturn, packetQueueReturnMutex);
      if(pkt) {
          int ret = av_read_frame(fc.get(), pkt);
        if (ret >= 0) {
            if (pkt->stream_index == videoStreamIndex) {
                pushToQueue<AVPacket*>(pkt, packetVideoQueueSend, packetVideoQueueSendMutex);
            } else if (pkt->stream_index == audioStreamIndex) {
                pushToQueue<AVPacket*>(pkt, packetAudioQueueSend, packetAudioQueueSendMutex);
            } else {
                pushToQueue<AVPacket*>(pkt, packetQueueReturn, packetQueueReturnMutex);
            }
            pkt = nullptr;
        } else {
            pushToQueue<AVPacket*>(pkt, packetQueueReturn, packetQueueReturnMutex);
            pkt = nullptr;
        }
      }
    }
    videoThread.join();
    audioThread.join();
  }

private:
  void decodeVideo() {
    Buffer *temp = nullptr;
    AVPacket* pkt = nullptr;
    while (1) {
      if(endFlag.load()) break;
      if (!temp) temp = popFromQueue(videoQueueReturn, videoQueueReturnMutex);
      if (!pkt) pkt = popFromQueue<AVPacket*>(packetVideoQueueSend, packetVideoQueueSendMutex);
      if(pkt != nullptr && temp != nullptr) {
          avcodec_send_packet(videoCodecContext.get(), pkt);
          pushToQueue<AVPacket*>(pkt,packetQueueReturn, packetQueueReturnMutex);
          pkt = nullptr;
          while (1) {
              int ret = avcodec_receive_frame(videoCodecContext.get(), videoFrame.get());
              if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
              uint8_t* buf = reinterpret_cast<uint8_t*>(temp->data());
              sws_scale(scaleContext.get(), videoFrame->data, videoFrame->linesize, 0,
                        videoCodecContext->height, &buf,
                        pFmtLineSize);
          pushToQueue(temp, videoQueueSend, videoQueueSendMutex);
          temp = nullptr;
        }
      }
    }
  }

  void decodeAudio() {
    Buffer *temp = nullptr;
    AVPacket* pkt = nullptr;
    while (1) {
      if(endFlag.load()) break;
      if (!temp) temp = popFromQueue(audioQueueReturn, audioQueueReturnMutex);
      if (!pkt) pkt = popFromQueue<AVPacket*>(packetAudioQueueSend, packetAudioQueueSendMutex);
      if(pkt != nullptr && temp != nullptr) {
          avcodec_send_packet(audioCodecContext.get(), pkt);
          pushToQueue<AVPacket*>(pkt,packetQueueReturn, packetQueueReturnMutex);
          pkt = nullptr;
          while (1) {
              int ret = avcodec_receive_frame(audioCodecContext.get(), audioFrame.get());
              if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
              uint8_t* buf = reinterpret_cast<uint8_t*>(temp->data());
              swr_convert(resampleContext.get(), &buf,
                          audioFrame->nb_samples, (const uint8_t**)(audioFrame->data),
                          audioFrame->nb_samples);
          pushToQueue(temp, audioQueueSend, audioQueueSendMutex);
          temp = nullptr;
        }
      }
    }
  }

  template <typename T = Buffer*>
  void pushToQueue(T ptr, std::deque<T>& queue, std::mutex& mutex) {
    static_assert (std::is_pointer<T>::value, "Template parameter must be a pointer");
    const std::lock_guard<std::mutex> lock(mutex);
    queue.push_front(ptr);
  }

  template <typename T = Buffer*>
  T popFromQueue(std::deque<T>& queue, std::mutex& mutex) {
    static_assert (std::is_pointer<T>::value, "Template parameter must be a pointer");
    const std::lock_guard<std::mutex> lock(mutex);
    if (queue.size() > 0) {
      T ptr = queue.back();
      queue.pop_back();
      return ptr;
    }
    return nullptr;
  }

public:
  void end() {
      endFlag.exchange(true);
  }
  Buffer *getVideoBuffer() {
      return popFromQueue(videoQueueSend, videoQueueSendMutex);
  }
  void returnVideoBuffer(Buffer *ptr) {
      pushToQueue(ptr, videoQueueReturn, videoQueueReturnMutex);
  }
  Buffer *getAudioBuffer() {
      return popFromQueue(audioQueueSend, audioQueueSendMutex);
  }
  void returnAudioBuffer(Buffer *ptr) {
      pushToQueue(ptr, audioQueueReturn, audioQueueReturnMutex);
  }

  int getWidth() const { return videoCodecContext->width; }
  int getHeight() const { return videoCodecContext->height; }
  int getAudioFrameSize() const { return audioCodecContext->frame_size*audioCodecContext->channels*sampleBytes; }
  int getVideoFrameSize() const { return getWidth()*getHeight()*pixelBytes; }
  int getSampleRate() const { return audioCodecContext->sample_rate; }
  int getChannelCount() const { return audioCodecContext->channels; }
  int getSampleBytes() const { return sampleBytes; }
  int getPixelBytes() const { return pixelBytes; }
  double getFps() const {
    return (double)(fc->streams[videoStreamIndex]->avg_frame_rate).num/
        (double)(fc->streams[videoStreamIndex]->avg_frame_rate).den;
  }

public:
  engine(const engine &other) = delete;
  engine &operator=(const engine &other) = delete;
  engine(engine &&other) noexcept = delete;
  engine &operator=(engine &&other) noexcept = delete;
  ~engine() noexcept {}


protected:
private:
  int audioStreamIndex;
  int videoStreamIndex;
  int sampleBytes;
  int pixelBytes;
  uPtrAVFormatContext fc;
  uPtrAVCodecContext videoCodecContext;
  uPtrAVCodecContext audioCodecContext;
  uPtrSwsContext scaleContext;
  uPtrSwrContext resampleContext;
  int pFmtLineSize[4];
  uPtrAVFrame videoFrame;
  uPtrAVFrame audioFrame;
  std::vector<Buffer> videoBuffers;
  std::vector<Buffer> audioBuffers;
  std::vector<uPtrAVPacket> packetBuffers;
  std::deque<Buffer *> videoQueueSend;
  std::deque<Buffer *> videoQueueReturn;
  std::deque<Buffer *> audioQueueSend;
  std::deque<Buffer *> audioQueueReturn;
  std::deque<AVPacket *> packetQueueReturn;
  std::deque<AVPacket *> packetVideoQueueSend;
  std::deque<AVPacket *> packetAudioQueueSend;
  mutable std::mutex videoQueueSendMutex;
  mutable std::mutex videoQueueReturnMutex;
  mutable std::mutex audioQueueSendMutex;
  mutable std::mutex audioQueueReturnMutex;
  mutable std::mutex packetQueueReturnMutex;
  mutable std::mutex packetVideoQueueSendMutex;
  mutable std::mutex packetAudioQueueSendMutex;
  std::atomic<bool> endFlag;
};

} // namespace mk01

#endif

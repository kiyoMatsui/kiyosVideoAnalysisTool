//sadly this is hacky due to the way av_dump_format() works

#ifndef QTWIDGETSINTERFACE_AVDUMPFORMAT_H
#define QTWIDGETSINTERFACE_AVDUMPFORMAT_H

#include <gsl/gsl>

#include <vector>
#include <string>

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

namespace qtWidgetsInterface {


class avDumpFormat {

 public:    
  explicit avDumpFormat(std::string aString, int i);
  avDumpFormat(const avDumpFormat &other) = delete;
  avDumpFormat& operator=(const avDumpFormat &other) = delete;
  avDumpFormat(avDumpFormat &&other) noexcept = default;
  avDumpFormat& operator=(avDumpFormat &&other) noexcept = default;
  ~avDumpFormat() noexcept {
    if(fcAllocFlag) {
        avformat_close_input(&fc);
        av_free(fc);
    }
    setbuf(stderr, nullptr);
  }
  char* getBuffer() const;

 protected:

 private:
  std::vector<char> stdErrorBuffer;
  const unsigned int bufferSize = 16384;
  gsl::owner<AVFormatContext*> fc;
  bool fcAllocFlag = true;
  char* latestBufferPtr = nullptr;
};
}

#endif

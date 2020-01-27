//sadly this is hacky due to the way av_dump_format() works

#ifndef QTWIDGETSINTERFACE_AVDUMPFORMAT_H
#define QTWIDGETSINTERFACE_AVDUMPFORMAT_H

#include "ffmpegUPtr.h"

#include <vector>
#include <string>


namespace qtWidgetsInterface {

class avDumpFormat {

 public:    
  explicit avDumpFormat(std::string mediaSourceString, int i);
  avDumpFormat(const avDumpFormat &other) = delete;
  avDumpFormat& operator=(const avDumpFormat &other) = delete;
  avDumpFormat(avDumpFormat &&other) noexcept = delete;
  avDumpFormat& operator=(avDumpFormat &&other) noexcept = delete;
  ~avDumpFormat() noexcept {
    setvbuf(stderr, NULL,_IONBF,0);
  }
  char* getBuffer() const;

 protected:

 private:
  const unsigned int bufferSize = 65536;
  std::vector<char> stdErrorBuffer;
  uPtrAVFormatContext fc;
  char* latestBufferPtr;
};
}

#endif

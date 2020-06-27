// sadly this is hacky due to the way av_dump_format() works

#ifndef QTWIDGETSINTERFACE_AVDUMPFORMAT_H
#define QTWIDGETSINTERFACE_AVDUMPFORMAT_H

#include "ffmpegUPtr.h"

#include <string>
#include <vector>

namespace metadata {

class avDumpFormat {
 public:
  explicit avDumpFormat(const std::string& mediaSourceString, int i);
  avDumpFormat(const avDumpFormat& other) = delete;
  avDumpFormat& operator=(const avDumpFormat& other) = delete;
  avDumpFormat(avDumpFormat&& other) noexcept = delete;
  avDumpFormat& operator=(avDumpFormat&& other) noexcept = delete;
  ~avDumpFormat() noexcept { setvbuf(stderr, NULL, _IONBF, 0); }
  char* getBuffer() const;

 protected:
 public:
  const int index;

 private:
  const unsigned int bufferSize = 65536;
  std::vector<char> stdErrorBuffer;
  uPtrAVFormatContext fc;
  char* latestBufferPtr;
};
}  // namespace metadata

#endif

#include "avDumpFormat.h"
#include "appinfo.h"
#include "customExceptions.h"
#include "ffmpegUPtr.h"

namespace metadata {

avDumpFormat::avDumpFormat(const std::string& mediaSourceString, int i)
    : index(i), fc(AVFormatContextConstructor()), latestBufferPtr(nullptr) {
  AVFormatContext* temp = fc.get();
  if (avformat_open_input(&temp, mediaSourceString.c_str(), nullptr, nullptr) < 0) {
    // so painfull but best, avformat_open_input deallocs fc
    fc.release();  // release uptr
    throw mediaSourceWrongException();
  }
  stdErrorBuffer.reserve(bufferSize);
  for (unsigned int n = 0; n < bufferSize; n++) {
    stdErrorBuffer.push_back('\0');
  }
  char* ptrToBuffer = &stdErrorBuffer[0];
  setvbuf(stderr, ptrToBuffer, _IOFBF, bufferSize);
  av_dump_format(fc.get(), i, mediaSourceString.c_str(), 0);
  latestBufferPtr = ptrToBuffer;
}

char* avDumpFormat::getBuffer() const {
  assert(latestBufferPtr);
  return latestBufferPtr;
}
}  // namespace metadata

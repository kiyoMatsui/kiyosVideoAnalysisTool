//

#include "customExceptions.h"
#include "avDumpFormat.h"
#include "ffmpegUPtr.h"

namespace qtWidgetsInterface {


avDumpFormat::avDumpFormat(std::string aString, int i) {
    fc = avformat_alloc_context();
    if(avformat_open_input(&fc, aString.c_str(), nullptr, nullptr) < 0)
    {
        //so painfull but best, avformat_open_input deallocs fc
        fcAllocFlag = false;
        throw mediaSourceWrongException();
    }
    stdErrorBuffer.reserve(bufferSize);
    for (unsigned int n = 0; n < bufferSize; n++ ) {
        stdErrorBuffer.push_back('\0'); //memset (&stdErrorBuffers,0,fc->nb_streams*bufferSize);
    }
    char * ptrToBuffer = &stdErrorBuffer[0];
    setbuf(stderr, ptrToBuffer);
    av_dump_format(fc, i, aString.c_str(), 0); // i was n
    latestBufferPtr = ptrToBuffer;
}

char* avDumpFormat::getBuffer() const {
  return latestBufferPtr;
}

}


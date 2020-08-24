#ifndef __LEARNING_FFMPEG_STREAM_PAESER_H__
#define __LEARNING_FFMPEG_STREAM_PAESER_H__

#include <cstdint>
#include <memory>

namespace ff {

class IStreamParser {
public:
    virtual int parse(const char* uri) = 0;
};

}  // namespace ff

#endif  // __LEARNING_FFMPEG_STREAM_PAESER_H__
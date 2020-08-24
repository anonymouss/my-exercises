#ifndef __LEARNING_FFMPEG_METADATA_PAESER_H__
#define __LEARNING_FFMPEG_METADATA_PAESER_H__

#include "StreamParser.h"

#include <fstream>

namespace ff {

class AvcParser : public IStreamParser {
public:
    enum NaluType : uint32_t {
        NALU_TYPE_UNKNOWN = 0,
        NALU_TYPE_SLICE,
        NALU_TYPE_DPA,
        NALU_TYPE_DPB,
        NALU_TYPE_DPC,
        NALU_TYPE_IDR,
        NALU_TYPE_SEI,
        NALU_TYPE_SPS,
        NALU_TYPE_PPS,
        NALU_TYPE_AUD,
        NALU_TYPE_EOSEQ,
        NALU_TYPE_EOSTREAM,
        NALU_TYPE_FILL,
    };

    enum NaluRefIdc : uint32_t {
        NALU_PRIORITY_DISPOSABLE = 0,
        NALU_PRIORITY_LOW,
        NALU_PRIORITY_HIGH,
        NALU_PRIORITY_HIGHEST,
    };

    struct nalu_t {
        int32_t startcodeprefix_len;  //!< 4 for parameter sets and first slice in picture, 3 for
                                      //!< everything else (suggested)
        uint32_t len;  //!< Length of the NAL unit (Excluding the start code, which does not belong
                       //!< to the NALU)
        uint32_t max_size;             //!< NAL Unit Buffer size
        int32_t forbidden_bit;         //!< should be always FALSE
        NaluRefIdc nal_reference_idc;  //!< NALU_PRIORITY_xxxx
        NaluType nal_unit_type;        //!< NALU_TYPE_xxxx
        char *buf;                     //!< contains the first byte followed by the EBSP
    };

    AvcParser() = default;
    virtual ~AvcParser();
    int parse(const char *uri) final;

private:
    int getAnnexbNalu(nalu_t *nalu);

    std::fstream mAvcFile;
};

}  // namespace ff

#endif  // __LEARNING_FFMPEG_METADATA_PAESER_H__
#include "AvcParser.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>

namespace ff {

constexpr uint32_t MAX_BUFFER_SIZE = UINT32_MAX;

AvcParser::~AvcParser() {
    if (mAvcFile.is_open()) { mAvcFile.close(); }
}

static bool isStartCode3(const char *buf) {
    if (!buf) return false;
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1) {
        return true;
    } else {
        return false;
    }
}

static bool isStartCode4(const char *buf) {
    if (!buf) return false;
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1) {
        return true;
    } else {
        return false;
    }
}

int AvcParser::getAnnexbNalu(nalu_t *nalu) {
    int pos = 0;
    auto *buf = new char[nalu->max_size];
    if (buf == nullptr) {
        printf("ERROR: failed to allocate buf\n");
        return 0;
    }

    nalu->startcodeprefix_len = 3;
    if (!mAvcFile.read(buf, 3) || mAvcFile.gcount() < 3) {
        printf("ERROR: not enough data to read\n");
        delete[] buf;
        return 0;
    }

    if (isStartCode3(buf)) {
        nalu->startcodeprefix_len = 3;
        pos = 3;
    } else {
        if (!mAvcFile.read(buf + 3, 1) || mAvcFile.gcount() < 1) {
            printf("ERROR: not enough data to read\n");
            delete[] buf;
            return 0;
        }
        if (isStartCode4(buf)) {
            nalu->startcodeprefix_len = 4;
            pos = 4;
        } else {
            printf("ERROR: incorrect startcode type\n");
            delete[] buf;
            return 0;
        }
    }

    bool foundNextStartCode = false;
    bool type3 = false, type4 = false;
    int rewind = 0;
    while (!foundNextStartCode) {
        if (mAvcFile.eof()) {
            nalu->len = (pos - 1) - nalu->startcodeprefix_len;
            memcpy(nalu->buf, &buf[nalu->startcodeprefix_len], nalu->len);
            nalu->forbidden_bit = nalu->buf[0] & 0x80;                               // 1 bit
            nalu->nal_reference_idc = static_cast<NaluRefIdc>(nalu->buf[0] & 0x60);  // 2 bit
            nalu->nal_unit_type = static_cast<NaluType>((nalu->buf[0]) & 0x1f);      // 5 bit
            delete[] buf;
            // printf("INFO: EOF\n");
            return pos - 1;
        }
        buf[pos++] = mAvcFile.get();
        type4 = isStartCode4(&buf[pos - 4]);
        type3 = isStartCode3(&buf[pos - 3]);
        if (type3 || type4) { foundNextStartCode = true; }
    }

    rewind = type3 ? -3 : -4;
    mAvcFile.seekg(rewind, std::ios_base::cur);

    nalu->len = (pos + rewind) - nalu->startcodeprefix_len;
    memcpy(nalu->buf, &buf[nalu->startcodeprefix_len], nalu->len);
    nalu->forbidden_bit = nalu->buf[0] & 0x80;                               // 1 bit
    nalu->nal_reference_idc = static_cast<NaluRefIdc>(nalu->buf[0] & 0x60);  // 2 bit
    nalu->nal_unit_type = static_cast<NaluType>((nalu->buf[0]) & 0x1f);      // 5 bit
    delete[] buf;

    return (pos + rewind);
}

int AvcParser::parse(const char *uri) {
    if (mAvcFile.is_open()) {
        printf("INFO: Closing current file...\n");
        mAvcFile.close();
    }

    mAvcFile.open(uri, std::ios::in | std::ios::binary);
    if (!mAvcFile.is_open()) {
        printf("ERROR: Failed to open file %s\n", uri);
        return -1;
    }

    auto *naluBuf = new nalu_t;
    if (naluBuf == nullptr) {
        printf("ERROR: Failed to allocate nalu buffer\n");
        return -1;
    }
    naluBuf->max_size = MAX_BUFFER_SIZE;
    naluBuf->buf = new char[naluBuf->max_size];
    if (naluBuf->buf == nullptr) {
        printf("ERROR: Failed to allocate buffers in nalu\n");
        delete naluBuf;
        naluBuf = nullptr;
        return -1;
    }

    int offset = 0;
    int nal_num = 0;

    printf("-----+----- NALU Table -+-------+---------+\n");
    printf(" NUM |    POS  |  IDC   |  TYPE |    LEN  |\n");
    printf("-----+---------+--------+-------+---------+\n");

    while (!mAvcFile.eof()) {
        auto dataLength = getAnnexbNalu(naluBuf);
        char type_str[20] = {0};
        switch (naluBuf->nal_unit_type) {
            case NaluType::NALU_TYPE_SLICE: std::sprintf(type_str, "SLICE"); break;
            case NaluType::NALU_TYPE_DPA: std::sprintf(type_str, "DPA"); break;
            case NaluType::NALU_TYPE_DPB: std::sprintf(type_str, "DPB"); break;
            case NaluType::NALU_TYPE_DPC: std::sprintf(type_str, "DPC"); break;
            case NaluType::NALU_TYPE_IDR: std::sprintf(type_str, "IDR"); break;
            case NaluType::NALU_TYPE_SEI: std::sprintf(type_str, "SEI"); break;
            case NaluType::NALU_TYPE_SPS: std::sprintf(type_str, "SPS"); break;
            case NaluType::NALU_TYPE_PPS: std::sprintf(type_str, "PPS"); break;
            case NaluType::NALU_TYPE_AUD: std::sprintf(type_str, "AUD"); break;
            case NaluType::NALU_TYPE_EOSEQ: std::sprintf(type_str, "EOSEQ"); break;
            case NaluType::NALU_TYPE_EOSTREAM: std::sprintf(type_str, "EOSTREAM"); break;
            case NaluType::NALU_TYPE_FILL: std::sprintf(type_str, "FILL"); break;
            default: std::sprintf(type_str, "?"); break;
        }
        char idc_str[20] = {0};
        switch (naluBuf->nal_reference_idc >> 5) {
            case NaluRefIdc::NALU_PRIORITY_DISPOSABLE: std::sprintf(idc_str, "DISPOS"); break;
            case NaluRefIdc::NALU_PRIORITY_LOW: std::sprintf(idc_str, "LOW"); break;
            case NaluRefIdc::NALU_PRIORITY_HIGH: std::sprintf(idc_str, "HIGH"); break;
            case NaluRefIdc::NALU_PRIORITY_HIGHEST: std::sprintf(idc_str, "HIGHEST"); break;
            default: std::sprintf(idc_str, "?"); break;
        }

        printf("%5d| %8d| %7s| %6s| %8d|\n", nal_num, offset, idc_str, type_str, naluBuf->len);

        offset += dataLength;
        nal_num++;
    }

    if (naluBuf) {
        if (naluBuf->buf) {
            delete[] naluBuf->buf;
            naluBuf->buf = nullptr;
        }
        delete naluBuf;
        naluBuf = nullptr;
    }
    return 0;
}

}  // namespace ff
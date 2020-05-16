#ifndef __ANDROID_YV12_HPP__
#define __ANDROID_YV12_HPP__

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#define ALIGN(x, y) (((x) + y - 1) & (~(y - 1)))

constexpr uint8_t kSampleY = 120;
constexpr uint8_t kSampleU = 160;
constexpr uint8_t kSampleV = 200;

// https://developer.android.com/reference/android/graphics/ImageFormat#YV12
class YV12 {
public:
    YV12(uint32_t width, uint32_t height, uint32_t frames = 1) {
        mWidth = width;
        mHeight = height;
        mYStride = ALIGN(mWidth, 16);
        mUVStride = ALIGN(mYStride / 2, 16);
        mBufferSize = (mYStride + mUVStride) * mHeight;

        mFrames = frames;
    }

    bool dump() {
        if (!check()) { return false; }
        const char *filename = "YV12_aligned.yuv";
        const char *rawFilename = "YV12_raw.yuv";
        std::fstream falign(filename, std::ios::trunc | std::ios::binary | std::ios::out);
        std::fstream fraw(rawFilename, std::ios::trunc | std::ios::binary | std::ios::out);

        if (!falign.is_open() || !fraw.is_open()) {
            std::cout << "failed to open file to write" << std::endl;
            falign.close();
            fraw.close();
            return false;
        }
        falign.seekp(0);
        fraw.seekp(0);
        uint8_t data[mBufferSize];
        for (auto i = 0; i < mFrames; ++i) {
            if (!generateFrame(data, i)) {
                std::cout << "Failed to generate frame : " << i << std::endl;
                falign.close();
                fraw.close();
                return false;
            }
            // dump aligned
            std::cout << "dumping alinged frame " << i << std::endl;
            falign.write(reinterpret_cast<char *>(data), mBufferSize);
            // dump raw
            auto writePlane = [&fraw](uint8_t *src, uint32_t wBytes, uint32_t strideBytes,
                                      uint32_t nLines) {
                for (auto i = 0; i < nLines; ++i) {
                    fraw.write(reinterpret_cast<char *>(src), wBytes);
                    src += strideBytes;
                }
            };
            std::cout << "dumping raw frame " << i << std::endl;
            // Y
            auto *base = data;
            writePlane(base, mWidth, mYStride, mHeight);
            // UV
            base += (mYStride * mHeight);
            writePlane(base, mWidth / 2, mUVStride, mHeight);
        }
        std::cout << "done!" << std::endl;
        falign.close();
        fraw.close();
        return true;
    }

private:
    bool generateFrame(uint8_t *data, uint32_t frameId) {
        if (!data) {
            std::cout << "empty data" << std::endl;
            return false;
        }
        std::memset(data, 0, mBufferSize);

        std::cout << "generating frame " << frameId << std::endl;
        int W = 0, H = 0, uvW = 0, uvH = 0;
        auto index = frameId % 4;
        if (index < 2) {
            W = index * (mWidth / 2);
            H = 0;
            uvW = index * (mWidth + 1) / 4;
            uvH = 0;
        } else {
            W = (3 - index) * (mWidth / 2);
            H = mHeight / 2;
            uvW = (3 - index) * (mWidth + 1) / 4;
            uvH = mHeight / 4;
        }

        // y plane
        for (auto h = H; h < H + (mHeight / 2); ++h) {
            for (auto w = W; w < W + (mWidth / 2); ++w) { data[h * mYStride + w] = kSampleY; }
        }
        // uv plane
        auto v_offset = mYStride * mHeight;
        auto u_offset = mUVStride * mHeight / 2;
        for (auto h = uvH; h < uvH + (mHeight + 1) / 4; ++h) {
            for (auto w = uvW; w < uvW + (mWidth + 1) / 4; ++w) {
                data[v_offset + h * mUVStride + w] = kSampleV;
                data[v_offset + u_offset + h * mUVStride + w] = kSampleU;
            }
        }

        return true;  // OK
    }

    bool check() const {
        if (mFrames == 0 || mWidth % 2 || mWidth % 2) {
            std::cout << "Invalid parameter" << std::endl;
            return false;
        }
        return true;
    }

    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mFrameSize;
    uint32_t mYStride;
    uint32_t mUVStride;
    uint32_t mBufferSize;

    uint32_t mFrames;
};

#endif  // __ANDROID_YV12_HPP__
#include "AvcParser.h"

constexpr const char* avcFileName = "../media/720p.264";

int main() {
    ff::AvcParser avcParser;
    avcParser.parse(avcFileName);
}
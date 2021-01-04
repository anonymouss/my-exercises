#include "include/Data.h"

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

namespace util {

constexpr const unsigned kTrainSetSize = 10000;

TrainingData::TrainingData(const std::string filename) {
    mFile.open(filename.c_str());
    if (mFile.is_open()) {
        if (mFile.peek() == std::ifstream::traits_type::eof()) {
            std::cout << "empty file... regenerate training data" << std::endl;
            mFile.close();
            std::ofstream of{filename.c_str()};
            generateTrainingData(of);
            of.close();
            mFile.open(filename.c_str());
        }
    } else {
        // std::cout << "open file failed : " << errno << std::endl;
        mFile.close();
        std::ofstream of{filename.c_str()};
        generateTrainingData(of);
        of.close();
        mFile.open(filename.c_str());
    }

    if (!mFile.is_open()) {
        std::cout << "open file failed : " << errno << std::endl;
    }
}

TrainingData::~TrainingData() {
    if (mFile.is_open()) {
        mFile.close();
    }
}

bool TrainingData::isEof() const {
    return mFile.eof();
}

void TrainingData::getTopology(std::vector<unsigned> *topology) {
    std::string line, label;
    std::getline(mFile, line);
    std::stringstream ss(line);
    ss >> label;
    if (isEof() || label.compare("topology:") != 0) {
        abort();
    }
    while (!ss.eof()) {
        unsigned n;
        ss >> n;
        topology->push_back(n);
    }
}

unsigned TrainingData::getNextInputs(std::vector<double> *inputs) {
    inputs->clear();
    std::string line, label;
    std::getline(mFile, line);
    std::stringstream ss(line);
    ss >> label;
    if (label.compare("in:") == 0) {
        double v;
        while (ss >> v) {
            inputs->push_back(v);
        }
    }
    return inputs->size();
}

unsigned TrainingData::getTargetOutputs(std::vector<double> *targets) {
    targets->clear();
    std::string line, label;
    std::getline(mFile, line);
    std::stringstream ss(line);
    ss >> label;
    if (label.compare("out:") == 0) {
        double v;
        while (ss >> v) {
            targets->push_back(v);
        }
    }
    return targets->size();
}

void TrainingData::generateTrainingData(std::ostream &of) {
    of << "topology: 2 4 1" << std::endl;
    for (auto i = 0; i < kTrainSetSize; ++i) {
        auto a = static_cast<int>(2.0 * rand() / double(RAND_MAX));
        auto b = static_cast<int>(2.0 * rand() / double(RAND_MAX));
        auto y = a ^ b;
        of << "in: " << a <<".0 " << b << ".0" << std::endl;
        of << "out: " << y << ".0" << std::endl;
    }
}

} // namespace util
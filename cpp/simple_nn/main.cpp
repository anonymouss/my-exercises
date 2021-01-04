#include <Net.h>
#include <Data.h>

#include <vector>
#include <iostream>
#include <cassert>

constexpr const char * TRAIN_DATA = "test.txt";

int main() {
    util::TrainingData trainData(TRAIN_DATA);

    std::vector<unsigned> topology;
    trainData.getTopology(&topology);

    for (auto i = 0; i < topology.size(); ++i) {
        std::cout << topology[i] << std::endl;
    }

    auto dispVec = [] (const char *prefix, const std::vector<double> &vec) {
        std::cout << prefix << ": ";
        for (const auto &v : vec) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    };

    snn::Net net(topology);
    std::vector<double> input, target, result;
    int iteration = 0;
    while (!trainData.isEof()) {
        ++iteration;
        std::cout << "iter: " << iteration << std::endl;
        if (trainData.getNextInputs(&input) != topology[0]) {
            break;
        }
        dispVec("input", input);
        net.feedForward(input);
        net.getResults(&result);
        dispVec("output", result);
        trainData.getTargetOutputs(&target);
        dispVec("target", target);
        assert(target.size() == topology.back());
        net.backPropagate(target);
        std::cout << "recent avg error: " << net.getRecentAverageError() << std::endl;
    }
    std::cout << "Done." << std::endl;
}
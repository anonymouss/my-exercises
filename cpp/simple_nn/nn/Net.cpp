#include "include/Net.h"
#include "include/Debug.h"
#include "include/Neuron.h"

#include <iostream>
#include <cassert>
#include <cmath>

namespace snn {

Net::Net(const std::vector<unsigned> &topology) {
    auto numLayers = topology.size();
    for (auto layerNum = 0; layerNum < numLayers; ++layerNum) {
        mLayers.emplace_back(Layer());
        auto numOutputs = layerNum == (numLayers - 1) ? 0 : topology[layerNum + 1];
        auto &layer = mLayers.back();
        for (auto neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
            layer.emplace_back(Neuron(numOutputs, neuronNum));
            gLogger << "made neuron - " << neuronNum << std::endl;
        }
        // bias node
        layer.back().setOutputValue(1.0);
        gLogger << "made layer - " << layerNum << std::endl;
    }

    mRecentAvgError = 0.0;
    mRecentAvgSmoothFactor = 0.0;
}

Net::~Net() {}

void Net::feedForward(const std::vector<double> &inputVals) {
    assert(inputVals.size() == mLayers[0].size() - 1);

    for (auto i = 0; i < inputVals.size(); ++i) {
        mLayers[0][i].setOutputValue(inputVals[i]);
    }

    // feed forward
    for (auto layerNum = 1; layerNum < mLayers.size(); ++layerNum) {
        auto &prevLayer = mLayers[layerNum - 1];
        for (auto neuronNum = 0; neuronNum < mLayers[layerNum].size() - 1; ++ neuronNum) {
            mLayers[layerNum][neuronNum].feedForward(prevLayer);
        }
    }
}

void Net::backPropagate(const std::vector<double> &targetVals) {
    assert(targetVals.size() == mLayers.back().size() - 1);

    // calculate overall error (RMSE)
    Layer &outputLayer = mLayers.back();
    mError = 0.0;

    for (auto n = 0; n < outputLayer.size() - 1; ++n) {
        // diff between target & prediction
        auto delta = targetVals[n] - outputLayer[n].getOutputValue();
        mError = delta * delta;
    }
    mError /= outputLayer.size() - 1; // average error squared
    mError = std::sqrt(mError); // RMSE

    mRecentAvgError = (mRecentAvgError * mRecentAvgSmoothFactor + mError)
            / (mRecentAvgSmoothFactor + 1.0);

    // calcualte output layer gradients
    for (auto n = 0; n < outputLayer.size() - 1; ++n) {
        outputLayer[n].calcOutputGradients(targetVals[n]);
    }

    // calculate hidden layer gradients
    for (auto layerNum = mLayers.size() - 2; layerNum > 0; --layerNum) {
        auto &hiddenLayer = mLayers[layerNum];
        auto &nextLayer = mLayers[layerNum + 1];

        for (auto n = 0; n < hiddenLayer.size(); ++n) {
            hiddenLayer[n].calcHiddenGradients(nextLayer);
        }
    }

    // for all layers from outputs to first hidden layer. update connection weights
    for (auto layerNum = mLayers.size() - 1; layerNum > 0; --layerNum) {
        auto &layer = mLayers[layerNum];
        auto &prevLayer = mLayers[layerNum - 1];

        for (auto n = 0; n < layer.size() - 1; ++n) {
            layer[n].updateInputWeights(&prevLayer);
        }
    }
}

void Net::getResults(std::vector<double> *resultVals) const {
    resultVals->clear();

    for (auto n = 0; n < mLayers.back().size() - 1; ++n) {
        auto a = mLayers.back()[n].getOutputValue() > 0.5 ? 0.0 : 1.0;
		resultVals->push_back(a);
	}
}

double Net::getRecentAverageError() const {
    return mRecentAvgError;
}

} // namespace snn
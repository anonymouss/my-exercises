#include "include/Neuron.h"
#include "include/Debug.h"

#include <cstdlib>
#include <cmath>
#include <string>

namespace snn {

constexpr const char * kReLu = "relu";
constexpr const char * kSigmoid = "sigmoid";
constexpr const char * kTanh = "tanh";

static const std::string ACTIVATION_TYPE(kSigmoid);

constexpr const double alpha = 0.15; // learnig rate
constexpr const double momentum = 0.5;

// static
double Neuron::randomWeight() {
    return rand() / double(RAND_MAX);
}

// static
double Neuron::activate(double x) {
    gLogger << "activation type = " << ACTIVATION_TYPE << std::endl;
    if (ACTIVATION_TYPE == kTanh) {
        x = std::tanh(x);
    } else if (ACTIVATION_TYPE == kSigmoid) {
        x = 1 / (1 + std::exp(-x));
    } else if (ACTIVATION_TYPE == kReLu) {
        x = std::max(0.0, x);
    }
    return x;
}

// static
double Neuron::activateD(double x) {
    if (ACTIVATION_TYPE == kTanh) {
        x = 1.0 - std::tanh(x) * std::tanh(x);
    } else if (ACTIVATION_TYPE == kSigmoid) {
        x = (1 / (1 + std::exp(-x))) * (1 - (1 / (1 + std::exp(-x))));
    } else if (ACTIVATION_TYPE == kReLu) {
        x = x > 0.0 ? 1.0 : 0.0;
    }
    return x;
}

Neuron::Neuron(const unsigned numOutputs, unsigned index) {
    for (auto c = 0; c < numOutputs; ++c) {
        Connection connection;
        connection.weight = Neuron::randomWeight();
        mOutputWeights.push_back(connection);
    }
    mIndex = index;
}

Neuron::~Neuron() {}

void Neuron::setOutputValue(double value) {
    mOutputVal = value;
}

double Neuron::getOutputValue() const {
    return mOutputVal;
}

double Neuron::getWeight() const {
    return mOutputWeights[mIndex].weight;
}

void Neuron::feedForward(const Layer &prevLayer) {
    double sum = 0.0;
    for (auto n = 0; n < prevLayer.size(); ++n) {
        sum += prevLayer[n].getOutputValue() * prevLayer[n].getWeight();
    }

    mOutputVal = Neuron::activate(sum);
}

void Neuron::calcOutputGradients(double targetVal) {
    auto delta = targetVal - mOutputVal;
    mGradient = delta * Neuron::activateD(mOutputVal);
}

void Neuron::calcHiddenGradients(const Layer &nextLayer) {
    // delta of weights
    auto dow = sumDOW(nextLayer);
    mGradient = dow * Neuron::activateD(mOutputVal);
}

void Neuron::updateInputWeights(Layer *inputLayer) {
    for (auto n = 0; n < inputLayer->size(); ++n) {
        auto &neuron = (*inputLayer)[n];
        auto oldDeltaWeight = neuron.mOutputWeights[mIndex].deltaWeight;
        auto newDeltaWeight =
                // individual input, magnidied by the gradient and train rate
                alpha * neuron.getOutputValue() * mGradient + momentum * oldDeltaWeight;

        neuron.mOutputWeights[mIndex].deltaWeight = newDeltaWeight;
        neuron.mOutputWeights[mIndex].weight += newDeltaWeight;
    }
}

double Neuron::sumDOW(const Layer &layer) const {
    double sum = 0.0;
    for (auto n = 0; n < layer.size() - 1; ++n) {
        sum += mOutputWeights[n].weight * layer[n].mGradient;
    }

    return sum;
}

} // namespace snn
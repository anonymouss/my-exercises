#ifndef __NEURON_H__
#define __NEURON_H__

#include <vector>

namespace snn {

class Neuron;
using Layer = std::vector<Neuron>;

struct Connection {
    double weight;
    double deltaWeight;
};

class Neuron {
public:
    explicit Neuron(const unsigned numOutputs, unsigned index);
    virtual ~Neuron();

    void setOutputValue(double value);
    double getOutputValue() const;
    double getWeight() const;
    // double getDeltaWeight() const;
    void feedForward(const Layer &prevLayer);
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const Layer &nextLayer);
    void updateInputWeights(Layer *inputLayer);

private:
    static double randomWeight();
    static double activate(double x);
    // Derivative
    static double activateD(double x);
    double sumDOW(const Layer &layer) const;

    double mOutputVal;
    std::vector<Connection> mOutputWeights;
    unsigned mIndex;
    double mGradient;
};

} // namespace snn

#endif // __NEURON_H__
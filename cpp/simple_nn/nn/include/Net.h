#ifndef __NET_H__
#define __NET_H__

#include <vector>

namespace snn {

class Neuron;
using Layer = std::vector<Neuron>;

class Net {
public:
    explicit Net(const std::vector<unsigned> &topology);
    virtual ~Net();

    void feedForward(const std::vector<double> &inputVals);
    void backPropagate(const std::vector<double> &targetVals);
    void getResults(std::vector<double> *resultVals) const;

    double getRecentAverageError() const;

private:
    std::vector<Layer> mLayers; // mLayers[layerNum][neuronNum]
    double mError;
    double mRecentAvgError;
    double mRecentAvgSmoothFactor;
};

} // namespace snn

#endif // __NET_H__
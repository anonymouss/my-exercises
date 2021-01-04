#ifndef __DATA_H__
#define __DATA_H__

#include <vector>
#include <fstream>

namespace util {

class TrainingData {
public:
    TrainingData(const std::string filename);
    virtual ~TrainingData();

    bool isEof() const;
    void getTopology(std::vector<unsigned> *topology);
    unsigned getNextInputs(std::vector<double> *inputs);
    unsigned getTargetOutputs(std::vector<double> *targets);  

private:
    void generateTrainingData(std::ostream &of);

    std::ifstream mFile;
};

} // namespace data

#endif // __DATA_H__
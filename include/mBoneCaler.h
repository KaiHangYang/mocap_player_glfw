#ifndef M_BONE_CALER
#define M_BONE_CALER

#include "mDefs.h"

class mBoneCaler {
public:
    // Bone type 0 means using the common bones, 1 means using the vnect bones
    mBoneCaler(int bone_type=0);
    void calBoneLens(std::vector<float> points);
    std::vector<double> getBoneLens();
    void setReady(bool is_ready);
    bool getReady();
    void printBoneLens();

private:
    bool cal_ready;
    int bone_type;
    int bone_num;
    std::vector<float> calBoneLensFromPoints(std::vector<float> points);
    std::vector<float> bones_length;
    std::vector<unsigned int> bone_means;
    std::vector<unsigned int> bone_indices;
};

#endif

#ifndef M_DEFS
#define M_DEFS
#include <vector>

namespace MDEFS {
    static const int num_of_joints_vnect = 17;
    static const int num_of_joints = 15;
    static const int num_of_bones = 14;
    static const int num_of_bones_vnect = 16;
    static const std::vector<unsigned int> bone_indices_vnect({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            1, 5,
            5, 6,
            6, 7,
            1, 14,
            8, 14,
            8, 9,
            9, 10,
            14, 11,
            11, 12,
            12, 13,
            10, 15,
            13, 16
    });
    
    static const std::vector<unsigned int> bone_indices({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            1, 5,
            5, 6,
            6, 7,
            1, 14,
            14, 8,
            8, 9,
            9, 10,
            14, 11,
            11, 12,
            12, 13,
    });

    static const std::vector<unsigned int> bone_means({
            1, 4,
            2, 5,
            3, 6,
            8, 11,
            9, 12,
            10, 13
            });

    static const std::vector<unsigned int> bone_means_vnect({
            1, 4,
            2, 5,
            3, 6,
            8, 11,
            9, 12,
            10, 13,
            14, 15
            });
}

#endif

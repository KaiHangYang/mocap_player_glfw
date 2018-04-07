#include "../../include/mBoneCaler.h"
#include <algorithm>
#include <iostream>

mBoneCaler::mBoneCaler(int bone_type) {
    this->bone_type = bone_type;
    this->cal_ready = false;

    if (this->bone_type == 0) {
        this->bone_num = MDEFS::num_of_bones;
        this->bone_means = MDEFS::bone_means;
        this->bone_indices = MDEFS::bone_indices;
    }
    else if (this->bone_type == 1) {
        this->bone_num = MDEFS::num_of_bones_vnect;
        this->bone_means = MDEFS::bone_means_vnect;
        this->bone_indices = MDEFS::bone_indices_vnect;
    }
    else {
        std::cout << "(mBoneCaler.cpp)bone_type is not exits" << std::endl;
        exit(-1);
    }
}

void mBoneCaler::printBoneLens() {
    for (int i = 0; i < this->bones_length.size(); ++i) {
        std::cout << i << ": "  << this->bones_length[i] << std::endl;
    }
}

std::vector<float> mBoneCaler::calBoneLensFromPoints(std::vector<float> points) {
    int point_a, point_b;
    float a_x, a_y, a_z, b_x, b_y, b_z;
    int tmp_index;
    unsigned int * b_index_ptr;
    std::vector<float> bones_len(this->bone_num, 0);

    for (int i = 0; i < bone_num; ++i) {
        point_a = this->bone_indices[2*i];
        point_b = this->bone_indices[2*i + 1];
        tmp_index = point_a * 3;
        a_x = points[tmp_index + 0];
        a_y = points[tmp_index + 1];
        a_z = points[tmp_index + 2];


        tmp_index = point_b * 3;
        b_x = points[tmp_index + 0];
        b_y = points[tmp_index + 1];
        b_z = points[tmp_index + 2];

        bones_len[i] = std::sqrt((a_x - b_x) * (a_x - b_x) + (a_y - b_y) * (a_y - b_y) + (a_z - b_z) * (a_z - b_z));
    }
    return bones_len;
}

void mBoneCaler::setReady(bool is_ready) {
    if (!is_ready) {
        this->bones_length.clear();
        this->cal_ready = is_ready;
    }

    if (is_ready && !this->bones_length.empty()) {
        this->cal_ready = is_ready;
    }
}
bool mBoneCaler::getReady() {
    return this->cal_ready;
}
std::vector<double> mBoneCaler::getBoneLens() {
    std::vector<double> result(this->bones_length.size(), 0);
    std::copy(this->bones_length.begin(), this->bones_length.end(), result.begin());
    return result;
}

void mBoneCaler::calBoneLens(std::vector<float> points) {
    if (!this->cal_ready) {
        std::vector<float> cur_bone_length = this->calBoneLensFromPoints(points);
        int mean_size = this->bone_means.size() / 2;
        const unsigned int * mean_ptr = &this->bone_means[0];
        unsigned int m_a, m_b;
        // cal the mean of both side of the body
        for (int i =0; i < mean_size; ++i) {
            m_a = *(mean_ptr++);
            m_b = *(mean_ptr++);
            cur_bone_length[m_a] = (cur_bone_length[m_a] + cur_bone_length[m_b]) / 2;
            cur_bone_length[m_b] = cur_bone_length[m_a];
        }

        if (this->bones_length.empty()) {
            this->bones_length = cur_bone_length;
        }
        else {
            for (int i = 0; i < this->bone_num; ++i) {
                this->bones_length[i] = (this->bones_length[i] + cur_bone_length[i]) / 2;
            }
        }
    }
    else {
        std::cout << "The bonelength is already ready for using!" << std::endl;
    }
}

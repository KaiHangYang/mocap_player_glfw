#ifndef M_CAL_UTILS
#define M_CAL_UTILS

#include <glm/glm.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

namespace MCU {

    void getRotateMat(float x_angle, float y_angle, float z_angle, glm::mat3 & r_mat);
    void getCamExMat(std::vector<float> omc_ground, std::vector<float> tc_ground, glm::mat4 & cam_ex_mat);

};
#endif

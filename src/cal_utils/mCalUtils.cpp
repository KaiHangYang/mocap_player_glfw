#include "../../include/mCalUtils.h"
#include <algorithm>

#include "../../include/mDebugUtils.h"

namespace MCU {

void getRotateMat(float x_angle, float y_angle, float z_angle, glm::mat3 & r_mat) {
    float cos_gama = std::cos(x_angle);
    float sin_gama = std::sin(x_angle);

    float cos_beta = std::cos(y_angle);
    float sin_beta = std::sin(y_angle);

    float cos_alpha = std::cos(z_angle);
    float sin_alpha = std::sin(z_angle);

    r_mat[0][0] = cos_alpha * cos_beta;  r_mat[1][0] = cos_alpha * sin_beta * sin_gama - sin_alpha * cos_gama;  r_mat[2][0] = cos_alpha * sin_beta * cos_gama + sin_alpha * sin_gama;
    r_mat[0][1] = sin_alpha * cos_beta;  r_mat[1][1] = sin_alpha * sin_beta * sin_gama + cos_alpha * cos_gama;  r_mat[2][1] = sin_alpha * sin_beta * cos_gama - cos_alpha * sin_gama;
    r_mat[0][2] = -sin_beta;             r_mat[1][2] = cos_beta*sin_gama;                                       r_mat[2][2] = cos_beta * cos_gama;
}

// This function get the row-major storage for cam_ex_mat
void getCamExMat(std::vector<float> omc_ground, std::vector<float> tc_ground, glm::mat4 & cam_ex_mat) {
    glm::mat3 rotate_mat;
    cv::Mat rotate_vec_cv = cv::Mat::zeros(1, 3, CV_32F);
    float *r_vec_ptr = rotate_vec_cv.ptr<float>(0);
    r_vec_ptr[0] = omc_ground[0];
    r_vec_ptr[1] = omc_ground[1];
    r_vec_ptr[2] = omc_ground[2];

    cv::Mat rotate_mat_cv = cv::Mat::zeros(3, 3, CV_32F);
    cv::Rodrigues(rotate_vec_cv, rotate_mat_cv);
    memcpy(&rotate_mat[0], rotate_mat_cv.ptr<float>(0), sizeof(float) * 9);

    cam_ex_mat = glm::mat4(rotate_mat);

    cam_ex_mat[0][3] = tc_ground[0];
    cam_ex_mat[1][3] = tc_ground[1];
    cam_ex_mat[2][3] = tc_ground[2];

    // Here I need to get cam_ex_mat * exch_mat, but the glm::mat4 is col major, so you know, I get below
    glm::mat4 exch_mat = glm::mat4({0, 0, 1, 0,
                                    1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 0, 1});
    cam_ex_mat = exch_mat * cam_ex_mat;
}

}

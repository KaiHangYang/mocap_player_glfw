#ifndef M_OUR_OPT
#define M_OUR_OPT
#include <ceres/ceres.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>


namespace mOurOpt {
    
    std::vector<double> optimize(std::vector<double> points_2d, std::vector<double> points_depth, std::vector<double> prev_points_3d, const double * i_proj_mat, std::vector<double> i_bones_length, std::vector<double> &last_params);
    std::vector<double> skeletonOptimize(std::vector<double> points_2d, std::vector<double> points_depth, const double * i_proj_mat, std::vector<double> &last_params);

    std::vector<double> skeletonDzfOptimize(std::vector<double> points_2d, std::vector<double> points_dzf, const double * i_proj_mat, std::vector<double> &last_params);
    std::vector<double> dzfOptimize(std::vector<double> points_2d, std::vector<double> points_dzf, std::vector<double> prev_points_3d, const double * i_proj_mat, std::vector<double> i_bones_length, std::vector<double> &last_params);

};
#endif

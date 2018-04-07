#ifndef M_POSEMODEL
#define M_POSEMODEL

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "mShader.h"
#include "mMeshReader.h"

class mPoseModel {
public:

    mPoseModel(GLuint vao, mShader * pose_shader, mShader * depth_shader, glm::mat4 cam_in_mat, std::string model_path, float target_model_size, bool is_ar, int pose_type=0);
    ~mPoseModel();

    void draw(std::vector<float> points, glm::mat4 raw_cam_ex_mat_inverse, glm::mat4 & cam_ex_mat, int render_type=0, int shadow_sampler_id = 0);
    void draw(std::vector<float> points, int root_num);
    void setRotate(glm::mat4 rotate_mat);
    void resetRotate();

    // Debug method
    void drawOne(std::vector<float> root_pos, glm::mat4 cam_ex_mat);

    /***************Pose parameters********************/
    std::vector<unsigned int> bone_indices;
    int num_of_joints;
private:
    void renderPose(std::vector<float> &vertexs, glm::mat4 view_mat, int render_type = 0, int shadow_sampler_id = 0);
    void renderPose(std::vector<float> &vertexs);

    mMeshReader *mesh_reader;

    float model_size;
    float model_scale;

    mShader * depth_shader;
    mShader * pose_shader;

    glm::mat4 rotate_mat;
    glm::mat4 proj_mat;
    bool is_ar;

    GLuint VAO;

};

#endif

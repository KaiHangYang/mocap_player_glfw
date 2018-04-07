#ifndef SCENE_UTILS
#define SCENE_UTILS

#include <glm/glm.hpp>
#include "mOpenGLUtils.h"
#include "mPoseModel.h"
#include "mShader.h"

#include <opencv2/core/core.hpp>

class mSceneUtils {
public:
    mSceneUtils(int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, int pose_type=0, std::string wnd_title = std::string("Scene"));
    ~mSceneUtils();

    bool windowShouldClose();
    void render(std::vector<float> points_3d = std::vector<float>(0, 0));

    void setSurround(bool do_surround, glm::vec3 surround_center = glm::vec3(0.0,0.0,0.0));
private:

    GLuint genTexture();
    void setTextureData(cv::Mat frame);
    std::vector<GLfloat> getGroundVertexs();
    std::vector<GLfloat> getGroundColor();
    void rotateExMat(glm::mat4 & ex_mat, glm::mat4 rotate_mat);
    void transExMat(glm::mat4 ex_mat);

    void surroundOnePoint(glm::mat4 & model_mat);

    int wnd_width;
    int wnd_height;
    int ground_col;
    int ground_row;
    int array_size;
    float ground_size;

    GLuint VAO;

    cv::Mat ground_texture;

    GLuint ground_vbo;
    GLuint ground_cbo;
    GLuint ground_tbo;

    GLuint shadow_fbo;
    GLuint shadow_tbo;

    glm::mat4 cam_proj_mat;
    glm::mat4 cam_ex_mat;
    glm::mat4 cam_ex_mat_inverse;
    glm::mat4 cam_in_mat;
    glm::mat4 rotate_mat;
    glm::mat4 trans_mat;
    glm::mat4 cam_ex_t_mat;
    glm::mat4 cam_ex_r_mat;
    glm::mat4 cur_cam_ex_mat;
    glm::mat4 cur_cam_ex_t_mat;

    /***** center surround *****/
    glm::vec3 surround_center;
    bool do_use_surround;

    mShader * scene_shader;
    mShader * depth_shader;

    mPoseModel * pose_model;
    mOpenGLUtils * gl_session;


    //for debug
};

#endif

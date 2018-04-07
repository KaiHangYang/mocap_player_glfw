#ifndef M_CAMERA
#define M_CAMERA
#include <iostream>
#include "mShader.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
class mCamera {
public:
    cv::VideoCapture m_camera;
    int wndWidth;
    int wndHeight;
    mCamera(int wndWidth, int wndHeight, float base_fov, mShader *camShader);
    mCamera(int wndWidth, int wndHeight, float base_fov, std::string vs_path, std::string fs_path);
    ~mCamera();
    void drawFrame(cv::Mat &frame);
    glm::mat4 cam_view_mat;

private:
    bool init();
    float fov;
    GLuint VAO;
    mShader * camShader;
    GLuint vertexBuffer;
    GLuint uvBuffer;
    glm::mat4 MVP;
    GLuint textureID;
    GLuint samplerID;

    float far_plane;
    float near_plane;
    float frame_zpos;

    void initGLFrame();
    GLuint genTexture();
    void setTextureData(cv::Mat & frame);
};

#endif

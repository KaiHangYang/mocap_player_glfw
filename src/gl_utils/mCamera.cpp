#include "../../include/mCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

mCamera::~mCamera() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    delete this->camShader;
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
}
mCamera::mCamera(int wndWidth, int wndHeight, float base_fov, mShader *camShader) {
    this->wndWidth = wndWidth;
    this->wndHeight = wndHeight;
    this->camShader = camShader;
    this->fov = base_fov;
    this->far_plane = 100.0f;
    this->near_plane = 0.1f;
    this->frame_zpos = 99.f;
    glGenVertexArrays(1, &VAO);
    init();
}

mCamera::mCamera(int wndWidth, int wndHeight, float base_fov, std::string vs_path, std::string fs_path) {
    this->wndWidth = wndWidth;
    this->wndHeight = wndHeight;
    this->camShader = new mShader(vs_path, fs_path);
    this->fov = base_fov;
    this->far_plane = 100.0f;
    this->near_plane = 0.1f;
    this->frame_zpos = 99.f;
    glGenVertexArrays(1, &VAO);
    init();
}
bool mCamera::init() {
    glBindVertexArray(VAO);
    initGLFrame();
    glBindVertexArray(0);
    return true;
}

void mCamera::initGLFrame() {
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)wndWidth / wndHeight, near_plane, far_plane);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4(1.0f);

    this->cam_view_mat = view;
    this->MVP = projection*view*model;
    this->textureID = genTexture();

    float tmp_t = frame_zpos * glm::tan(glm::radians(fov/2));
    float tmp_r = tmp_t * (float)wndWidth / wndHeight;

    static const GLfloat g_vertex_buffer_data[] = {
                         -tmp_r, tmp_t, -frame_zpos,
                         -tmp_r, -tmp_t, -frame_zpos,
                         tmp_r, -tmp_t, -frame_zpos,
                         tmp_r, -tmp_t, -frame_zpos,
                         tmp_r, tmp_t, -frame_zpos,
                         -tmp_r, tmp_t, -frame_zpos};
    static const GLfloat uv_data[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };
 
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}
GLuint mCamera::genTexture() {

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return texture;
}
void mCamera::setTextureData(cv::Mat & frame) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    //cv::flip(frame, frame, -1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wndWidth, wndHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr(0));
    glGenerateMipmap(GL_TEXTURE_2D);
}
void mCamera::drawFrame(cv::Mat &frame) {
    glBindVertexArray(VAO);

    camShader->use();
    camShader->setVal("MVP", MVP);
    setTextureData(frame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    camShader->setVal("myTextureSampler", 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

#include "../../include/mSceneUtils.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <glm/gtx/component_wise.hpp>

#include "../../include/mRotateUtils.h"
#include "../../include/mDebugUtils.h"
#include "../../include/mRenderParameters.h"

static bool is_reset = false;
static int move_dir = 0;
static float move_step = 80.0;

static bool is_surround = false;
static float surround_step = 1.0f;
static bool is_surround_still = false;

static void keybutton_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_R) {
            is_reset = true;
        }
        else if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        else if (key == GLFW_KEY_D) {
            move_dir = 1;
        }
        else if (key == GLFW_KEY_A) {
            move_dir = -1;
        }
        else if (key == GLFW_KEY_W) {
            move_dir = 3;
        }
        else if (key == GLFW_KEY_S) {
            move_dir = -3;
        }
        else if (key == GLFW_KEY_O) {
            is_surround = !is_surround;
        }
        else if (key == GLFW_KEY_SPACE) {
            is_surround_still = !is_surround_still;
        }
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    move_dir = -yoffset * 2;
}


mSceneUtils::mSceneUtils(int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, int pose_type, std::string wnd_title) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;

    // The parameter maybe changed as reality make sure the ground_col and ground_row is even
    this->ground_col = 200;
    this->ground_row = 200;
    this->ground_size = 2000.0f;

    this->do_use_surround = false;
    this->surround_center = glm::vec3(0.f, 0.f, 0.f);

    this->cur_cam_ex_mat = glm::transpose(cam_ex_mat);
    this->cam_ex_mat = this->cur_cam_ex_mat;
    this->cam_ex_mat_inverse = glm::inverse(this->cam_ex_mat);

    this->cam_ex_r_mat = glm::mat4(glm::mat3(this->cam_ex_mat));
    this->cam_ex_t_mat = glm::inverse(this->cam_ex_r_mat) * this->cam_ex_mat;
    this->cam_ex_t_mat[0][1] = 0;this->cam_ex_t_mat[0][2] = 0;this->cam_ex_t_mat[1][0] = 0;this->cam_ex_t_mat[1][2] = 0;this->cam_ex_t_mat[2][0]= 0;this->cam_ex_t_mat[2][1] = 0;

    this->cur_cam_ex_t_mat = this->cam_ex_t_mat;

    this->cam_in_mat = cam_in_mat;
    this->rotate_mat = glm::mat4(1.f);
    this->trans_mat = glm::mat4(1.f);

    this->cam_proj_mat = glm::transpose(glm::mat4({
        2.0*this->cam_in_mat[0][0] / wnd_width, 0, -1 + 2.0*this->cam_in_mat[0][2] / wnd_width, 0.0,
        0, -2.0*this->cam_in_mat[1][1]/wnd_height, 1 - 2.0*this->cam_in_mat[1][2] / wnd_height, 0.0,
        0, 0, 1, -2 * this->cam_in_mat[0][0],
        0, 0, 1, 0}));

    this->gl_session = new mOpenGLUtils(wnd_title, this->wnd_width, this->wnd_height);
    this->gl_session->setCallback(mRawRotate::mouse_button_callback, mRawRotate::mouse_move_callback, keybutton_callback, scroll_callback);
    
    glGenVertexArrays(1, &this->VAO);

    this->scene_shader = new mShader(mPoseShaderFiles[0], mPoseShaderFiles[1]);
    this->depth_shader = new mShader(mDepthShaderFiles[0], mDepthShaderFiles[1], mDepthShaderFiles[2]);

    this->pose_model = new mPoseModel(this->VAO, this->wnd_width, this->wnd_height, this->scene_shader, this->depth_shader, this->cam_in_mat , "/home/kaihang/Projects/visualization/models/", 30 * 2, pose_type);
    // Use the same vao for rendering the shading
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->ground_vbo);
    glGenBuffers(1, &this->ground_cbo);

    std::vector<GLfloat> vertexs_data = this->getGroundVertexs();
    std::vector<GLfloat> color_data = this->getGroundColor();

    this->array_size = vertexs_data.size() / 3;

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexs_data.size() * sizeof(GLfloat), &vertexs_data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    glBufferData(GL_ARRAY_BUFFER, color_data.size() * sizeof(GLfloat), &color_data[0], GL_STATIC_DRAW);

    /************************ Handel shadow displayment ****************************/
    glEnable(GL_DEPTH_TEST);
    glGenFramebuffers(1, &this->shadow_fbo);
    glGenTextures(1, &this->shadow_tbo);

    glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_tbo);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, mShadowWndWidth, mShadowWndHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadow_tbo, 0);
    // Tell OpenGL not to render color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "(mSceneUtils.cpp) Shadow Framebuffer is not complete!" << std::endl;
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*******************************************************************************/

    glBindVertexArray(0);
}

mSceneUtils::~mSceneUtils() {
    this->gl_session->~mOpenGLUtils();
    this->pose_model->~mPoseModel();
    this->scene_shader->~mShader();
    this->depth_shader->~mShader();
}

bool mSceneUtils::windowShouldClose() {
    return this->gl_session->checkShouldClose();
}

void mSceneUtils::setTextureData(cv::Mat frame) {
    glBindTexture(GL_TEXTURE_2D, this->ground_tbo);
    cv::Size frame_size = frame.size();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_size.width, frame_size.height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr(0));
    glGenerateMipmap(GL_TEXTURE_2D);
}

std::vector<GLfloat> mSceneUtils::getGroundColor() {
    std::vector<GLfloat> result_gd(this->ground_col * this->ground_row * 18, 0);
    GLfloat * r_ptr = &result_gd[0];
    float color_table[2][3] = {
        {0.4470588235294118, 0.4745098039215686, 0.611764705882353},
        {0.8784313725490196, 0.9098039215686274, 1}
    };

    int start_index = 0;
    int cur_index;

    for (int i = 0; i < this->ground_row; ++i) {
        start_index = !start_index;
        cur_index = start_index;
        for (int j = 0; j < this->ground_col; ++j) {
            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            cur_index = !cur_index;
        }
    }

    return result_gd;
}


std::vector<GLfloat> mSceneUtils::getGroundVertexs() {
    std::vector<GLfloat> result_gd(this->ground_col*this->ground_row * 18, 0);
    
    int row_from = -1 * this->ground_row / 2;
    int row_end = -row_from;

    int col_from = -1 * this->ground_col / 2;
    int col_end = -col_from;

    GLfloat * r_ptr = &result_gd[0];

    for (int i = row_from; i < row_end; ++i) {
        for (int j = col_from; j < col_end; ++j) {

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;
        }
    }

    return result_gd;
}

GLuint mSceneUtils::genTexture() {
    // Mind: this function is called after bind the vao
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return texture_id;
}

//void mSceneUtils::rotateExMat(glm::mat4 & ex_mat, glm::mat4 rotate_mat) {
    //for (int i = 0; i < 3; ++i) {
        //glm::vec3 c_p(ex_mat[0][i], ex_mat[1][i], ex_mat[2][i]);
        //c_p = glm::normalize(c_p);
        //glm::vec4 c_p_4 = rotate_mat * glm::vec4(c_p, 1.0);

        //ex_mat[0][i] = c_p_4[0];
        //ex_mat[1][i] = c_p_4[1];
        //ex_mat[2][i] = c_p_4[2];
    //}
//}

void mSceneUtils::rotateExMat(glm::mat4 & ex_mat, glm::mat4 rotate_mat) {
    for (int i = 0; i < 3; ++i) {

        glm::vec3 c_p(ex_mat[0][i], ex_mat[1][i], ex_mat[2][i]);
        c_p = glm::normalize(c_p);
        glm::vec4 c_p_4 = rotate_mat * glm::vec4(c_p, 1.0);

        ex_mat[0][i] = c_p_4[0];
        ex_mat[1][i] = c_p_4[1];
        ex_mat[2][i] = c_p_4[2];
    }
}

void mSceneUtils::transExMat(glm::mat4 ex_mat) {
    if (move_dir != 0) {

        glm::vec3 dir_x(ex_mat[0][0], ex_mat[1][0], ex_mat[2][0]);
        glm::vec3 dir_y(ex_mat[0][1], ex_mat[1][1], ex_mat[2][1]);
        glm::vec3 dir_z(-ex_mat[0][2], -ex_mat[1][2], -ex_mat[2][2]);
        
        //glm::vec3 dir_x(1.0, 0.0, 0.0);
        //glm::vec3 dir_y(0.0, 1.0, 0.0);
        //glm::vec3 dir_z(0.0, 0.0, 1.0);

        if (move_dir == 1) {
            this->trans_mat = glm::translate(this->trans_mat, -move_step * dir_x);
        }
        else if (move_dir == -1) {
            this->trans_mat = glm::translate(this->trans_mat, move_step * dir_x);
        }
        else if (move_dir == 2) {
            this->trans_mat = glm::translate(this->trans_mat, -move_step * dir_y);
        }
        else if (move_dir == -2) {
            this->trans_mat = glm::translate(this->trans_mat, move_step * dir_y);
        }
        else if (move_dir == -3) {
            this->trans_mat = glm::translate(this->trans_mat, -move_step * dir_z);
        }
        else if (move_dir == 3) {
            this->trans_mat = glm::translate(this->trans_mat, move_step * dir_z);
        }

        move_dir = 0;
    }
}

/********************* Going to set the rotate around one person ********************/
void mSceneUtils::surroundOnePoint(glm::mat4 & model_mat) {
    //std::cout << this->surround_center[0] << this->surround_center[1] << this->surround_center[2] << std::endl;
    model_mat = glm::translate(glm::mat4(1.f), this->surround_center) * glm::rotate(glm::mat4(1.f), glm::radians(surround_step), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.f), -this->surround_center);
}

void mSceneUtils::setSurround(bool do_surround, glm::vec3 surround_center) {
    this->surround_center = surround_center;
    this->do_use_surround = do_surround;
}

void mSceneUtils::render(std::vector<float> points_3d) {

    /****************************** Temporary surround rotate ********************************/
    //if (!this->do_use_surround) {
    if (!is_surround) {
        // move freely
        glm::mat4 cur_rotate = mRawRotate::getRotateMat(this->wnd_width, this->wnd_width, this->cur_cam_ex_mat);
        this->rotate_mat = cur_rotate * this->rotate_mat;
        // must translate first
        this->transExMat(this->cur_cam_ex_mat);
        this->cur_cam_ex_t_mat = this->trans_mat * this->cam_ex_t_mat;

        // then rotate here I use the identity matrix to set the initial camera pose
        this->cur_cam_ex_mat = this->cam_ex_r_mat;
        //this->cur_cam_ex_mat = glm::mat4(1.f);
        //this->cur_cam_ex_mat[1][1] = -1.0;
        //this->cur_cam_ex_mat[2][2] = -1.0;
        this->rotateExMat(this->cur_cam_ex_mat, this->rotate_mat);
        this->cur_cam_ex_mat = this->cur_cam_ex_mat * this->cur_cam_ex_t_mat;
    }
    else {
        // TODO Here the line below is moved from the above, I don't know what will happen to the surround now.
        this->surround_center = this->cam_ex_mat_inverse * glm::vec4(points_3d[14 * 3], points_3d[14 * 3 + 1], points_3d[14 * 3 + 2], 1.0);
        // rotate surround one point
        if (!is_surround_still) {
            glm::mat4 surround_model_mat;
            this->surroundOnePoint(surround_model_mat);
            this->cur_cam_ex_mat = this->cur_cam_ex_mat * surround_model_mat;
        }
    }
    /******************************** Set the rendering parameter **********************************/
    this->gl_session->drawStart();
    /************************* Render the depthmap *************************/
    glBindVertexArray(this->VAO);

    glViewport(0, 0, mShadowWndWidth, mShadowWndHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    this->depth_shader->use();

    for (int i = 0; i < 6; ++i) {
        this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mShadowTransforms[i]);
    }
    this->depth_shader->setVal("far_plane", mShadowFarPlane);
    this->depth_shader->setVal("lightPos", mLightPos);
    this->depth_shader->setVal("model", glm::mat4(1.f));

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, this->array_size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (points_3d.size() == 3*this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d, this->cam_ex_mat_inverse, this->cur_cam_ex_mat, 1);
    }

    /************************** Render the scene **************************/
    glBindVertexArray(this->VAO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, this->wnd_width, this->wnd_height);

    this->scene_shader->use();
    glm::mat4 view_r_mat = glm::mat4(glm::mat3(this->cur_cam_ex_mat));
    glm::mat4 view_t_mat = glm::inverse(view_r_mat) * this->cur_cam_ex_mat;

    this->scene_shader->setVal("renderType", 1);
    this->scene_shader->setVal("use_shadow", mShadowUseShadow);
    // Set the light parameter
    this->scene_shader->setVal("pointLights[0].position", mLightPos);
    this->scene_shader->setVal("pointLights[0].ambient", mAmbient);
    this->scene_shader->setVal("pointLights[0].diffuse", mDiffuse);
    this->scene_shader->setVal("pointLights[0].specular", mSpecular);
    this->scene_shader->setVal("viewPos", glm::vec3(view_t_mat[3][0], view_t_mat[3][1], view_t_mat[3][2]));
    this->scene_shader->setVal("projection", this->cam_proj_mat);
    this->scene_shader->setVal("view", this->cur_cam_ex_mat);
    this->scene_shader->setVal("model", glm::mat4(1.f));
    this->scene_shader->setVal("normMat", glm::transpose(glm::inverse(glm::mat4(1.f))));

    this->scene_shader->setVal("depth_cube", 1);
    this->scene_shader->setVal("far_plane", mShadowFarPlane);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_fbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, this->array_size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (points_3d.size() == 3*this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d, this->cam_ex_mat_inverse, this->cur_cam_ex_mat, 0, 1);
    }

    this->gl_session->drawEnd();

    if (is_reset) {
        is_reset = false;
        this->pose_model->resetRotate();
        this->rotate_mat = glm::mat4(1.f);
        this->trans_mat = glm::mat4(1.f);
    }
}


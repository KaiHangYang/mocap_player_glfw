#include "../../include/mPoseModel.h"
#include <cstring>
#include "../../include/mDefs.h"
#include "../../include/mRenderParameters.h"

static float skeleton_style[] = {
    0, 1.3, // head
    1, 0.6, // left shoulder
    0, 1.5, // left upper arm
    0, 1.3, // left lower arm
    1, 0.6, // right shoulder
    0, 1.5, // right upper arm
    0, 1.3, // right lower arm
    1, 0.6, // spine
    1, 0.6, // left hip
    0, 2.0, // left ham,
    0, 1.6, // left calf,
    1, 0.6, // right hip
    0, 2.0, // right ham,
    0, 1.6, // right calf,
    0, 1.3, // left feet,
    0, 1.3, // right feet
};

mPoseModel::mPoseModel(GLuint vao, mShader * pose_shader, mShader * depth_shader, glm::mat4 cam_mat, std::string model_path, float target_model_size, bool is_ar, int pose_type) {
    this->pose_shader = pose_shader;
    this->depth_shader = depth_shader;

    this->proj_mat = cam_mat;
    this->is_ar = is_ar;

    if (vao > 0) {
        this->VAO = vao;
    }
    else {
        glGenVertexArrays(1, &this->VAO);
    }

    if (pose_type == 0) {
        // use our pose_type
        this->num_of_joints = MDEFS::num_of_joints;
        this->bone_indices = MDEFS::bone_indices;
    }
    else if (pose_type == 1) {
        // use vnect pose_type
        this->num_of_joints = MDEFS::num_of_joints_vnect;
        this->bone_indices = MDEFS::bone_indices_vnect;
    }
    else {
        std::cout << "(mPoseModel.cpp) pose_type is not valid!" << std::endl;
        exit(-1);
    }

    this->model_size = 30 * 2;
    this->model_scale = target_model_size / this->model_size;
    this->model_size = target_model_size;

    this->mesh_reader = new mMeshReader(this->VAO);
    this->mesh_reader->addMesh(model_path + "sphere-30.ply");
    this->mesh_reader->addMesh(model_path + "cylinder-30.ply");
}

mPoseModel::~mPoseModel() {
    this->mesh_reader->~mMeshReader();
}

void mPoseModel::setRotate(glm::mat4 rotate_mat) {
    this->rotate_mat = rotate_mat;
}

void mPoseModel::resetRotate() {
    this->rotate_mat = glm::mat4(1.f);
}

void mPoseModel::renderPose(std::vector<float> &vertexs) {
    glBindVertexArray(this->VAO);
    unsigned int vertexNum = vertexs.size() / 3;
    unsigned int lineNum = this->bone_indices.size() / 2;
    std::vector<bool> vertexFlags(vertexNum, false);
    mShader * shader = this->pose_shader;

    shader->use();
    // This render type meams to render the ground or the pose.
    shader->setVal("renderType", 0);
    shader->setVal("use_shadow", 0);
    shader->setVal("viewPos", glm::vec3(0.f, 0.f, 0.f));
    shader->setVal("pointLights[0].position", glm::vec3(10.f, 10.f ,10.f));
    shader->setVal("pointLights[0].ambient", mAmbient);
    shader->setVal("pointLights[0].diffuse", mDiffuse);
    shader->setVal("pointLights[0].specular", mSpecular);
    shader->setVal("projection", this->proj_mat);
    shader->setVal("view", glm::mat4(1.f));

    unsigned int * indices_ptr = &this->bone_indices[0];
    glm::mat4 trans;
    glm::mat4 curmodel;

    for (unsigned int i = 0; i < lineNum; ++i) {
        unsigned int line[2] = { *(indices_ptr++), *(indices_ptr++) };
    
        // Draw the points first
        shader->setVal("fragColor", glm::vec3(1.0, 0.2196078431372549, 0.32941176470588235));
        for (unsigned int j = 0; j < 2; ++j) {
            if (!vertexFlags[line[j]]) {
                vertexFlags[line[j]] = true;

                curmodel = glm::scale(glm::mat4(1.f), this->model_scale * glm::vec3(0.95, 0.95, 0.95));
                curmodel = glm::translate(glm::mat4(1.0f), glm::vec3(vertexs[3 * line[j]], vertexs[3 * line[j] + 1], vertexs[3 * line[j] + 2])) * curmodel;

                shader->setVal("model", curmodel);
                shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
                mesh_reader->render(0);
            }
        }
        shader->setVal("fragColor", glm::vec3(0.8705882352941177, 0.788235294117647, 0.8352941176470589));

        float lineCen[3] = {(vertexs[3 * line[0]] + vertexs[3 * line[1]]) / 2.0f, (vertexs[3 * line[0] + 1] + vertexs[3 * line[1] +1]) / 2.0f, (vertexs[3 * line[0] + 2] + vertexs[3 * line[1] + 2]) / 2.0f };
        float length = sqrt(pow((vertexs[3 * line[0]] - vertexs[3 * line[1]]), 2) + pow((vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), 2) + pow((vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2]), 2));

        glm::vec3 vFrom(0, 1, 0);
        glm::vec3 vTo = glm::normalize(glm::vec3((vertexs[3 * line[0]] - vertexs[3 * line[1]]), (vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), (vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2])));

        trans = glm::translate(glm::mat4(1.0), glm::vec3(lineCen[0], lineCen[1], lineCen[2]));
        float angle = (float)glm::acos(glm::dot(vFrom, vTo));

        if (angle <= 0.0000001) {
            curmodel = glm::mat4(1.0f);
        }
        else {
            curmodel = glm::rotate(glm::mat4(1.0), angle, glm::normalize(glm::cross(vFrom, vTo)));
        }
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.7, length/this->model_size, 0.7)) * glm::scale(glm::mat4(1.f), glm::vec3(this->model_scale));

        curmodel = trans * curmodel * scaleMat;
        
        shader->setVal("model", curmodel);
        shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));

        this->mesh_reader->render(1);
    }
}

void mPoseModel::renderPose(std::vector<float> &vertexs, glm::mat4 view_mat, int render_type, int shadow_sampler_id) {
    glBindVertexArray(this->VAO);
    unsigned int vertexNum = vertexs.size() / 3;
    unsigned int lineNum = this->bone_indices.size() / 2;

    std::vector<bool> vertexFlags(vertexNum, false);
    glm::mat4 view_r_mat = glm::mat4(glm::mat3(view_mat));
    glm::mat4 view_t_mat = glm::inverse(view_r_mat) * view_mat;

    mShader * shader;
    if (render_type == 0) {
        shader = this->pose_shader;
    }
    else if (render_type == 1)  {
        shader = this->depth_shader;
    }
    else {
        std::cout << "(mPoseModel.cpp): render_type is not valid!" << std::endl;
        exit(-1);
    }
    shader->use();

    // Some uniform has been set in the scene_utils before this function is called
    shader->setVal("renderType", 0);
    shader->setVal("viewPos", glm::vec3(-view_t_mat[3][0], -view_t_mat[3][1], -view_t_mat[3][2]));
    shader->setVal("fragColor", mBoneColor);
    shader->setVal("projection", this->proj_mat);
    shader->setVal("view", view_mat);

    unsigned int * indices_ptr = &this->bone_indices[0];
    glm::mat4 trans;
    glm::mat4 curmodel;

    for (unsigned int i = 0; i < lineNum; ++i) {
        unsigned int line[2] = { *(indices_ptr++), *(indices_ptr++) };
    
        // Draw the points first
        for (unsigned int j = 0; j < 2; ++j) {
            if (!vertexFlags[line[j]]) {
                vertexFlags[line[j]] = true;

                curmodel = glm::scale(glm::mat4(1.f), this->model_scale * glm::vec3(0.8, 0.8, 0.8));
                curmodel = glm::translate(glm::mat4(1.0f), glm::vec3(vertexs[3 * line[j]], vertexs[3 * line[j] + 1], vertexs[3 * line[j] + 2])) * curmodel;

                shader->setVal("model", curmodel);
                if (render_type == 0) {
                    shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
                }
                mesh_reader->render(0);
            }
        }

        float lineCen[3] = {(vertexs[3 * line[0]] + vertexs[3 * line[1]]) / 2.0f, (vertexs[3 * line[0] + 1] + vertexs[3 * line[1] +1]) / 2.0f, (vertexs[3 * line[0] + 2] + vertexs[3 * line[1] + 2]) / 2.0f };
        float length = sqrt(pow((vertexs[3 * line[0]] - vertexs[3 * line[1]]), 2) + pow((vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), 2) + pow((vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2]), 2));

        glm::vec3 vFrom(0, 1, 0);
        glm::vec3 vTo = glm::normalize(glm::vec3((vertexs[3 * line[0]] - vertexs[3 * line[1]]), (vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), (vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2])));

        trans = glm::translate(glm::mat4(1.0), glm::vec3(lineCen[0], lineCen[1], lineCen[2]));
        float angle = (float)glm::acos(glm::dot(vFrom, vTo));

        if (angle <= 0.0000001) {
            curmodel = glm::mat4(1.0f);
        }
        else {
            curmodel = glm::rotate(glm::mat4(1.0), angle, glm::normalize(glm::cross(vFrom, vTo)));
        }
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(skeleton_style[2*i + 1], length/this->model_size, skeleton_style[2*i + 1])) * glm::scale(glm::mat4(1.0), glm::vec3(this->model_scale));

        curmodel = trans * curmodel * scaleMat;
        
        shader->setVal("model", curmodel);

        if (render_type == 0) {
            shader->setVal("fragColor", mBoneColor);
            shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
        }

        this->mesh_reader->render(skeleton_style[2*i]);
    }
}

void mPoseModel::draw(std::vector<float> points, glm::mat4 raw_cam_ex_mat_inv, glm::mat4 & cam_ex_mat, int render_type, int shadow_sampler_id) {
    int p_num = points.size() / 3;

    float * p_ptr = &points[0];
    glm::vec4 p_cur;

    for (int i = 0; i < p_num; ++i) {
        memcpy(&p_cur[0], p_ptr, sizeof(float) * 3);
        p_cur[3] = 1.0;

        p_cur = raw_cam_ex_mat_inv * p_cur;
        //p_cur = this->rotate_mat * p_cur;
        *(p_ptr++) = p_cur[0];
        *(p_ptr++) = p_cur[1];
        *(p_ptr++) = p_cur[2];
    }
    this->renderPose(points, cam_ex_mat, render_type, shadow_sampler_id);
}

// Draw the points in the camera coords
void mPoseModel::draw(std::vector<float> points, int root_num) {
    int p_num = points.size() / 3;
    float root_pos[3] = {points[3*root_num], points[3*root_num+1], points[3*root_num+2]};
    float * p_ptr = &points[0];
    glm::vec4 p_cur;

    for (int i = 0; i < p_num; ++i) {
        memcpy(&p_cur[0], p_ptr, sizeof(float) * 3);
        p_cur[0] -= root_pos[0];
        p_cur[1] -= root_pos[1];
        p_cur[2] -= root_pos[2];
        p_cur[3] = 1.0;
        p_cur = this->rotate_mat * p_cur; 
        *(p_ptr++) = p_cur[0] + root_pos[0];
        *(p_ptr++) = p_cur[1] + root_pos[1];
        *(p_ptr++) = p_cur[2] + root_pos[2];
    }
    this->renderPose(points);
}

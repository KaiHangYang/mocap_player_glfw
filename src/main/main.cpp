#include "../../include/mSceneUtils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../include/mDebugUtils.h"
#include "../../include/mCalUtils.h"
#include "../../include/bvh-parser/bvh-parser.h"

/**** Just for sleep ****/
#include <thread>
#include <chrono>


std::vector<unsigned int> sfu_valid_joints_arr({14, 13, 
                                                24, 25, 26, 
                                                17, 18, 19, 
                                                6, 7, 8, 
                                                1, 2, 3, 
                                                0});
const int num_of_joints = 15;

int main() {

    int wnd_width = 1280;
    int wnd_height = 960;

    /******************** Just for read frame data from the bvh file **********************/
    bvh::Bvh reader;
    bvh::Bvh_parser parser;
    //parser.parse("/home/kaihang/Desktop/0005_2FeetJump001.bvh", &reader);
    //parser.parse("/home/kaihang/Desktop/0018_DanceTurns001.bvh", &reader);
    //parser.parse("/home/kaihang/Desktop/0012_JumpAndRoll001.bvh", &reader);
    parser.parse("/home/kaihang/Desktop/0017_ParkourRoll001.bvh", &reader);
    
    reader.recalculate_joints_ltm(reader.root_joint());
    /**************************************************************************************/


    int total_frame_num = reader.num_frames();
    std::vector<unsigned int> valid_joints_arr = sfu_valid_joints_arr;
    std::vector<std::shared_ptr<bvh::Joint>> bvh_joints = reader.joints();
    std::vector<std::vector<glm::vec3>> all_joints(num_of_joints);
    std::vector<float> cur_joint(num_of_joints * 3);

    for (int i = 0; i < num_of_joints; ++i) {
        all_joints[i] = bvh_joints[valid_joints_arr[i]]->pos();
    }

    /************************* Show the joints name and size **************************/
    for (int i = 0; i < bvh_joints.size(); ++i) {
        std::cout << i << " " << bvh_joints[i]->name() << std::endl;
    }
    /**********************************************************************************/

    glm::mat4 cam_in_mat = glm::transpose(glm::perspective(glm::radians(45.f), (float)wnd_width / wnd_height, 0.01f, 10000000.f));
    glm::mat4 cam_ex_mat = glm::transpose(glm::lookAt(glm::vec3(0, 10.f, 300.f), glm::vec3(0, 10.f, 0), glm::vec3(0, 1, 0)));
    mSceneUtils scene(wnd_width, wnd_height, cam_in_mat, cam_ex_mat, false);


    //glm::mat4 cam_ex_mat(1.f);
    //MCU::getCamExMat(std::vector<float>({1.412737e+00, 1.196062e+00, -1.091680e+00}), std::vector<float>({751.990, 784.32, 2593.9756}), cam_ex_mat);
    //glm::mat4 cam_in_mat({1110.754, 0, 525.169, 0,
                          //0, 1116.737, 484.766, 0,
                          //0, 0, 1, 0,
                          //0, 0, 0, 1});
    //mSceneUtils scene(wnd_width, wnd_height, cam_in_mat, cam_ex_mat, true);

    int cur_frame_num = 0;
    float cur_scale = 1;

    while (scene.windowShouldClose()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        for (int i = 0; i < num_of_joints; ++i) {
            glm::vec4 g_joint(all_joints[i][cur_frame_num], 1.f);

            g_joint = glm::transpose(cam_ex_mat) * g_joint;
            cur_joint[3*i + 0] = g_joint.x;
            cur_joint[3*i + 1] = g_joint.y;
            cur_joint[3*i + 2] = g_joint.z;
            
            //g_joint = cam_ex_mat * g_joint;
            //cur_joint[3*i + 0] = g_joint.x * 30;
            //cur_joint[3*i + 1] = g_joint.y * 30;
            //cur_joint[3*i + 2] = g_joint.z * 30;
        }

        scene.render(cur_joint);
        cur_frame_num += 1;

        if (cur_frame_num >= total_frame_num) {
            cur_frame_num = 0;
        }
    }
    return 0;
}


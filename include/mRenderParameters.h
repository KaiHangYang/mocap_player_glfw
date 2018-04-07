#ifndef M_RENDER_PARAMETER
#define M_RENDER_PARAMETER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

/************************Shader parameters*************************/
static glm::vec3 mLightPos(0, 3000, 3000);
static glm::vec3 mAmbient(0.3f, 0.3f, 0.3f);
static glm::vec3 mDiffuse(0.5f, 0.5f, 0.5f);
static glm::vec3 mSpecular(0.4f, 0.4f, 0.4f);

static glm::vec3 mBoneColor(0.9882352941176471, 0.788235294117647, 0.5294117647058824);

/******************** Light Cube parameter ************************/
static int mShadowUseShadow = 1;
static int mShadowWndWidth = 4096;
static int mShadowWndHeight = 4096;

static float mShadowNearPlane = 1.0f;
static float mShadowFarPlane  = 100000.0f; // TODO This plane must cover all the models you will render
static glm::mat4 mShadowProj = glm::perspective(glm::radians(90.0f), (float)mShadowWndWidth / (float)mShadowWndHeight, mShadowNearPlane, mShadowFarPlane);

static std::vector<glm::mat4> mShadowTransforms({
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
mShadowProj * glm::lookAt(mLightPos, mLightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))});


/******************************************************************/

/************************Shader file path**************************/
static std::vector<std::string> mDepthShaderFiles({"/home/kaihang/Projects/visualization/shaders/depth.vert",
                                              "/home/kaihang/Projects/visualization/shaders/depth.frag",
                                              "/home/kaihang/Projects/visualization/shaders/depth.geo"});

static std::vector<std::string> mPoseShaderFiles({"/home/kaihang/Projects/visualization/shaders/multilight.vert",
                                                 "/home/kaihang/Projects/visualization/shaders/multilight.frag"});

static std::vector<std::string> mCamShaderFiles({"/home/kaihang/Projects/visualization/shaders/cam_v.shader",
                                                 "/home/kaihang/Projects/visualization/shaders/cam_f.shader"});
/******************************************************************/

#endif
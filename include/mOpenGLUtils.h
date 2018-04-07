#ifndef M_OPENGL_UTILS
#define M_OPENGL_UTILS
#include <vector>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Used for capture frame
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

class mOpenGLUtils {
public:
    mOpenGLUtils(std::string win_title, int wnd_width, int wnd_height);
    ~mOpenGLUtils();
    GLFWwindow * window;

    void makeCurCtx();
    void drawStart();
    void drawEnd();
    bool checkShouldClose();
    bool checkInited();
    void terminate();
    void setCallback(void (mouse_button_callback)(GLFWwindow *, int, int, int), void (mouse_move_callback)(GLFWwindow *, double, double), void(keybutton_callback) (GLFWwindow *, int, int, int, int), void(scroll_callback)(GLFWwindow*,double,double) = nullptr);

    void captureCurFrame(cv::Mat & cur_frame);

private:
    GLFWwindow * initWindow();
    
    static void error_callback(int error, const char *description) {
        printf("glfwERROR: code %d, desc:%s\n", error,  description);
    }

    //static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
    //static void mouse_move_callback(GLFWwindow * window, double x, double y);

    std::string win_title;
    int wnd_width;
    int wnd_height;
};

#endif

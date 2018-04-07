#ifndef M_DEBUG_UTILS
#define M_DEBUG_UTILS
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <fstream>


static void printMat3(const glm::mat3 & mat) {
    for (int i = 0; i < 3; ++i) {
        std::cout << mat[i][0] << "\t" << mat[i][1] << "\t" << mat[i][2] << std::endl;
    }
}
static void printMat4(const glm::mat4 & mat) {
    for (int i = 0; i < 4; ++i) {
        std::cout << mat[i][0] << "\t" << mat[i][1] << "\t" << mat[i][2] << "\t" << mat[i][3] << std::endl;
    }
}

static void printVec3(const glm::vec3 & vec) {
    std::cout << vec[0] << "\t" << vec[1] << "\t" << vec[2] << std::endl;
}

static void printVec4(const glm::vec4 & vec) {
    std::cout << vec[0] << "\t" << vec[1] << "\t" << vec[2] << "\t" << vec[3] << std::endl;
}

template<typename T>
static void printPoints(const std::vector<T> points, int step) {
    for (int i = 0; i < points.size() / step; ++i) {
        for (int j = 0; j < step; ++j) {
            std::cout << points[step * i + j] << " ";
        }

        std::cout << std::endl;
    }
}

template<typename T>
static void saveArray(std::string save_path, std::vector<T> arr) {
    std::ofstream save_file(save_path);

    for (int i = 0; i < arr.size(); ++i) {
        save_file << arr[i];

        if (i != arr.size() - 1) {
            save_file << " ";
        }
    }
    save_file.close();
};


#endif

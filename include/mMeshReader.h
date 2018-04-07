#ifndef M_MESH_READER
#define M_MESH_READER

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class mMeshEntry {
public:
    enum BUFFERS {
        VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER,INDEX_BUFFER
    };
    GLuint VAO;
    GLuint VBO[4];
    unsigned int elm_num;

    mMeshEntry(aiMesh * mesh, GLuint vao);
    ~mMeshEntry();

    void render();
};

class mMeshReader {
public:
    int mesh_size;
    std::vector<std::vector<mMeshEntry *>> mesh_entries;
    GLuint VAO;

    mMeshReader(GLuint vao);
    ~mMeshReader();

    void addMesh(std::string mesh_file);

    void render(int mesh_num = 0);
};




#endif

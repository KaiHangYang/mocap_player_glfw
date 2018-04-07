#include "../../include/mMeshReader.h"

mMeshEntry::~mMeshEntry() {
    glBindVertexArray(VAO);
    if (VBO[VERTEX_BUFFER]) {
        glDeleteBuffers(1, &VBO[VERTEX_BUFFER]);
    }

    if (VBO[TEXCOORD_BUFFER]) {
        glDeleteBuffers(1, &VBO[TEXCOORD_BUFFER]);
    }

    if (VBO[NORMAL_BUFFER]) {
        glDeleteBuffers(1, &VBO[NORMAL_BUFFER]);
    }

    if (VBO[INDEX_BUFFER]) {
        glDeleteBuffers(1, &VBO[INDEX_BUFFER]);
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

}

mMeshEntry::mMeshEntry(aiMesh * mesh, GLuint vao) {
    this->VAO = vao;
    this->VBO[0] = 0;
    this->VBO[0] = 0;
    this->VBO[0] = 0;
    this->VBO[0] = 0;

    glBindVertexArray(VAO);

    if (mesh->HasPositions()) {
        std::vector<GLfloat> vertexs;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            vertexs.push_back(mesh->mVertices[i].x);
            vertexs.push_back(mesh->mVertices[i].y);
            vertexs.push_back(mesh->mVertices[i].z);
        }
        
        glGenBuffers(1, &VBO[VERTEX_BUFFER]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_BUFFER]);
        glBufferData(GL_ARRAY_BUFFER, vertexs.size()*sizeof(GLfloat), &vertexs[0], GL_STATIC_DRAW);
    }

    if (mesh->HasFaces()) {
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
        glGenBuffers(1, &VBO[INDEX_BUFFER]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_BUFFER]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        this->elm_num = indices.size();
    }

    if (mesh->HasNormals()) {
        std::vector<GLfloat> normals;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            normals.push_back(mesh->mNormals[i].x);
            normals.push_back(mesh->mNormals[i].y);
            normals.push_back(mesh->mNormals[i].z);
        }
        glGenBuffers(1, &VBO[NORMAL_BUFFER]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_BUFFER]);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void mMeshEntry::render() {
    glBindVertexArray(this->VAO);

    if (0 != this->VBO[VERTEX_BUFFER]) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_BUFFER]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
        glEnableVertexAttribArray(0);
    }
    if (0 != this->VBO[NORMAL_BUFFER]) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_BUFFER]);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(2);
        
    }
    if (0 != this->VBO[INDEX_BUFFER]) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_BUFFER]);
        glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(1);
    }
    glDrawElements(GL_TRIANGLES, this->elm_num, GL_UNSIGNED_INT, NULL); 

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

mMeshReader::mMeshReader(GLuint vao) {
    this->VAO = vao;
    this->mesh_size = 0;
}
void mMeshReader::addMesh(std::string mesh_file) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(mesh_file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "(mMeshReader.cpp): " << importer.GetErrorString() << std::endl;
    }

    this->mesh_entries.push_back(std::vector<mMeshEntry *>());
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        this->mesh_entries[this->mesh_size].push_back(new mMeshEntry(scene->mMeshes[i], this->VAO));
    }

    ++this->mesh_size;
}
mMeshReader::~mMeshReader() {
    for (unsigned int i = 0; i < this->mesh_entries.size(); ++i) {
        for (unsigned int j = 0; j < this->mesh_entries[i].size(); ++j) {
            this->mesh_entries[i][j]->~mMeshEntry();
        }
        this->mesh_entries[i].clear();
    }
    this->mesh_entries.clear();
    glBindVertexArray(0);
}
void mMeshReader::render(int mesh_num) {
    if (mesh_num >= this->mesh_size) {
        std::cout << "(mMeshReader.cpp): mesh number is exceed the bound.(cur:"+std::to_string(mesh_num)+", max:"+std::to_string(this->mesh_size-1)+")" << std::endl;
        exit(-1);
    }
    for (unsigned int i = 0; i < this->mesh_entries[mesh_num].size(); ++i) {
        this->mesh_entries[mesh_num][i]->render();
    }
}

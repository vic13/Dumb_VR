#pragma once

#include "Chunk.hpp"
#include "Constants.hpp"
#include "Shader.hpp"

class World {
private:
    int defaultSize;
    GLuint texture;
    Shader* chunkShader;
    Chunk* worldChunks[SCX][SCY][SCZ];
    Chunk* renderChunks[30];
    float chunkScale = 1.0f;


    void initWorld() {
        for (int i = 0; i < defaultSize; i++) {
            for (int j = 0; j < defaultSize; j++) {
                worldChunks[i][0][j] = new Chunk(i * CX, 2, j * CZ, chunkScale);
            }
        }
        setChunkNeighbours();
    }

    void setChunkNeighbours() {
        for (int x = 0; x < defaultSize; x++) {
            for (int z = 0; z < defaultSize; z++) {
                if (0 <= x + 1  && x + 1 < defaultSize) {
                    this->worldChunks[x][0][z]->setPosXNeighbour(worldChunks[x + 1][0][z]);
                }

                if (0 <= x - 1  && x - 1 < defaultSize) {
                    this->worldChunks[x][0][z]->setNegXNeighbour(worldChunks[x - 1][0][z]);
                }

                if (0 <= z + 1 && z + 1 < defaultSize) {
                    this->worldChunks[x][0][z]->setPosZNeighbour(worldChunks[x][0][z + 1]);
                }

                if (0 <= z - 1 && z - 1 < defaultSize) {
                    this->worldChunks[x][0][z]->setNegZNeighbour(worldChunks[x][0][z - 1]);
                }

            }
        }
    }

public:
    World(int defaultSize, Shader chunkShader) {
        this->defaultSize = defaultSize;
        this->texture = createTexture("chunk.png", true);
        this->chunkShader = &chunkShader;
        initWorld();
    }

    void render(glm::mat4 &pv, Material chunkMaterial) {
        glEnable(GL_CULL_FACE);  // Order of the vertices matters because of this. Vertices need to be placed in a clock-wise manner otherwise they won't be displayed. Read more at https://en.wikipedia.org/wiki/Back-face_culling
        glEnable(GL_DEPTH_TEST);

        std::string name = "texture_diffuse";
        std::string number = "1";
        const GLchar* uniformName = (name + number).c_str();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(chunkShader->ID, uniformName), 0);
        glBindTexture(GL_TEXTURE_2D, this->texture);

        setModelUniforms(*chunkShader, chunkMaterial);
        for (int i = 0; i < defaultSize; i++) {
            for (int j = 0; j < defaultSize; j++) {
                    this->chunkShader->setMatrix4("m", worldChunks[i][0][j]->getChunkModel());
                    this->chunkShader->setMatrix4("mvp", pv * worldChunks[i][0][j]->getChunkModel());
                    worldChunks[i][0][j]->render();
            }
        }
    }

    void setModelUniforms(Shader shader, Material material) {
        shader.setFloat("material.ns", material.ns);
        shader.setFloat("material.ka", material.ka);
        shader.setFloat("material.kd", material.kd);
        shader.setFloat("material.ks", material.ks);
    }
    



};
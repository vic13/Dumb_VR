#pragma once

#include "Chunk.hpp"
#include "Constants.hpp"
#include "Shader.hpp"

static const glm::vec4 chunkMiddle = glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
static const int drawingDistance = 2;
static const int yWorldTranslation;


class World {
private:
    int defaultSize;
    GLuint texture;
    Shader* chunkShader;
    Chunk* worldChunks[SCX][SCY][SCZ];
    float chunkScale = 1.0f;
    glm::vec4 steveLastPos = glm::vec4(stevePos, 1); 
    glm::vec4 steveLastDir = glm::vec4(0.0);


    void initWorld() {
        for (int i = 0; i < defaultSize; i++) {
            for (int j = 0; j < defaultSize; j++) {
                worldChunks[i][0][j] = new Chunk(i * CX, yWorldTranslation, j * CZ, chunkScale);
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

    int static getChunkAxisOriginWorldCoordinates(int axisCoordinate, int axisChunkSize) {
        int res = std::floor(axisCoordinate) - fmod(axisCoordinate,  axisChunkSize);
        return res;
    }

    glm::ivec3 objectChunkCoordinates(glm::vec4 objCoordinates) {
        int x = getChunkAxisOriginWorldCoordinates(objCoordinates.x, CX);
        int z = getChunkAxisOriginWorldCoordinates(objCoordinates.z, CX);
        return glm::ivec3(x, objCoordinates.y, z);
    }

    glm::ivec4 getSteveChunkPosition(glm::vec3 chunkOrigin, glm::vec4 steveWorldCoordinates) {
        glm::vec4 steveDirection = steveWorldCoordinates - steveLastPos;
        steveLastDir.y = steveDirection.y;

        int x;
        int z;
        // Add test to make sure position only changes to a new block that is in the same direction as the movement direction  
        if (steveDirection.x) {
            steveLastDir.x = steveDirection.x;
        }

        if (steveDirection.z) {
            steveLastDir.z = steveDirection.z;
        }

        if (steveLastDir.x >= 0) {
            x = round(steveWorldCoordinates.x - chunkOrigin.x + 0.15) - 1;
        }
        else {
            x = round(steveWorldCoordinates.x - chunkOrigin.x - 0.15);
        }


        if (steveLastDir.z >= 0) {
            z = round(steveWorldCoordinates.z - chunkOrigin.z + 0.15) - 1;
        }
        else {
            z = round(steveWorldCoordinates.z - chunkOrigin.z - 0.15);
        }

        int currentBlockX = 0;
        int currentBlockZ = 0;
        if (x < 0) {
            if (!chunkOrigin.x) {
                x = 0;
            }
            else {
                x = 15;
                currentBlockX = 1;
            }
        }

        else if (x >= 16) {
            if (chunkOrigin.x == defaultSize) {
                x = 15;
            }
            else {
                x = 0;
                currentBlockX = - 1;
            }
        }



        if (z < 0) {
            if (!chunkOrigin.z) {
                z = 0;
            }
            else {
                z = 15;
                currentBlockZ = 1;
            }
        }

        else if (z >= 16) {
            if (chunkOrigin.z == defaultSize) {
                z = 15;
            }
            else {
                z = 0;
                currentBlockZ = -1;
            }
        }

        return glm::ivec4(x, currentBlockX, z, currentBlockZ);
    }


public:
    World(int defaultSize, Shader& chunkShader) {
        this->defaultSize = defaultSize;
        this->texture = createTexture("chunk.png", true);
        this->chunkShader = &chunkShader;
        initWorld();
    }

    void render(glm::mat4 &pv, Material chunkMaterial, glm::vec4 steveWorldCoordinates, glm::vec3 steveHeight) {
        glEnable(GL_CULL_FACE);  // Order of the vertices matters because of this. Vertices need to be placed in a clock-wise manner otherwise they won't be displayed. Read more at https://en.wikipedia.org/wiki/Back-face_culling
        glEnable(GL_DEPTH_TEST);

        std::string name = "texture_diffuse";
        std::string number = "1";
        const GLchar* uniformName = (name + number).c_str();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(chunkShader->ID, uniformName), 0);
        glBindTexture(GL_TEXTURE_2D, this->texture);

        glm::vec4 coords;
        glm::mat4 mvp;
        setModelUniforms(*chunkShader, chunkMaterial);

        
        int totalChunksCounter = 0;
        int drawnChunksCounter = 0;

        glm::ivec3 steveChunkCoordinates = objectChunkCoordinates(steveWorldCoordinates);
        
        int startChunkX = std::max(0,  steveChunkCoordinates.x / CX - drawingDistance);
        int endChunkX = std::min(defaultSize, steveChunkCoordinates.x / CX + drawingDistance);
        
        int startChunkZ = std::max(0, steveChunkCoordinates.z / CZ - drawingDistance);
        int endChunkZ = std::min(defaultSize, steveChunkCoordinates.z / CZ + drawingDistance);


        for (int i = startChunkX; i < endChunkX; i++) {
            for (int j = startChunkZ; j < endChunkZ; j++) {
                mvp = pv * worldChunks[i][0][j]->getChunkModel();
                coords = mvp * chunkMiddle;

                //glm::vec4 testDebug = worldChunks[i][0][j]->getChunkModel() * chunkMiddle;
                
                totalChunksCounter++;

                // Compute NDC coordinates for x and y
                coords.x /= coords.w;  
                coords.y /= coords.w;

                

                if (coords.z < -16 && !(steveLastDir.y < 0)) {  // When steve is falling the z value may become very weird
                    continue;
                }

                else if ((coords.x < -1.0 - (28 / fabsf(coords.w)) || coords.x > 1.0  + (28 / fabsf(coords.w)) || coords.y < -1.0 - (28 / fabsf(coords.w)) || coords.y > 1.0 + (28 / fabsf(coords.w)))) {  //Why 28 works??
                    continue;
                }

                else {
 
                    drawnChunksCounter++;
                    this->chunkShader->setMatrix4("m", worldChunks[i][0][j]->getChunkModel());
                    this->chunkShader->setMatrix4("mvp", mvp);
                    worldChunks[i][0][j]->render();

                    if (i * CX <= steveChunkCoordinates.x && (i + 1) * CX > steveChunkCoordinates.x && j * CZ <= steveChunkCoordinates.z && (j+1) * CZ > steveChunkCoordinates.z) {
                        glm::ivec4 steveRelCoords = getSteveChunkPosition(steveChunkCoordinates, steveWorldCoordinates);
                        std::cout << "Chunk" << i << ", " << j << std::endl;
                        std::cout << steveRelCoords.x << ", " << steveRelCoords.z << std::endl;
                        int blockHeight = worldChunks[i - steveRelCoords.y][0][j - steveRelCoords.w]->getHeight(steveRelCoords.x, steveRelCoords.z) + yWorldTranslation;

                        if (steveWorldCoordinates.y - 1 < blockHeight && steveMoveDirections[3]) {
                            stevePos.y = blockHeight + 1;
                            steveMoveDirections[3] = 0;
                        }
                        else if (steveWorldCoordinates.y - 1 > blockHeight && !steveMoveDirections[3]){
                            steveMoveDirections[3] = 1;
                        }
                        else if (steveWorldCoordinates.y - 1 < blockHeight) {
                            stevePos.y = blockHeight + 1;
                            steveMoveDirections[3] = 0;
                            std::cout << "DEBUG" << std::endl;

                        }


                        //std::cout << "My chunk" << i*16 << ", " << j*16 << std::endl;
                    }
                }
            }
        }

        steveLastPos = steveWorldCoordinates;
        //std::cout << "Total Chunks: " << totalChunksCounter << ", Drawn Chunks: " << drawnChunksCounter << std::endl;
    }

    void setModelUniforms(Shader shader, Material material) {
        shader.setFloat("material.ns", material.ns);
        shader.setFloat("material.ka", material.ka);
        shader.setFloat("material.kd", material.kd);
        shader.setFloat("material.ks", material.ks);
    }
    



};

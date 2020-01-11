#pragma once

#include "Chunk.hpp"
#include "Constants.hpp"
#include "Shader.hpp"

static const glm::vec4 chunkMiddle = glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
static const int drawingDistance = 2;
static const int yWorldTranslation = 2;


class World {
private:
    int defaultSize;
    GLuint texture;
    Shader* chunkShader;
    Chunk* worldChunks[SCX][SCY][SCZ];
    float chunkScale = 1.0f;
    glm::vec4 steveLastPos = glm::vec4(stevePos, 1); 
    glm::vec4 steveLastDir = glm::vec4(0.0);
    glm::ivec3 steveLastChunk = glm::ivec3(0, 0, 0);


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

    bool static testCollisionBetweenChunkSteve(float* blockCoords, float* steveCoords, float steveAngle) {
        float projectedSteveCoords[12];
        float projectedBlockCoords[12];
        for (int i = 0; i <= 9; i += 3) {
            projectedSteveCoords[i] = steveCoords[i] / fabs(glm::cos(steveAngle));
            projectedSteveCoords[i + 1] = steveCoords[i + 1];
            projectedSteveCoords[i + 2] = steveCoords[i + 2] * fabs(glm::cos(steveAngle));

            projectedBlockCoords[i] = blockCoords[i] / fabs(glm::cos(steveAngle));
            projectedBlockCoords[i + 1] = blockCoords[i + 1];
            projectedBlockCoords[i + 2] = blockCoords[i + 2] * fabs(glm::cos(steveAngle));
        }

        float steveXMin = std::min(projectedSteveCoords[0], std::min(projectedSteveCoords[3], std::min(projectedSteveCoords[6], projectedSteveCoords[9])));
        float steveXMax = std::max(projectedSteveCoords[0], std::max(projectedSteveCoords[3], std::max(projectedSteveCoords[6], projectedSteveCoords[9])));
        float steveZMin = std::min(projectedSteveCoords[2], std::min(projectedSteveCoords[5], std::min(projectedSteveCoords[8], projectedSteveCoords[11])));
        float steveZMax = std::max(projectedSteveCoords[2], std::max(projectedSteveCoords[5], std::max(projectedSteveCoords[8], projectedSteveCoords[11])));

        float blockXMin = std::min(projectedBlockCoords[0], std::min(projectedBlockCoords[3], std::min(projectedBlockCoords[6], projectedBlockCoords[9])));
        float blockXMax = std::max(projectedBlockCoords[0], std::max(projectedBlockCoords[3], std::max(projectedBlockCoords[6], projectedBlockCoords[9])));
        float blockZMin = std::min(projectedBlockCoords[2], std::min(projectedBlockCoords[5], std::min(projectedBlockCoords[8], projectedBlockCoords[11])));
        float blockZMax = std::max(projectedBlockCoords[2], std::max(projectedBlockCoords[5], std::max(projectedBlockCoords[8], projectedBlockCoords[11])));


        if ((blockXMin < steveXMin && blockXMax > steveXMin) || (blockXMin < steveXMax && blockXMax > steveXMax)) {  // compare height
            if ((blockZMin < steveZMin && blockZMax > steveZMin) || (blockZMin < steveZMax && blockZMax > steveZMax)) {
                return true;
            }
        }

        return false;
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

    void computeBlockCorners(float* blockWorldPoints, glm::vec3 blockWorldOrigin) {
        blockWorldPoints[0] = blockWorldOrigin.x;
        blockWorldPoints[1] = blockWorldPoints[4] = blockWorldPoints[7] = blockWorldPoints[10] = blockWorldOrigin.y;
        blockWorldPoints[2] = blockWorldOrigin.z + 1;

        blockWorldPoints[3] = blockWorldOrigin.x;
        blockWorldPoints[5] = blockWorldOrigin.z;

        blockWorldPoints[6] = blockWorldOrigin.x + 1;
        blockWorldPoints[8] = blockWorldOrigin.z + 1;

        blockWorldPoints[9] = blockWorldOrigin.x + 1;
        blockWorldPoints[11] = blockWorldOrigin.z;
    }


    glm::ivec4 getNextChunkCoordinates(glm::ivec3 chunkRelCoords, glm::ivec3 chunkOrigin, int dirX, int dirZ) {
        int x = chunkRelCoords.x + dirX;
        int z = chunkRelCoords.z + dirZ;

        int chunkOriginX = chunkOrigin.x;
        int chunkOriginZ = chunkOrigin.z;

        if (x < 0) {
            x += 16;
            chunkOriginX--;
        }

        else if (x > 15) {
            x -= 16;
            chunkOriginX++;
        }

        if (z < 0) {
            z += 16;
            chunkOriginZ--;
        }

        else if (z > 15) {
            z -= 16;
            chunkOriginZ++;
        }

        return glm::ivec4(x, chunkOriginX, z, chunkOriginZ);
    }

    bool computePointBlockCollision(glm::vec3 pointWorldCoordinates, glm::ivec4 block) {
        int blockWorldOriginX = block.x + block.y;
        int blockWorldOriginZ = block.z + block.w;

        if (pointWorldCoordinates.x >= blockWorldOriginX && pointWorldCoordinates.x < blockWorldOriginX + 1 && pointWorldCoordinates.z >= blockWorldOriginZ && pointWorldCoordinates.z < blockWorldOriginZ + 1) {
            return true;
        }

        return false;
    }

    glm::ivec4 computeSteveLegCollisionPoints(float* steveWorldLegPoints, glm::ivec4 nextBlock) {
        glm::ivec4 collisionPoints;
        for (int i = 0; i <= 9; i += 3) {
            bool pointCollision = computePointBlockCollision(glm::vec3(steveWorldLegPoints[i], steveWorldLegPoints[i + 1], steveWorldLegPoints[i + 2]), nextBlock);
            if (pointCollision) {
                collisionPoints[i/3] = 1;
            }
            else {
                collisionPoints[i/3] = 0;
            }
        }

        return collisionPoints;
    }

    void computeSideCollision(glm::ivec3 steveRelCoords, glm::ivec3 steveChunkOrigin, glm::vec4 steveWorldCoordinates, glm::mat4 steveM) {
        glm::vec4 steveDirection = steveWorldCoordinates - steveLastPos;
        steveLastDir.y = steveDirection.y;
        float steveWorldLegPoints[12];
        glm::vec4 pointsContainer;
        glm::vec4 worldPointsContainer;
        float blockWorldPoints[12];

        for (int i = 0; i <= 9; i += 3) {
            pointsContainer = glm::vec4(steveBodyPoints[i], steveBodyPoints[i + 1], steveBodyPoints[i + 2], 1);
            worldPointsContainer = steveM * pointsContainer;
            steveWorldLegPoints[i] = worldPointsContainer.x;
            steveWorldLegPoints[i + 1] = worldPointsContainer.y;
            steveWorldLegPoints[i + 2] = worldPointsContainer.z;
        }
        
        bool collision;
        glm::ivec4 nextBlock;
        glm::ivec4 collisionDirection;
        int newSteveDirections[6] = { 1, 1, 1, 1, 1, 1 };
        int newSteveRotations[2] = { 1, 1 };

        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i !=0 || j != 0) {
                    nextBlock = getNextChunkCoordinates(steveRelCoords, steveChunkOrigin, i, j);
                    if (worldChunks[nextBlock.y / 16][0][nextBlock.w / 16]->getBlock(nextBlock.x, steveRelCoords.y + 1, nextBlock.z)) { // the + 1 corresponds to the heigh I need to test 
                        computeBlockCorners(blockWorldPoints, glm::vec3(nextBlock.x + nextBlock.y, 0, nextBlock.z + nextBlock.w));
                        collision = testCollisionBetweenChunkSteve(blockWorldPoints, steveWorldLegPoints, steveRotation);
                        
                        if (collision) {
                            glm::ivec4 collisionPoints = computeSteveLegCollisionPoints(steveWorldLegPoints, nextBlock);
                            
                            if (i > 0) {  // Collision with front
                                newSteveDirections[0] = 0;
                                if (collisionPoints.z) {
                                    newSteveRotations[1] = 0;
                                }

                                if (collisionPoints.w) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.x) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.y) {
                                    newSteveRotations[1] = 0;
                                }
                            }
                            else if (i < 0) {
                                newSteveDirections[1] = 0;
                                if (collisionPoints.z) {
                                    newSteveRotations[1] = 0;
                                }

                                if (collisionPoints.w) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.x) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.y) {
                                    newSteveRotations[1] = 0;
                                }
                            }

                            if (j > 0) {
                                newSteveDirections[4] = 0;
                                if (collisionPoints.z) {
                                    newSteveRotations[1] = 0;
                                }

                                if (collisionPoints.w) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.x) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.y) {
                                    newSteveRotations[1] = 0;
                                }
                            }

                            else if (j < 0) {
                                newSteveDirections[5] = 0;
                                if (collisionPoints.z) {
                                    newSteveRotations[1] = 0;
                                }

                                if (collisionPoints.w) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.x) {
                                    newSteveRotations[0] = 0;
                                }

                                if (collisionPoints.y) {
                                    newSteveRotations[1] = 0;
                                }
                            }
                        }
                    }                    
                }
            }
        }


        //FIX THIS LATER
        steveMoveDirections[0] = newSteveDirections[0];
        steveMoveDirections[1] = newSteveDirections[1];
        //steveMoveDirections[2] = newSteveDirections[2];
        //steveMoveDirections[3] = newSteveDirections[3];
        steveMoveDirections[4] = newSteveDirections[4];
        steveMoveDirections[5] = newSteveDirections[5];

        steveRotationDirection[0] = newSteveRotations[0];
        steveRotationDirection[1] = newSteveRotations[1];

    }


    glm::ivec3 getSteveChunkPosition(glm::ivec3 chunkOrigin, glm::vec4 steveWorldCoordinates, glm::mat4 steveM) {
        glm::vec4 steveDirection = steveWorldCoordinates - steveLastPos;
        steveLastDir.y = steveDirection.y;
        float steveWorldLegPoints[12];


        glm::vec4 pointsContainer;
        glm::vec4 worldPointsContainer;

        for (int i = 0; i <= 9; i += 3) {
            pointsContainer = glm::vec4(steveLegPoints[i], steveLegPoints[i + 1], steveLegPoints[i + 2], 1);
            worldPointsContainer = steveM * pointsContainer;
            steveWorldLegPoints[i] = worldPointsContainer.x;
            steveWorldLegPoints[i + 1] = worldPointsContainer.y;
            steveWorldLegPoints[i + 2] = worldPointsContainer.z;
        }

        float blockWorldPoints[12];
        computeBlockCorners(blockWorldPoints, chunkOrigin + steveLastChunk);
        bool collision = testCollisionBetweenChunkSteve(blockWorldPoints, steveWorldLegPoints, steveRotation);

        if (collision) {
            steveLastChunk.y = steveWorldCoordinates.y - yWorldTranslation - 1; // Steve may have jumped
            return steveLastChunk;
        }
        else {
            int x = floor(steveWorldCoordinates.x - chunkOrigin.x);
            int z = floor(steveWorldCoordinates.z - chunkOrigin.z);
            return glm::ivec3(x, steveWorldCoordinates.y - yWorldTranslation - 1, z);
        }
        // Add test to make sure position only changes to a new block that is in the same direction as the movement direction  
        //if (steveDirection.x) {
        //    steveLastDir.x = steveDirection.x;
        //}

        //if (steveDirection.z) {
        //    steveLastDir.z = steveDirection.z;
        //}

    }


public:
    World(int defaultSize, Shader& chunkShader) {
        this->defaultSize = defaultSize;
        this->texture = createTexture("chunk.png", true);
        this->chunkShader = &chunkShader;
        initWorld();
    }

    void render(glm::mat4 &pv, Material chunkMaterial, glm::mat4 steveM, glm::vec3 steveHeight) {
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

        glm::vec4 steveWorldCoordinates = steveM * glm::vec4(0, 0, 0, 1);
        glm::ivec3 steveChunkCoordinates = objectChunkCoordinates(steveWorldCoordinates);  // Coordinates of the Chunk origin

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

                

                if (coords.z < -16) {  // When steve is falling the z value may become very weird
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
                        glm::ivec3 steveRelCoords = getSteveChunkPosition(steveChunkCoordinates, steveWorldCoordinates, steveM);  // Only x and Z and relative to the current chunk
                        computeSideCollision(steveRelCoords, steveChunkCoordinates, steveWorldCoordinates, steveM);

                        //std::cout << "Chunk" << i << ", " << j << std::endl;
                        //std::cout << steveRelCoords.x << ", " << steveRelCoords.z << std::endl;

                        int blockHeight = worldChunks[i][0][j]->getHeight(steveRelCoords.x, steveRelCoords.z) + yWorldTranslation;
                        

                        if (steveWorldCoordinates.y - 1 < blockHeight && steveMoveDirections[3]) {
                            stevePos.y = blockHeight + 1;
                            steveMoveDirections[3] = 0;
                        }
                        else if (steveWorldCoordinates.y - 1 > blockHeight && !steveMoveDirections[3]){
                            fallingT = glfwGetTime();
                            steveMoveDirections[3] = 1;
                        }
                        //else if (steveWorldCoordinates.y - 1 < blockHeight) {
                        //    stevePos.y = blockHeight + 1;
                        //    steveMoveDirections[3] = 0;
                        //    std::cout << "DEBUG" << std::endl;
                        //}

                        steveLastChunk = steveRelCoords;


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

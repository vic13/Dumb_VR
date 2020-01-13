#pragma once

#include "Chunk.hpp"
#include "Constants.hpp"
#include "Shader.hpp"

static const glm::vec4 chunkMiddle = glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
static const int drawingDistance = 20;
static const int yWorldTranslation = 2;


class World {
private:
    int defaultSize;
    GLuint texture;
    Shader* chunkShader;
    Chunk* worldChunks[SCX][SCY][SCZ];
    float chunkScale = 1.0f;

    glm::vec4 steveLastPos = glm::vec4(stevePos, 1); 
    glm::ivec3 steveLastRel = glm::ivec3(0, 0, 0);
    glm::ivec3 steveLastChunk = glm::ivec3(0, 0, 0);
    int currentEmptySelectedBlock[6] = { -1, -1, -1, -1, -1, -1 };
    int currentNonEmptySelectedBlock[6] = { -1, -1, -1, -1, -1, -1};
    int debug;


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


    bool static testCollisionBetweenChunkSteve(float* blockCoords, float* steveCoords, float steveAngle, int dirX, int dirZ) {
        float projectedSteveCoords[12];
        float projectedBlockCoords[12];
        bool firstTest = false;

        float steveXMin = std::min(steveCoords[0], std::min(steveCoords[3], std::min(steveCoords[6], steveCoords[9])));
        float steveXMax = std::max(steveCoords[0], std::max(steveCoords[3], std::max(steveCoords[6], steveCoords[9])));
        float steveZMin = std::min(steveCoords[2], std::min(steveCoords[5], std::min(steveCoords[8], steveCoords[11])));
        float steveZMax = std::max(steveCoords[2], std::max(steveCoords[5], std::max(steveCoords[8], steveCoords[11])));

        float blockXMin = std::min(blockCoords[0], std::min(blockCoords[3], std::min(blockCoords[6], blockCoords[9])));
        float blockXMax = std::max(blockCoords[0], std::max(blockCoords[3], std::max(blockCoords[6], blockCoords[9])));
        float blockZMin = std::min(blockCoords[2], std::min(blockCoords[5], std::min(blockCoords[8], blockCoords[11])));
        float blockZMax = std::max(blockCoords[2], std::max(blockCoords[5], std::max(blockCoords[8], blockCoords[11])));



        if ((blockXMin < steveXMin && blockXMax > steveXMin) || (blockXMin < steveXMax && blockXMax > steveXMax)) {  
            if ((blockZMin < steveZMin && blockZMax > steveZMin) || (blockZMin < steveZMax && blockZMax > steveZMax)) {
                firstTest = true;
            }
        }

        if (!firstTest) {
            return false;
        }


        for (int i = 0; i <= 9; i += 3) {
            projectedSteveCoords[i] = steveCoords[i] / fabs(glm::cos(steveAngle));
            projectedSteveCoords[i + 1] = steveCoords[i + 1];
            projectedSteveCoords[i + 2] = steveCoords[i + 2] * fabs(glm::cos(steveAngle));

            projectedBlockCoords[i] = blockCoords[i] / fabs(glm::cos(steveAngle));
            projectedBlockCoords[i + 1] = blockCoords[i + 1];
            projectedBlockCoords[i + 2] = blockCoords[i + 2] * fabs(glm::cos(steveAngle));
        }

        steveXMin = std::min(projectedSteveCoords[0], std::min(projectedSteveCoords[3], std::min(projectedSteveCoords[6], projectedSteveCoords[9])));
        steveXMax = std::max(projectedSteveCoords[0], std::max(projectedSteveCoords[3], std::max(projectedSteveCoords[6], projectedSteveCoords[9])));
        steveZMin = std::min(projectedSteveCoords[2], std::min(projectedSteveCoords[5], std::min(projectedSteveCoords[8], projectedSteveCoords[11])));
        steveZMax = std::max(projectedSteveCoords[2], std::max(projectedSteveCoords[5], std::max(projectedSteveCoords[8], projectedSteveCoords[11])));

        blockXMin = std::min(projectedBlockCoords[0], std::min(projectedBlockCoords[3], std::min(projectedBlockCoords[6], projectedBlockCoords[9])));
        blockXMax = std::max(projectedBlockCoords[0], std::max(projectedBlockCoords[3], std::max(projectedBlockCoords[6], projectedBlockCoords[9])));
        blockZMin = std::min(projectedBlockCoords[2], std::min(projectedBlockCoords[5], std::min(projectedBlockCoords[8], projectedBlockCoords[11])));
        blockZMax = std::max(projectedBlockCoords[2], std::max(projectedBlockCoords[5], std::max(projectedBlockCoords[8], projectedBlockCoords[11])));


        if ((blockXMin < steveXMin && blockXMax > steveXMin) || (blockXMin < steveXMax && blockXMax > steveXMax)) {  
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
        return glm::ivec3(x, 2, z);  //glm::ivec3(x, objCoordinates.y, z);
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


    glm::ivec4 getNextChunkCoordinates(glm::ivec3 chunkRelCoords, glm::ivec3 chunkOrigin, int dirX, int dirZ, int height) {
        int x = chunkRelCoords.x + dirX;
        int z = chunkRelCoords.z + dirZ;

        int chunkOriginX = chunkOrigin.x;
        int chunkOriginZ = chunkOrigin.z;

        if (x < 0) {
            x += 16;
            if (chunkOriginX > 0) {
                chunkOriginX -= 16;
            }
            else {
                x = -1;
            }
            
        }

        else if (x > 15) {
            x -= 16;
            if (chunkOriginX < (defaultSize - 1) * CX) {
                chunkOriginX += 16;
            }
            else {
                x = -1;
            }
        }

        if (z < 0) {
            z += 16;
            if (chunkOriginZ > 0) {
                chunkOriginZ -= 16;
            }
            else {
                z = -1;
            }
        }

        else if (z > 15) {
            z -= 16;
            if (chunkOriginZ < (defaultSize - 1) * CZ) {
                chunkOriginZ += 16;
            }
            else {
                z = -1;
            }
        }

        if (height > 15 || height < 0) {
            x = -1;
            z = -1;
        }

        return glm::ivec4(x, chunkOriginX, z, chunkOriginZ);
    }

    void getIntersectedChunkWorldCoordinates(glm::ivec3 chunkRelCoords, glm::ivec3 chunkOrigin, int* coordinates) {
        glm::ivec4 nextBlock = getNextChunkCoordinates(chunkRelCoords, chunkOrigin, 0, 0, chunkRelCoords.y);

        if (nextBlock.x != -1 && nextBlock.z != -1) {
            coordinates[0] = nextBlock.x;
            coordinates[1] = nextBlock.y;
            coordinates[2] = chunkRelCoords.y;
            coordinates[3] = 0;
            coordinates[4] = nextBlock.z;
            coordinates[5] = nextBlock.w;
        }
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

    void computeOutOfBondsCollision(glm::vec4 steveWorldCoordinates) {  // Only launch after computing all lateral collisions with the map first
        int newSteveDirections[6];
        std::copy(std::begin(steveMoveDirections), std::end(steveMoveDirections), std::begin(newSteveDirections));

        if (steveWorldCoordinates.x > defaultSize* CX - 0.5) {
            newSteveDirections[0] = 0;
        }
        else if (steveWorldCoordinates.x < 0.5) {
            newSteveDirections[1] = 0;
        }

        if (steveWorldCoordinates.z > defaultSize* CZ - 0.5) {
            newSteveDirections[4] = 0;
        }
        else if (steveWorldCoordinates.z < 0.5) {
            newSteveDirections[5] = 0;
        }

        std::copy(std::begin(newSteveDirections), std::end(newSteveDirections), std::begin(steveMoveDirections));
    }

    void computeSideCollision(glm::ivec3 steveRelCoords, glm::ivec3 steveChunkOrigin, glm::vec4 steveWorldCoordinates, glm::mat4 steveM, float* steveBoundingBoxPoints) {
        float steveWorldLegPoints[12];
        glm::vec4 pointsContainer;
        glm::vec4 worldPointsContainer;
        float blockWorldPoints[12];

        for (int i = 0; i <= 9; i += 3) {
            pointsContainer = glm::vec4(steveBoundingBoxPoints[i], steveBoundingBoxPoints[i + 1], steveBoundingBoxPoints[i + 2], 1);
            worldPointsContainer = steveM * pointsContainer;
            steveWorldLegPoints[i] = worldPointsContainer.x;
            steveWorldLegPoints[i + 1] = worldPointsContainer.y;
            steveWorldLegPoints[i + 2] = worldPointsContainer.z;
        }
        
        bool collision;
        glm::ivec4 nextBlock;
        glm::ivec4 collisionDirection;

        for (int height = 1; height < 3; height++) {  // steves height
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    if (i != 0 || j != 0) {
                        nextBlock = getNextChunkCoordinates(steveRelCoords, steveChunkOrigin, i, j, steveRelCoords.y + height);
                        if (nextBlock.x != -1 && nextBlock.z != -1 && worldChunks[nextBlock.y / 16][0][nextBlock.w / 16]->getBlock(nextBlock.x, steveRelCoords.y + height, nextBlock.z)) { // the + height corresponds to the height I need to test 
                            computeBlockCorners(blockWorldPoints, glm::vec3(nextBlock.x + nextBlock.y, 0, nextBlock.z + nextBlock.w));
                            collision = testCollisionBetweenChunkSteve(blockWorldPoints, steveWorldLegPoints, steveRotation, i, j);

                            if (collision) {
                                glm::ivec4 collisionPoints = computeSteveLegCollisionPoints(steveWorldLegPoints, nextBlock);

                                if (i > 0) {  // Collision with front
                                    steveMoveDirections[0] = 0;
                                }
                                else if (i < 0) {
                                    steveMoveDirections[1] = 0;
                                }

                                if (j > 0) {
                                    steveMoveDirections[4] = 0;
                                }

                                else if (j < 0) {
                                    steveMoveDirections[5] = 0;
                                }

                                if (collisionPoints.z) {
                                    steveRotationDirection[1] = 0;
                                }

                                if (collisionPoints.w) {
                                    steveRotationDirection[0] = 0;
                                }

                                if (collisionPoints.x) {
                                    steveRotationDirection[0] = 0;
                                }

                                if (collisionPoints.y) {
                                    steveRotationDirection[1] = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    glm::ivec3 getSteveChunkPosition(glm::ivec3 chunkOrigin, glm::vec4 steveWorldCoordinates, glm::mat4 steveM, float* steveLegsBoudingBox) {
        float steveWorldLegPoints[12];

        glm::vec4 pointsContainer;
        glm::vec4 worldPointsContainer;

        for (int i = 0; i <= 9; i += 3) {
            pointsContainer = glm::vec4(steveLegsBoudingBox[i], steveLegsBoudingBox[i + 1], steveLegsBoudingBox[i + 2], 1);
            worldPointsContainer = steveM * pointsContainer;
            steveWorldLegPoints[i] = worldPointsContainer.x;
            steveWorldLegPoints[i + 1] = worldPointsContainer.y;
            steveWorldLegPoints[i + 2] = worldPointsContainer.z;
        }

        float blockWorldPoints[12];
        //Compute collision between Steve and the block he was in in the previous frame
        computeBlockCorners(blockWorldPoints, chunkOrigin + steveLastRel);
        bool collision = testCollisionBetweenChunkSteve(blockWorldPoints, steveWorldLegPoints, steveRotation, 0, 0);


        if (collision) { // Steve is still in the same block
            steveLastRel.y = steveWorldCoordinates.y - yWorldTranslation - 1; // Steve may have jumped so we update the y coordinate
            return steveLastRel;
        }

        else { // Steve no longer in the same block
            int x = floor(steveWorldCoordinates.x - chunkOrigin.x);
            int z = floor(steveWorldCoordinates.z - chunkOrigin.z);
            steveLastChunk = objectChunkCoordinates(steveWorldCoordinates);
            return glm::ivec3(x, steveWorldCoordinates.y - yWorldTranslation - 1, z);
        }
    }

    void resetSteveBlockedDirectionsRotation() {
        //This method resets the global variables that state if Steve is currently block in any give direction
        steveMoveDirections[0] = 1;
        steveMoveDirections[1] = 1;
        steveMoveDirections[4] = 1;
        steveMoveDirections[5] = 1;
        steveRotationDirection[0] = 1;
        steveRotationDirection[1] = 1;
    }

    void testAddBlock() {
        if (addBlock && currentEmptySelectedBlock[0] != -1) {
            worldChunks[currentEmptySelectedBlock[1] / 16][0][currentEmptySelectedBlock[5] / 16]->setBlock(currentEmptySelectedBlock[0], currentEmptySelectedBlock[2], currentEmptySelectedBlock[4], selectedBlockType);
            addBlock = false;
        }
        else if (addBlock) {
            addBlock = false;
        }
    }

    void testRemoveBlock() {
        if (removeBlock && currentNonEmptySelectedBlock[0] != -1) {
            worldChunks[currentNonEmptySelectedBlock[1] / 16][0][currentNonEmptySelectedBlock[5] / 16]->setBlock(currentNonEmptySelectedBlock[0], currentNonEmptySelectedBlock[2], currentNonEmptySelectedBlock[4], 0);
            removeBlock = false;
        }
        else if (removeBlock) {
            removeBlock = false;
        }
    }


    int computeRayStep(float rayDir) {
        int step = 0;

        if (rayDir > 0) {
            step = 1;
        }

        else if (rayDir < 0) {
            step = -1;
        }

        return step;
    }

    float computeRayTMax(int coord, int step, float rayDir) {
        if (rayDir != 0) {
            return fabs((coord + step) / rayDir);
        }

        else {
            return 99999999;
        }
    }

    void castRay(glm::ivec3 steveRelCoords, glm::vec4 steveWorldCoordinates, glm::ivec3 steveChunkOrigin, int maxDistance) {
        glm::vec3 rayDir = glm::normalize(glm::vec3(steveDirection.x, steveDirection.y, steveDirection.z));
        int stepX = computeRayStep(rayDir.x);
        int stepY = computeRayStep(rayDir.y);
        int stepZ = computeRayStep(rayDir.z);

        int x = stepX > 0 ? steveRelCoords.x : steveRelCoords.x + 1;
        int y = steveRelCoords.y + 2; // Camera position
        int z = stepZ > 0 ? steveRelCoords.z : steveRelCoords.z + 1;

        int blockOriginX, blockOriginY, blockOriginZ;
        int distance = 0;
       
        std::vector<glm::ivec3> castedPoints;

        
        float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;

        tDeltaX = tMaxX = fabs(1 / rayDir.x);
        tDeltaY = tMaxY = fabs(1 / rayDir.y);
        tDeltaZ = tMaxZ = fabs(1 / rayDir.z);

        int castedPoint[6] = { -1, -1, -1, -1, -1, -1 };
        bool foundBlock = false;
        bool foundEmpty = false;

        while (distance < maxDistance && !foundBlock) {
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    tMaxX = tMaxX + tDeltaX;
                    x += stepX;
                }

                else {
                    tMaxZ = tMaxZ + tDeltaZ;
                    z += stepZ;
                }
            }
            else {
                if (tMaxY < tMaxZ) {
                    tMaxY = tMaxY + tDeltaY;
                    y += stepY;
                }

                else {
                    tMaxZ = tMaxZ + tDeltaZ;
                    z += stepZ;
                }
            }

            blockOriginX = stepX > 0 ? x : x - 1;
            blockOriginY = y;
            blockOriginZ = stepZ > 0 ? z : z - 1;

            getIntersectedChunkWorldCoordinates(glm::ivec3(blockOriginX, blockOriginY, blockOriginZ), steveChunkOrigin, castedPoint);
            if (castedPoint[0] != -1) {
                int blockType = worldChunks[castedPoint[1] / 16][castedPoint[3] / 16][castedPoint[5] / 16]->getBlock(castedPoint[0], castedPoint[2], castedPoint[4]);
                if (blockType) {
                    std::copy(std::begin(castedPoint), std::end(castedPoint), std::begin(currentNonEmptySelectedBlock));
                    foundBlock = true;
                }
                else {
                    glm::ivec3 blockCoords = glm::ivec3(castedPoint[0], castedPoint[2], castedPoint[4]);
                    glm::ivec3 blockChunkOrigin = glm::ivec3(castedPoint[1], castedPoint[3], castedPoint[5]);
                    int blockType = 0;
                    for (int i = -1; i < 2; i += 2) {
                        glm::ivec4 nextBlockX = getNextChunkCoordinates(blockCoords, blockChunkOrigin, i, 0, blockCoords.y);
                        glm::ivec4 nextBlockY = getNextChunkCoordinates(blockCoords, blockChunkOrigin, 0, 0, blockCoords.y + i);
                        glm::ivec4 nextBlockZ = getNextChunkCoordinates(blockCoords, blockChunkOrigin, 0, i, blockCoords.y);

                        if (nextBlockX.x != -1 && nextBlockX.z != -1 && !blockType) {
                            blockType = worldChunks[nextBlockX.y / 16][0][nextBlockX.w / 16]->getBlock(nextBlockX.x, blockCoords.y, nextBlockX.z);
                        }

                        if (nextBlockY.x != -1 && nextBlockY.z != -1 && !blockType) {
                            blockType = worldChunks[nextBlockY.y / 16][0][nextBlockY.w / 16]->getBlock(nextBlockY.x, blockCoords.y + i, nextBlockY.z);
                        }

                        if (nextBlockZ.x != -1 && nextBlockZ.z != -1 && !blockType) {
                            blockType = worldChunks[nextBlockZ.y / 16][0][nextBlockZ.w / 16]->getBlock(nextBlockZ.x, blockCoords.y, nextBlockZ.z);
                        }
                    }
                    if (blockType) {
                        std::copy(std::begin(castedPoint), std::end(castedPoint), std::begin(currentEmptySelectedBlock));
                    }
                    else {
                        int noBlock[7] = { -1, -1, -1, -1, -1, -1, -1 };
                        std::copy(std::begin(noBlock), std::end(noBlock), std::begin(currentEmptySelectedBlock));
                        std::copy(std::begin(noBlock), std::end(noBlock), std::begin(currentNonEmptySelectedBlock));
                    }
                }
            }
            distance++;
        }
    }


public:
    World(int defaultSize, Shader& chunkShader) {
        this->defaultSize = defaultSize;
        this->texture = createTexture("VR_Assets/Textures/chunk.png", true);
        this->chunkShader = &chunkShader;
        initWorld();
    }


    glm::ivec3 getSelectedBlockWorldCoords() {
        return glm::ivec3(currentEmptySelectedBlock[0] + currentEmptySelectedBlock[1], currentEmptySelectedBlock[2] + currentEmptySelectedBlock[3] + yWorldTranslation, currentEmptySelectedBlock[4] + currentEmptySelectedBlock[5]);
    }

    void render(glm::mat4 &pv, Material chunkMaterial, glm::mat4 steveM, glm::vec3 steveHeight) {

        //We send/activate the chunks texture and material only once per frame
        std::string name = "texture_diffuse";
        std::string number = "1";
        std::string uniformNameString = (name + number);
        const GLchar* uniformName = uniformNameString.c_str();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(chunkShader->ID, uniformName), 0);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        setModelUniforms(*chunkShader, chunkMaterial);

        glm::vec4 coords;
        glm::mat4 mvp;

        int totalChunksCounter = 0;
        int drawnChunksCounter = 0;


        glm::vec4 steveWorldCoordinates = steveM * glm::vec4(0, 0, 0, 1);  // Get the current world coordinates from the player into a vec4

        glm::ivec3 steveChunkRenderCoordinates = objectChunkCoordinates(steveWorldCoordinates);  // Coordinates of the Chunk Origin Steve is currently in
        glm::ivec3 steveChunkCoordinates = steveLastChunk;  // Coordinates of the Chunk Origin Steve was in in the previous frame


        // Compute the chunks we should display according to the drawing distance
        int startChunkX = std::max(0, steveChunkRenderCoordinates.x / CX - drawingDistance);
        int endChunkX = std::min(defaultSize, steveChunkRenderCoordinates.x / CX + drawingDistance);
        int startChunkZ = std::max(0, steveChunkRenderCoordinates.z / CZ - drawingDistance);
        int endChunkZ = std::min(defaultSize, steveChunkRenderCoordinates.z / CZ + drawingDistance);
        
        // These will be passed to the Frag Shader to highlight the current block
        bool selectedValidBlock = false;  
        glm::vec4 selectedBlock = glm::vec4(0, 0, 0, 0);

        // If steve is flying fast, the Chunk he was previously in may stop being rendered. In that case, we no longer compute collision which is bad .To fix this, if in one frame we didn't compute collision, then we update the steveLastChunk position 
        bool flyingSteve = true; 

        for (int i = startChunkX; i < endChunkX; i++) {
            for (int j = startChunkZ; j < endChunkZ; j++) {
                mvp = pv * worldChunks[i][0][j]->getChunkModel();
                coords = mvp * chunkMiddle;
                totalChunksCounter++;

                // Compute NDC coordinates for x and y
                coords.x /= coords.w;  
                coords.y /= coords.w;

                if (coords.z < - CZ/2) {  // Chunk is behind the camera so we can discard it 
                    continue;
                }

                else if ((coords.x < -1.0 - (40 / fabsf(coords.w)) || coords.x > 1.0  + (40 / fabsf(coords.w)) || coords.y < -1.0 - (40 / fabsf(coords.w)) || coords.y > 1.0 + (40 / fabsf(coords.w)))) { //Chunk is outside of the screen, no need to draw it 
                    continue;
                }

                else {

                    // If we are drawing the Chunk steve is currently in, then we need to compute collisions
                    if (i * CX <= steveChunkCoordinates.x && (i + 1) * CX > steveChunkCoordinates.x && j * CZ <= steveChunkCoordinates.z && (j+1) * CZ > steveChunkCoordinates.z ) {  
                        flyingSteve = false;
                        glm::ivec3 steveRelCoords = getSteveChunkPosition(steveChunkCoordinates, steveWorldCoordinates, steveM, steveLegPoints);  // Only x and Z and relative to the current chunk
                        
                        resetSteveBlockedDirectionsRotation();

                        // We test collision with two different bounding boxes to try to improve the collision precision
                        computeSideCollision(steveRelCoords, steveChunkCoordinates, steveWorldCoordinates, steveM, steveBodyPoints);
                        computeSideCollision(steveRelCoords, steveChunkCoordinates, steveWorldCoordinates, steveM, steveLegPoints);

                        computeOutOfBondsCollision(steveWorldCoordinates);
                        castRay(steveRelCoords, steveWorldCoordinates, steveChunkCoordinates, 3);


                        testAddBlock();
                        testRemoveBlock();

                        if (currentNonEmptySelectedBlock[0] != -1) {
                            selectedValidBlock = true;
                            selectedBlock = glm::vec4(currentNonEmptySelectedBlock[0], currentNonEmptySelectedBlock[2], currentNonEmptySelectedBlock[4], 1);
                        }

                        int blockHeight = worldChunks[i][0][j]->getHeight(steveRelCoords.x, steveRelCoords.z) + yWorldTranslation;
                        if (steveWorldCoordinates.y - 1 < blockHeight && steveMoveDirections[3]) {
                            stevePos.y = blockHeight + 1;
                            steveMoveDirections[3] = 0;
                        }
                        else if (steveWorldCoordinates.y - 1 > blockHeight && !steveMoveDirections[3]){
                            fallingT = glfwGetTime();
                            steveMoveDirections[3] = 1;
                            steveLastChunk = steveChunkRenderCoordinates;
                        }

                        else if (steveWorldCoordinates.y - 1 > blockHeight) {
                            steveLastChunk = steveChunkRenderCoordinates;
                        }

                        steveLastRel = steveRelCoords;

                    }

                    drawnChunksCounter++;
                    this->chunkShader->setMatrix4("m", worldChunks[i][0][j]->getChunkModel());
                    this->chunkShader->setMatrix4("mvp", mvp);
                    this->chunkShader->setInteger("validSelectedBlock", selectedValidBlock);
                    this->chunkShader->setVector4f("blockSelected", selectedBlock);

                    if (selectedValidBlock && currentNonEmptySelectedBlock[1]/16 == i && currentNonEmptySelectedBlock[5]/16 == j) {
                        selectedValidBlock = false;
                    }

                    worldChunks[i][0][j]->render();
                    
                }
            }
        }

        steveLastPos = steveWorldCoordinates;
        if (flyingSteve) {
            steveLastChunk = steveChunkRenderCoordinates;
        }
    }

    void setModelUniforms(Shader shader, Material material) {
        shader.setFloat("material.ns", material.ns);
        shader.setFloat("material.ka", material.ka);
        shader.setFloat("material.kd", material.kd);
        shader.setFloat("material.ks", material.ks);
    }
    



};

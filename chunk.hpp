#pragma once

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/string_cast.hpp> //print matrix/vertices

#include "Model.hpp"

#define CX 16
#define CY 16
#define CZ 16




typedef glm::tvec4<GLbyte> byte4;

static const glm::vec3 xDirPos = glm::vec3(1, 0, 0);
static const glm::vec3 xDirNeg = glm::vec3(-1, 0, 0);

static const glm::vec3 yDirPos = glm::vec3(0, 1, 0);
static const glm::vec3 yDirNeg = glm::vec3(0, -1, 0);

static const glm::vec3 zDirPos = glm::vec3(0, 0, 1);
static const glm::vec3 zDirNeg = glm::vec3(0, 0, -1);




struct ChunkVertex {
	byte4 Position; 	
	glm::vec3 Normal;
};


class Chunk {
private:
	uint8_t block[CX][CY][CZ];
	GLuint VAO, VBO;
	int elements;
	bool changed;
	bool mergeVertices;
    bool isNoised = false;
	std::vector<ChunkVertex> vertices;
	glm::mat4 m;
    int chunkX, chunkY, chunkZ;
    int biomeType;
    Chunk *negX, *negZ, *posX, *posZ;

	ChunkVertex createVertex(GLbyte posX, GLbyte posY, GLbyte posZ, GLbyte type, float nX, float nY, float nZ) {
		ChunkVertex vertex;
		vertex.Position = byte4(posX, posY, posZ, type);
		vertex.Normal = glm::vec3(nX, nY, nZ);
		return vertex;
	}

    int getBlockTexture(int blockType, glm::vec3 direction) {
        if (blockType == 1) { //GRASS
            if (direction.x == -1 || direction.z == -1 || direction.x == 1 || direction.z == 1) {
                return 2;
            }
            else if (direction.y == 1) {
                return 3;
            }
            else {
                return 1;
            }
        }

        else if (blockType == 2) { // ROCK
            return 6;
        }

        else if (blockType == 3) { // DIRT
            return 1;
        }

        else if (blockType == 4) { // SAND
            return 7;
        }

        else if (blockType == 5) {  // WATER
            return 8;
        }

        else if (blockType == 6) { //TNT
            if (direction.x == -1 || direction.z == -1 || direction.x == 1 || direction.z == 1) {
                return 11;
            }
            else if (direction.y == 1) {
                return 12;
            }
            else {
                return 13;
            }
        }

        else if (blockType == 7) { // WOOD
            return 5;
        }
    }

    bool isVisible(int x, int y, int z, int neighX, int neighY, int neighZ) {
        if (block[x][y][z] == 0) {
            return false;
        }
        
        else {
            return getBlock(neighX, neighY, neighZ) == 0;
        }
    }

	void update() {
		changed = false;

		int i = 0;
		int xNegativeMerge[4] = { 0, 0, 0, 0 };
		int yNegativeMerge[4] = { 0, 0, 0, 0 };
        vertices.clear();

		// View from negative x and negative y
		for (int x = 0; x < CX; x++) {
			for (int y = 0; y < CY; y++) {
				bool xVisible = false;
				bool yVisible = false;
				for (int z = 0; z < CZ; z++) {
					uint8_t type = block[x][y][z];

					if (xVisible && block[x][y][z] == block[x][y][z - 1] && mergeVertices) {
						vertices[xNegativeMerge[0]] = createVertex(x, y, z + 1, xNegativeMerge[3], -1.0f, 0.0f, 0.0f);
						vertices[xNegativeMerge[1]] = createVertex(x, y, z + 1, xNegativeMerge[3], -1.0f, 0.0f, 0.0f);
						vertices[xNegativeMerge[2]] = createVertex(x, y + 1, z + 1, xNegativeMerge[3], -1.0f, 0.0f, 0.0f);
					}
                    

					else if (x >= 0 && isVisible(x, y, z, x - 1, y, z)) {
                        int blockTexture = getBlockTexture(type, xDirNeg);
						vertices.push_back(createVertex(x, y, z, blockTexture, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, blockTexture, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z, blockTexture, -1.0f, 0.0f, 0.0f));

						vertices.push_back(createVertex(x, y + 1, z, blockTexture, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, blockTexture, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, blockTexture, -1.0f, 0.0f, 0.0f));

						i += 6;
						xNegativeMerge[0] = i - 5;
						xNegativeMerge[1] = i - 2;
						xNegativeMerge[2] = i - 1;
                        xNegativeMerge[3] = blockTexture;
						xVisible = true;
					}
					else {
						xVisible = false;
					}

					if (yVisible && block[x][y][z] == block[x][y][z - 1] && mergeVertices) {					
						vertices[yNegativeMerge[0]] = createVertex(x, y, z + 1, yNegativeMerge[3] + 128, 0.0f, -1.0f, 0.0f);
						vertices[yNegativeMerge[1]] = createVertex(x, y, z + 1, yNegativeMerge[3] + 128, 0.0f, -1.0f, 0.0f);
						vertices[yNegativeMerge[2]] = createVertex(x + 1, y, z + 1, yNegativeMerge[3] + 128, 0.0f, -1.0f, 0.0f);
					}

					else if (y >= 0 && isVisible(x, y, z, x, y - 1, z)) {
                        int blockTexture = getBlockTexture(type, yDirNeg);
						vertices.push_back(createVertex(x, y, z, blockTexture + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z, blockTexture + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, blockTexture + 128, 0.0f, -1.0f, 0.0f));

						vertices.push_back(createVertex(x, y, z + 1, blockTexture + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z, blockTexture + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, blockTexture + 128, 0.0f, -1.0f, 0.0f));

						i += 6;
						yNegativeMerge[0] = i - 4;
						yNegativeMerge[1] = i - 3;
						yNegativeMerge[2] = i - 1;
                        yNegativeMerge[3] = blockTexture;

						yVisible = true;
					}
					else {
						yVisible = false;
					}
				}
			}
		}


		int xPositiveMerge[4] = { 0, 0, 0, 0};
		int yPositiveMerge[4] = { 0, 0, 0, 0};
		// View from positive x and positive y
		for (int x = CX - 1; x >= 0; x--) {
			for (int y = CY - 1; y >= 0; y--) {
				bool xVisible = false;
				bool yVisible = false;
				for (int z = CZ - 1; z >= 0; z--) {
					uint8_t type = block[x][y][z];

					if (xVisible && block[x][y][z] == block[x][y][z + 1] && mergeVertices) {
						vertices[xPositiveMerge[0]] = createVertex(x + 1, y + 1, z, xPositiveMerge[3], 1.0f, 0.0f, 0.0f);
						vertices[xPositiveMerge[1]] = createVertex(x + 1, y + 1, z, xPositiveMerge[3], 1.0f, 0.0f, 0.0f);
						vertices[xPositiveMerge[2]] = createVertex(x + 1, y, z, xPositiveMerge[3], 1.0f, 0.0f, 0.0f);
					}

					else if (x <= CX - 1 && isVisible(x, y, z, x + 1, y, z)) {
                        int blockTexture = getBlockTexture(type, xDirPos);
						vertices.push_back(createVertex(x + 1, y + 1, z, blockTexture, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, blockTexture, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, blockTexture, 1.0f, 0.0f, 0.0f));

						vertices.push_back(createVertex(x + 1, y, z, blockTexture, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, blockTexture, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, blockTexture, 1.0f, 0.0f, 0.0f));

						i += 6;
						xPositiveMerge[0] = i - 6;
						xPositiveMerge[1] = i - 2;
						xPositiveMerge[2] = i - 3;
                        xPositiveMerge[3] = blockTexture;
						xVisible = true;
					}
					else {
						xVisible = false;
					}


					if (yVisible && block[x][y][z] == block[x][y][z + 1] && mergeVertices) {
						vertices[yPositiveMerge[0]] = createVertex(x + 1, y + 1, z, yPositiveMerge[3] + 128, 0.0f, 1.0f, 0.0f);
						vertices[yPositiveMerge[1]] = createVertex(x + 1, y + 1, z, yPositiveMerge[3] + 128, 0.0f, 1.0f, 0.0f);
						vertices[yPositiveMerge[2]] = createVertex(x, y + 1, z, yPositiveMerge[3] + 128, 0.0f, 1.0f, 0.0f);
					}

					else if (y <= CY - 1 && isVisible(x, y, z, x, y + 1, z)) {
                        int blockTexture = getBlockTexture(type, yDirPos);
						vertices.push_back(createVertex(x, y + 1, z, blockTexture + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, blockTexture + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, blockTexture + 128, 0.0f, 1.0f, 0.0f));

						vertices.push_back(createVertex(x, y + 1, z + 1, blockTexture + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, blockTexture + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, blockTexture + 128, 0.0f, 1.0f, 0.0f));

						i += 6;
						yPositiveMerge[0] = i - 4;
						yPositiveMerge[1] = i - 1;
						yPositiveMerge[2] = i - 6;
                        yPositiveMerge[3] = blockTexture;
						yVisible = true;
					}
					else {
						yVisible = false;
					}
				}
			}
		}

		// View from negative z
		for (int x = 0; x < CX; x++) {
			for (int z = 0; z < CZ; z++) {
				bool visible = false;
                int blockTexture;
				for (int y = 0; y < CY; y++) {
					uint8_t type = block[x][y][z];


					if (visible && block[x][y - 1][z] == block[x][y][z] && mergeVertices) {
						vertices[i - 5] = createVertex(x, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f);
						vertices[i - 3] = createVertex(x, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f);
						vertices[i - 2] = createVertex(x + 1, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f);
					}

					else if (z >= 0 && isVisible(x, y, z, x, y, z - 1)) {
                        blockTexture = getBlockTexture(type, zDirNeg);
						vertices.push_back(createVertex(x, y, z, blockTexture, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y, z, blockTexture, 0.0f, 0.0f, -1.0f));

						vertices.push_back(createVertex(x, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, blockTexture, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y, z, blockTexture, 0.0f, 0.0f, -1.0f));

						i += 6;
						visible = true;
					}

					else {
						visible = false;
					}
				}
			}
		}


		// View from positive z
		for (int x = CX - 1; x >= 0; x--) {
			for (int z = CZ - 1; z >= 0; z--) {
				bool visible = false;
                int blockTexture;
				for (int y = CY - 1; y >= 0; y--) {
					uint8_t type = block[x][y][z];

					if (visible && block[x][y + 1][z] == block[x][y][z] && mergeVertices) {
						vertices[i - 6] = createVertex(x, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f);
						vertices[i - 5] = createVertex(x + 1, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f);
						vertices[i - 2] = createVertex(x + 1, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f);
					}

					else if (z <= CZ - 1 && isVisible(x, y, z, x, y, z + 1)) {
                        blockTexture = getBlockTexture(type, zDirPos);
						vertices.push_back(createVertex(x, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));

						vertices.push_back(createVertex(x, y + 1, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, blockTexture, 0.0f, 0.0f, 1.0f));

						i += 6;
						visible = true;
					}

					else {
						visible = false;
					}
				}
			}
		}

		elements = i;
		setupChunk();
	}

	void setupChunk() {
		// Create buffers/arrays
		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array. * sizeof(glm::vec4)
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(ChunkVertex), &this->vertices[0], GL_STATIC_DRAW);
		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, sizeof(ChunkVertex), (GLvoid*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (GLvoid*)offsetof(ChunkVertex, Normal));

		glBindVertexArray(0);
	}

    static float create2DNoise(int numIterations, int x, int y, int seed, float persistence, float scale, float low, float high) {
        // https://cmaher.github.io/posts/working-with-simplex-noise/
        float maxAmp = 0;
        float amp = 1.0;
        float freq = scale;
        float noise = 0;

        for (int i = 0; i < numIterations; i++) {
            noise += glm::simplex(glm::vec2(x + seed, y + seed) * freq) * amp ;
            maxAmp += amp;
            amp *= persistence;
            freq *= 2;
        }

        noise /= maxAmp;

        noise = noise * (high - low) / 2 + (high + low) / 2;

        return noise;
    }

    static int create3DNoise(int numIterations, int x, int y, int z, int seed, float persistence, float scale, float low, float high) {
        // https://cmaher.github.io/posts/working-with-simplex-noise/
        float maxAmp = 0;
        float amp = 1.0;
        float freq = scale;
        float noise = 0;

        for (int i = 0; i < numIterations; i++) {
            noise += glm::simplex(glm::vec3(x + seed, y + seed, z + seed) * freq) * amp;
            maxAmp += amp;
            amp *= persistence;
            freq *= 2;
        }

        noise /= maxAmp;

        int noiseFloor = std::floor(noise * (high - low) / 2 + (high + low) / 2);

        return noiseFloor;

    }



public:
    glm::vec4 chunkCenter;


    Chunk(float x, float y, float z, float scale, bool isNoised = false, bool mergeVertices = true) {
		memset(block, 0, sizeof(block)); // set all blocks to dirt by default
		this->mergeVertices = mergeVertices;
        this->isNoised = isNoised;
		this->m = Model::getM(x, y, z, scale);
        this->chunkX = x;
        this->chunkY = y;
        this->chunkZ = z;
        this->chunkCenter = glm::vec4(x + CX / 2, y + CY / 2, z + CZ / 2, 1);
        elements = 0;
        changed = true;
        noise(MAPSEED);
        negX = nullptr;
        posX = nullptr;
        negZ = nullptr;
        posZ = nullptr;
	}

	~Chunk() {
		glDeleteBuffers(1, &VBO);
	}

	uint8_t getBlock(int x, int y, int z) {
        if (x < 0)
            return negX != nullptr ? negX->getBlock(x + CX, y, z) : 0;
        if (z < 0)
            return negZ != nullptr ? negZ->getBlock(x, y, z + CZ) : 0;

        if (x >= CX)
            return posX != nullptr ? posX->getBlock(x - CX, y, z) : 0;

        if (z >= CZ)
            return posZ != nullptr ? posZ->getBlock(x, y, z - CZ) : 0;

        if (0 > y || y >= CY) {
            return 0;
        }
        
        return this->block[x][y][z];
	}

    int getHeight(int x, int z) {
        for (int i = CY - 1; i >= 0; i--) {
            if (this->getBlock(x, i, z)) {
                return i;
            }
        }
        return 0;
    }

	void setBlock(int x, int y, int z, uint8_t blockType) {
        if (0 <= x && x < CX && 0 <= y && y < CY && 0 <= z && z < CZ) { 
            this->block[x][y][z] = blockType;
            changed = true;

            if (x == 0 && negX != nullptr) {
                negX->askUpdate();
            }
            else if (x == CX - 1 && posX != nullptr) {
                posX->askUpdate();
            }


            if (z == 0 && negZ != nullptr) {
                negZ->askUpdate();
            }
            else if (z == CX - 1 && posZ != nullptr) {
                posZ->askUpdate();
            }


        }
	}

	glm::mat4 getChunkModel() {
		return this->m;
	}

    void setNegXNeighbour(Chunk* neighbour) {
        this->negX = neighbour;
    }

    void setNegZNeighbour(Chunk* neighbour) {
        this->negZ = neighbour;
    }

    void setPosXNeighbour(Chunk* neighbour) {
        this->posX = neighbour;
    }

    void setPosZNeighbour(Chunk* neighbour) {
        this->posZ = neighbour;
    }

    void noise(int seed) {
        float SEALEVEL = 5.0;

        biomeType = create2DNoise(5, chunkX, chunkZ, seed, 0.1, 0.005, 1.0, 4.0);
        int seed3D = seed / 125;

        if (isNoised)
            return;
        else
            isNoised = true;

        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                float n;
                int h;

                if (biomeType == 1) {
                    n = create2DNoise(5, x + chunkX, z + chunkZ, seed, 0.4, 0.005, SEALEVEL / 2, 6.0);
                    h = n * 2;
                    for (int y = 0; y < h; y++) {
                        block[x][y][z] = create3DNoise(3, x + chunkX, y + chunkY, z + chunkZ, seed3D, 0.01, 0.0005, 1.0, 2.9);
                    }
                    block[x][h][z] = 1;
                }

                else if (biomeType == 2) {
                    n = create2DNoise(5, x + chunkX, z + chunkZ, seed, 2, 0.005, SEALEVEL / 2, 8.0);
                    h = n * 2;
                    for (int y = 0; y <= h; y++) {
                        block[x][y][z] = create3DNoise(3, x + chunkX, y + chunkY, z + chunkZ, seed3D, 0.1, 0.05, 1.0, 3.9);
                    }
                }

                else {
                    n = create2DNoise(3, x + chunkX, z + chunkZ, seed3D, 0.1, 0.005, 0.0, 5.0);
                    h = n * 2;
                    for (int y = 0; y <= h; y++) {
                        block[x][y][z] = 4;
                    }
                }

                if (h < SEALEVEL && biomeType == 3) {
                    for (int y = h + 1; y <= SEALEVEL; y++) {
                        block[x][y][z] = 5;
                    }
                }

            }
        }
        changed = true;
    }

    void askUpdate() {
        this->changed = true;
    }


	void render() {
		if (changed) {
			update();
		}

		// If this chunk is empty, we don't need to draw anything.
		if (!elements)
			return;

		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, elements);
		glBindVertexArray(0);
	}

};

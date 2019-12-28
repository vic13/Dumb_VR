#pragma once

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> //print matrix/vertices

#include "Model.hpp"

#define CX 16
#define CY 16
#define CZ 16

typedef glm::tvec4<GLbyte> byte4;


struct ChunkVertex {
	byte4 Position; 	
	glm::vec3 Normal;
};


class Chunk {
private:
	uint8_t block[CX][CY][CZ];
	GLuint vbo;
	GLuint VAO, VBO, EBO;
	GLuint texture;
	int elements;
	bool changed;
	bool mergeVertices;
	std::vector<ChunkVertex> vertices;
	glm::mat4 m;

	ChunkVertex createVertex(GLbyte posX, GLbyte posY, GLbyte posZ, GLbyte type, float nX, float nY, float nZ) {
		ChunkVertex vertex;
		vertex.Position = byte4(posX, posY, posZ, type);
		vertex.Normal = glm::vec3(nX, nY, nZ);

		return vertex;
	}

	void update() {
		changed = false;

		// Each block has 36 vertices hence the 6*6
		int i = 0;
		int xNegativeMerge[3] = { 0, 0, 0 };
		int yNegativeMerge[3] = { 0, 0, 0 };

		// View from negative x and negative y
		for (int x = 0; x < CX; x++) {
			for (int y = 0; y < CY; y++) {
				bool xVisible = false;
				bool yVisible = false;
				for (int z = 0; z < CZ; z++) {
					uint8_t type = block[x][y][z];

					// Empty block?
					if (!type) {
						xVisible = false;
						yVisible = false;
						continue;
					}

					if (xVisible && block[x][y][z] == block[x][y][z - 1] && mergeVertices) {
						vertices[xNegativeMerge[0]] = createVertex(x, y, z + 1, type, -1.0f, 0.0f, 0.0f);
						vertices[xNegativeMerge[1]] = createVertex(x, y, z + 1, type, -1.0f, 0.0f, 0.0f); 
						vertices[xNegativeMerge[2]] = createVertex(x, y + 1, z + 1, type, -1.0f, 0.0f, 0.0f);
					}

					else if (x > 0 && block[x - 1][y][z] == 0 || x == 0) {
						vertices.push_back(createVertex(x, y, z, type, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, type, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z, type, -1.0f, 0.0f, 0.0f));

						vertices.push_back(createVertex(x, y + 1, z, type, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, type, -1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, type, -1.0f, 0.0f, 0.0f));

						i += 6;
						xNegativeMerge[0] = i - 5;
						xNegativeMerge[1] = i - 2;
						xNegativeMerge[2] = i - 1;

						xVisible = true;
					}
					else {
						xVisible = false;
					}

					if (yVisible && block[x][y][z] == block[x][y][z - 1] && mergeVertices) {					
						vertices[yNegativeMerge[0]] = createVertex(x, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f); 
						vertices[yNegativeMerge[1]] = createVertex(x, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f);
						vertices[yNegativeMerge[2]] = createVertex(x + 1, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f);
					}

					else if (y > 0 && !block[x][y - 1][z] || y == 0) {
						vertices.push_back(createVertex(x, y, z, type + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z, type + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f));

						vertices.push_back(createVertex(x, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z, type + 128, 0.0f, -1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, type + 128, 0.0f, -1.0f, 0.0f));

						i += 6;
						yNegativeMerge[0] = i - 4;
						yNegativeMerge[1] = i - 3;
						yNegativeMerge[2] = i - 1;

						yVisible = true;
					}
					else {
						yVisible = false;
					}
				}
			}
		}


		int xPositiveMerge[3] = { 0, 0, 0 };
		int yPositiveMerge[3] = { 0, 0, 0 };
		// View from positive x and positive y
		for (int x = CX - 1; x >= 0; x--) {
			for (int y = CY - 1; y >= 0; y--) {
				bool xVisible = false;
				bool yVisible = false;
				for (int z = CZ - 1; z >= 0; z--) {
					uint8_t type = block[x][y][z];

					if (!type) {
						xVisible = false;
						yVisible = false;
						continue;
					}

					if (xVisible && block[x][y][z] == block[x][y][z + 1] && mergeVertices) {
						vertices[xPositiveMerge[0]] = createVertex(x + 1, y + 1, z, type, 1.0f, 0.0f, 0.0f);
						vertices[xPositiveMerge[1]] = createVertex(x + 1, y + 1, z, type, 1.0f, 0.0f, 0.0f);
						vertices[xPositiveMerge[2]] = createVertex(x + 1, y, z, type, 1.0f, 0.0f, 0.0f);
					}

					else if (x < CX - 2 && block[x + 1][y][z] == 0 || x == CX - 1) {
						vertices.push_back(createVertex(x + 1, y + 1, z, type, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, type, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, type, 1.0f, 0.0f, 0.0f));

						vertices.push_back(createVertex(x + 1, y, z, type, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, type, 1.0f, 0.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, type, 1.0f, 0.0f, 0.0f));

						i += 6;
						xPositiveMerge[0] = i - 6;
						xPositiveMerge[1] = i - 2;
						xPositiveMerge[2] = i - 3;

						xVisible = true;
					}
					else {
						xVisible = false;
					}


					if (yVisible && block[x][y][z] == block[x][y][z + 1] && mergeVertices) {
						vertices[yPositiveMerge[0]] = createVertex(x + 1, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f);  
						vertices[yPositiveMerge[1]] = createVertex(x + 1, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f); 
						vertices[yPositiveMerge[2]] = createVertex(x, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f);
					}

					else if (y < CY - 2 && block[x][y + 1][z] == 0 || y == CY - 1 && mergeVertices) {
						vertices.push_back(createVertex(x, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, 3 + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f));

						vertices.push_back(createVertex(x, y + 1, z + 1, 3 + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, 3 + 128, 0.0f, 1.0f, 0.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, 3 + 128, 0.0f, 1.0f, 0.0f));

						i += 6;
						yPositiveMerge[0] = i - 4;
						yPositiveMerge[1] = i - 1;
						yPositiveMerge[2] = i - 6;
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
				for (int y = 0; y < CY; y++) {
					uint8_t type = block[x][y][z];

					// Empty block?
					if (!type) {
						visible = false;
					}

					else if (visible && block[x][y - 1][z] == block[x][y][z] && mergeVertices) {
						vertices[i - 5] = createVertex(x, y + 1, z, type, 0.0f, 0.0f, -1.0f);
						vertices[i - 3] = createVertex(x, y + 1, z, type, 0.0f, 0.0f, -1.0f);
						vertices[i - 2] = createVertex(x + 1, y + 1, z, type, 0.0f, 0.0f, -1.0f);
					}

					else if (z > 0 && !block[x][y][z - 1] || z == 0) {
						vertices.push_back(createVertex(x, y, z, type, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x, y + 1, z, type, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y, z, type, 0.0f, 0.0f, -1.0f));

						vertices.push_back(createVertex(x, y + 1, z, type, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z, type, 0.0f, 0.0f, -1.0f));
						vertices.push_back(createVertex(x + 1, y, z, type, 0.0f, 0.0f, -1.0f));

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
				for (int y = CY - 1; y >= 0; y--) {
					uint8_t type = block[x][y][z];

					// Empty block?
					if (!type) {
						visible = false;
					}

					else if (visible && block[x][y + 1][z] == block[x][y][z] && mergeVertices) {
						vertices[i - 6] = createVertex(x, y, z + 1, type, 0.0f, 0.0f, 1.0f);
						vertices[i - 5] = createVertex(x + 1, y, z + 1, type, 0.0f, 0.0f, 1.0f);
						vertices[i - 2] = createVertex(x + 1, y, z + 1, type, 0.0f, 0.0f, 1.0f);
					}

					else if (z < CZ - 2 && block[x][y][z + 1] == 0 || z == CZ - 1) {
						vertices.push_back(createVertex(x, y, z + 1, type, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, type, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x, y + 1, z + 1, type, 0.0f, 0.0f, 1.0f));

						vertices.push_back(createVertex(x, y + 1, z + 1, type, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y, z + 1, type, 0.0f, 0.0f, 1.0f));
						vertices.push_back(createVertex(x + 1, y + 1, z + 1, type, 0.0f, 0.0f, 1.0f));

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



public:
	Chunk(float x, float y, float z, float scale, bool mergeVertives=true) {
		memset(block, 2, sizeof(block));
		this->mergeVertices = mergeVertices;
		elements = 0;
		changed = true;
        this->texture = createTexture("chunk.png", true);
		this->m = Model::getM(x, y, z, scale);
	}

	~Chunk() {
		glDeleteBuffers(1, &vbo);
	}

	unsigned char getBlock(int x, int y, int z) {
		return block[x][y][z];
	}

	void setBlock(int x, int y, int z, unsigned char blockType) {
		block[x][y][z] = blockType;
		changed = true;
	}

	glm::mat4 getChunkModel() {
		return this->m;
	}

	void render(Shader shader) {
		if (changed) {
			update();
		}

		// If this chunk is empty, we don't need to draw anything.
		if (!elements)
			return;

		int i = 4;

		glEnable(GL_CULL_FACE);  // Order of the vertices matters because of this. Vertices need to be placed in a clock-wise manner otherwise they won't be displayed. Read more at https://en.wikipedia.org/wiki/Back-face_culling
		glEnable(GL_DEPTH_TEST);
		
		std::string name = "texture_diffuse";
		std::string number = "1";
		const GLchar* uniformName = (name + number).c_str();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.ID, uniformName), 0);
		glBindTexture(GL_TEXTURE_2D, this->texture);


		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, elements);
		glBindVertexArray(0);
	}

	void renderMultiple() {
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

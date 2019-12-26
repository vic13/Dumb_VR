#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// GL Includes
#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"

#define OBJ_PATH "VR_Assets/Objs/"
#define TEX_PATH "VR_Assets/Textures/"

GLint TextureFromFile(const char* path, std::string directory);

std::string getDiffuseTexPath(const char* texName);
std::string getNormalTexPath(const char* texName);
std::string getObjPath(const char* objName);

GLuint createTexture(std::string path, bool alpha);
glm::mat4 getM(float x, float y, float z, float scale);
void updatePosition(float x, float y, float z);
void updateRotation(float rotationAngle, glm::vec3 rotationAxis);

class Model 
{
public:
    GLuint texture;
    glm::mat4 m;
    float ns;
	const char* name;
    
    float x;
    float y;
    float z;
    float scale;
    

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const char* name, bool normal, float x, float y, float z, float scale, float ns = 0, bool tangent = false, bool alpha = true)
    {
        this->normal = normal;
        this->tangent = tangent;
        this->alpha = alpha;
        this->x = x;
        this->y = y;
        this->z = z;
        this->scale = scale;
        this->m = getM(x, y, z, scale);
        this->ns = ns;
		this->name = name;
        this->loadModel(getObjPath(name));
    }

    // Draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader);
    }
    
    void DrawMultiple()
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].DrawMultiple();
    }
    
    void updatePosition(float x, float y, float z) {
        this->m = getM(x, y, z, this->scale);
    }
    
    void updateRotation(float rotationAngle, glm::vec3 rotationAxis) {
        this->m = getM(this->x, this->y, this->z, this->scale, rotationAngle, rotationAxis);
    }

	glm::mat4 getM(float x, float y, float z, float scale, float rotationAngle=0, glm::vec3 rotationAxis = glm::vec3(1, 0, 0)) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1), rotationAngle, rotationAxis);
		glm::mat4 scaling = glm::scale(glm::mat4(1), glm::vec3(scale, scale, scale));
		glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(x, y, z));
		return translation * rotation * scaling;
	}

private:
    bool normal;
    bool tangent;
    bool alpha;
    /*  Model Data  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	

    /*  Functions   */
    std::string getDiffuseTexPath(const char* texName) {
        std::string str = std::string(TEX_PATH) + std::string(texName) + ".png";
		std::cout << str << std::endl;
        return str;
    }
    
    std::string getNormalTexPath(const char* texName) {
        std::string str = std::string(TEX_PATH) + std::string(texName) + "_normal.png";
        std::cout << str << std::endl;
        return str;
    }

    std::string getObjPath(const char* objName) {
        std::string str = std::string(OBJ_PATH) + std::string(objName) + ".obj";
		std::cout << str << std::endl;
        return str;
    }

    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene. 
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
            this->meshes.push_back(this->processMesh(mesh, scene));			
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        
		// Data to fill
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // We declare a placeholder std::vector since assimp uses its own std::vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normals
            if (this->normal) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // Tangents
            if (this->tangent) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, vertex.Tangent));
            }
            // Texture Coordinates
            if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices std::vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

		
		// This loads the texture inside the model and stores it in the textures_loaded vector
		Texture texture;
		std::string texturePath = getDiffuseTexPath(this->name);
		texture.id = createTexture(texturePath, this->alpha);
		texture.type = "texture_diffuse";
		textures.push_back(texture);
		this->textures_loaded.push_back(texture);  // This is not used atm. Could be used to avoid the same texture from being loaded more than one time
	
        if (this->tangent) {
            Texture textureNormal;
            std::string textureNormalPath = getNormalTexPath(this->name);
            textureNormal.id = createTexture(textureNormalPath, this->alpha);
            textureNormal.type = "texture_normal";
            textures.push_back(textureNormal);
            this->textures_loaded.push_back(textureNormal);
        }

        // Process materials Only uncomment if we decide to use .mtb files

        //if(mesh->mMaterialIndex >= 0)
        //{
        //    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        //    // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        //    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        //    // Same applies to other texture as the following list summarizes:
        //    // Diffuse: texture_diffuseN
        //    // Specular: texture_specularN
        //    // Normal: texture_normalN
        //    // 1. Diffuse std::maps
        //    std::vector<Texture> diffusemaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        //    textures.insert(textures.end(), diffusemaps.begin(), diffusemaps.end());
        //    // 2. Specular std::maps
        //    std::vector<Texture> specularmaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        //    textures.insert(textures.end(), specularmaps.begin(), specularmaps.end());
        //}
        
        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};



GLuint createTexture(std::string path, bool alpha) {
    int texWidth, texHeight, n;

    unsigned char* data = stbi_load(path.c_str(), &texWidth, &texHeight, &n, 0);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if (alpha) {
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}


GLint TextureFromFile(const char* path, std::string directory)
{
	std::cout << "from_file" << path << std::endl;
     //Generate texture ID and load texture data 
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height, components;
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &components, 0);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);	

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
    return textureID;
}

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>


#include "Material.h"
#include "Component.h"
#include "ECSEngine.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>
//#include "TempRender.h"

const GLint vertexAttrib1 = 0;
const GLint rgbaAttrib1   = 1;
const GLint normAttrib1   = 2;
const GLint tanAttrib1    = 3;
const GLint bitanAttrib1  = 4;
const GLint texAttrib1    = 5;

class LevelLoader{
private:

public:
	LevelLoader();
	ECSEngine& openLevel(std::string & directory); 
};



struct Mesh{
	GLuint VBO;
	GLuint IBO;
	glm::vec4 location;
	int numVertices;
	int indexCount;
	std::string name;
	unsigned int materialIndex;
	bool hasTangents;
	int count; // number of objects that use this Mesh. used to create render-only entities from non used objects
	Mesh();
	Renderable getRenderableComponent();
	Transform getTransformComponent();
};


struct Vertex {
	glm::vec4 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	Vertex();
	Vertex(const glm::vec4 &pos, const glm::vec2 &uvCoords, const glm::vec3 &norm, const glm::vec3 &tan, const glm::vec3 &bitan)
	{
		position = pos;
		uv = uvCoords;
		normal = norm;
		tangent = tan;
		bitangent = bitan;
	}
};


class Scene {
private:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	TextureLoader texManager;

	void processScene(const aiScene* scene, std::string & filename);
	void createMesh(unsigned int index, const aiMesh* m);
	void processMaterials(const aiScene* scene, std::string & filename);

public:
	Scene();
	bool openFile(std::string & filename);
	//Renderable getByName(std::string & name);
	void print();
	void render();
};

#endif
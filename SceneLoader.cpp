#include "LevelLoader.h"

Scene::Scene(){
	texManager = TextureLoader();
}

bool Scene::openFile(std::string& filename)
{
	Assimp::Importer Importer;
	const aiScene* scene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

	if(scene)
	{
		processScene(scene, filename);
		return true;
	}
	else {
		std::cout << "Error loading file: " << filename << "\n" << Importer.GetErrorString() << std::endl;
		return false;
	}
}

void Scene::processScene(const aiScene* scene, std::string & filename)
{
	std::cout << "Reading Scene" << std::endl;
	int numMeshes = scene->mNumMeshes;
	for(unsigned int i = 0; i < numMeshes; i++)
	{
		createMesh(i, scene->mMeshes[i]); // might need to create a separate const aiMesh* variable for scene->m Meshes[i]
	}
	std::cout << "Finished Reading Scene" << std::endl;
	processMaterials(scene, filename);
}

void Scene::createMesh(unsigned int index, const aiMesh* m)
{
	//std::cout << "Reading Mesh" << std::endl;
	meshes.push_back(Mesh());
	meshes[index].materialIndex = m->mMaterialIndex + materials.size();
	meshes[index].hasTangents = m->HasTangentsAndBitangents();
	meshes[index].name = std::string(m->mName.C_Str());
	meshes[index].numVertices = m->mNumVertices;
	meshes[index].indexCount = m->mNumFaces * 3;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	const aiVector3D zero(0.0f,0.0f,0.0f);
	glm::vec4 averageLocation = glm::vec4(0.0,0.0,0.0,0.0); // used to compute a transform component for the mesh

	for(unsigned int i = 0; i < m->mNumVertices; i++)
	{
		const aiVector3D* position = &(m->mVertices[i]);
		const aiVector3D* uv = m->HasTextureCoords(0)? &(m->mTextureCoords[0][i]) : &zero;
		const aiVector3D* normal = &(m->mNormals[i]);
		const aiVector3D* tangent = m->HasTangentsAndBitangents()? &(m->mTangents[i]) : &zero;
		const aiVector3D* bitangent = m->HasTangentsAndBitangents()? &(m->mBitangents[i]) : &zero;

		Vertex vert = Vertex(glm::vec4(position->x, position->y, position->z, 50.0), 
							 glm::vec2(uv->x, uv->y), 
							 glm::vec3(normal->x,normal->y, normal->z),
							 glm::vec3(tangent->x, tangent->y, tangent->z),
							 glm::vec3(bitangent->x, bitangent->y, bitangent->z));
		vertices.push_back(vert);
		averageLocation =averageLocation + vert.position;
	}
	averageLocation.x = averageLocation.x / meshes[index].numVertices;
	averageLocation.y = averageLocation.y / meshes[index].numVertices;
	averageLocation.z = averageLocation.z / meshes[index].numVertices;
	averageLocation.w = averageLocation.w / meshes[index].numVertices;
	meshes[index].location = averageLocation;

	//std::cout << averageLocation.x << ", " << averageLocation.y << ", " << averageLocation.z << ", " << averageLocation.w << std::endl;

	for(int i = 0; i < meshes[index].numVertices; i++)
	{
		vertices[i].position -= averageLocation;
		vertices[i].position.w = averageLocation.w;
	}


	for(unsigned int i = 0; i < m->mNumFaces; i++)
	{
		const aiFace &face = m->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	glGenBuffers(1, &(meshes[index].VBO));
  	glBindBuffer(GL_ARRAY_BUFFER, meshes[index].VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &(meshes[index].IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[index].IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

}

void Scene::processMaterials(const aiScene* scene, std::string & filename)
{
	std::string::size_type pathEnd = filename.find_last_of("/");
	std::string directory;

	if(pathEnd == std::string::npos)
		directory = ".";
	else if(pathEnd == 0)
		directory = "/";
	else directory = filename.substr(0,pathEnd);
	int materialIndexOffset = materials.size();

	std::cout << "Number of Materials: " << scene->mNumMaterials << std::endl;
	std::cout << "has Textures: " << scene->mNumTextures << std::endl;
	for(int i = 0; i < scene->mNumMaterials; i++)
	{
		int materialIndex = materialIndexOffset + i;
		materials.push_back(Material());
		const aiMaterial* currentMaterial = scene->mMaterials[i];

		//get all color properties of the material

		//read ambient color property
		aiColor3D ambientColor;
		if(currentMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS)
		{
			materials[materialIndex].ambient = glm::vec3(ambientColor.r,ambientColor.g,ambientColor.b);
		}
		//read diffuse color property
		aiColor3D diffuseColor;
		if(currentMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
		{
			materials[materialIndex].diffuse = glm::vec3(diffuseColor.r,diffuseColor.g,diffuseColor.b);
		}
		//read specular color property
		aiColor3D specularColor;
		if(currentMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
		{
			materials[materialIndex].specular = glm::vec3(specularColor.r,specularColor.g,specularColor.b);
		}
		//read transparent color property
		aiColor3D transparentColor;
		if(currentMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor) == AI_SUCCESS)
		{
			materials[materialIndex].transparentColor = glm::vec3(transparentColor.r,transparentColor.g,transparentColor.b);
		}


		//read floating point properties for the material
		float opacity;
		if(currentMaterial->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
		{
			materials[materialIndex].opacity = opacity;
		}

		float shiny;
		if(currentMaterial->Get(AI_MATKEY_SHININESS, shiny) == AI_SUCCESS)
		{
			materials[materialIndex].shininess = shiny;
		}

		// float shinystrength;
		// if(currentMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shinystrength) == AI_SUCCESS)
		// {
		// 	std::cout << "Shiny strength: " << shinystrength << std::endl;
		// }

		// load diffuse texture for the material
		if(currentMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			//std::cout << "Diffuse Texture Exists" << std::endl;
			aiString texture_path;
			if(currentMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string file_path = directory + "/" + texture_path.data;
				std::cout << "Texture: " << file_path << std::endl;
				materials[materialIndex].texture = texManager.loadTexture(file_path);
			}
		}


		// load normal texture if it exists
		if(currentMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			//std::cout << "Texture Exists" << std::endl;
			aiString texture_path;
			if(currentMaterial->GetTexture(aiTextureType_NORMALS, 0, &texture_path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string file_path = directory + "/" + texture_path.data;
				std::cout << "Texture: " << file_path << std::endl;
				materials[materialIndex].normals = texManager.loadTexture(file_path);
			}
		}

		//materials[materialIndex].print();
	}
}

void Scene::print()
{
	for(int i = 0; i < meshes.size(); i++)
	{
		std::cout << meshes[i].name << std::endl;
	}
}

//this will be deleted. this is only for testing purposes
void Scene::render()
{
	for(int i = 0; i < meshes.size(); i++)
	{
		glBindBuffer( GL_ARRAY_BUFFER, meshes[i].VBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, meshes[i].IBO );
		glVertexAttribPointer( vertexAttrib1, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0) );
		glVertexAttribPointer( rgbaAttrib1,   4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0 *sizeof(GLfloat)) );
		glVertexAttribPointer( normAttrib1,   3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(6 *sizeof(GLfloat)) );
		glVertexAttribPointer( tanAttrib1,    3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(9*sizeof(GLfloat)) );
		glVertexAttribPointer( bitanAttrib1,  3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(12*sizeof(GLfloat)) );
		glVertexAttribPointer( texAttrib1,    2, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)) );
		glDrawElements( GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT, 0 );
	}
}

Mesh::Mesh(){}

// Renderable& getByName(std::string& name)
// {
// 	//TODO
// 	return NULL
// }
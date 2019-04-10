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
		//delete scene;
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

	processNodes(scene->mRootNode, scene);

	// int numMeshes = scene->mNumMeshes;
	// for(unsigned int i = 0; i < numMeshes; i++)
	// {
	// 	std::string test = "";
	// 	createMesh(scene->mMeshes[i], test); // might need to create a separate const aiMesh* variable for scene->m Meshes[i]
	// }
	std::cout << "Finished Reading Scene" << std::endl;
	processMaterials(scene, filename);
	processLights(scene);
	// processNodes(scene->mRootNode, scene);
	// print();
}

void Scene::processNodes(aiNode * node, const aiScene* scene)
{
	//std::cout << "Node name: " << node->mName.C_Str() << ", Number of Meshes: " << node->mNumMeshes << std::endl;
	std::string nodeName = std::string(node->mName.C_Str());
	for(int i = 0; i < node->mNumMeshes; i++)
	{
		createMesh(scene->mMeshes[node->mMeshes[i]],nodeName, scene);
	}
	for(int i = 0; i < node->mNumChildren; i++)
	{
		processNodes(node->mChildren[i], scene);
	}
}

void Scene::createMesh(const aiMesh* m, std::string &name, const aiScene * scene)
{
	//std::cout << "Reading Mesh" << std::endl;
	int index = meshes.size();
	meshes.push_back(Mesh());
	meshes[index].name = name;
	//std::cout << "Mesh Name: " << meshes[index].name << " Original name: " << name << std::endl;
	meshes[index].materialIndex = m->mMaterialIndex + materials.size();
	meshes[index].hasTangents = m->HasTangentsAndBitangents();
	//meshes[index].name = std::string(m->mName.C_Str());
	meshes[index].numVertices = m->mNumVertices;
	meshes[index].indexCount = m->mNumFaces * 3;

	aiMatrix4x4 transform = getTransformation(scene, meshes[index].name);
	aiMatrix4x4 normalMatrix = transform;
	normalMatrix.Inverse().Transpose();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	const aiVector3D zero(0.0f,0.0f,0.0f);
	glm::vec4 averageLocation = glm::vec4(0.0,0.0,0.0,0.0); // used to compute a transform component for the mesh

	for(unsigned int i = 0; i < m->mNumVertices; i++)
	{
		aiVector3D position = transform * (m->mVertices[i]);
		const aiVector3D* uv = m->HasTextureCoords(0)? &(m->mTextureCoords[0][i]) : &zero;
		aiVector3D normal = normalMatrix * (m->mNormals[i]);
		aiVector3D tangent = normalMatrix * (m->mTangents[i]);
		aiVector3D bitangent = normalMatrix * (m->mBitangents[i]);

		Vertex vert = Vertex(glm::vec4(position.x, position.y, position.z, 1.0), 
							 glm::vec2(uv->x, uv->y), 
							 glm::vec3(normal.x,normal.y, normal.z),
							 glm::vec3(tangent.x, tangent.y, tangent.z),
							 glm::vec3(bitangent.x, bitangent.y, bitangent.z));
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
		aiString matName;
		currentMaterial->Get(AI_MATKEY_NAME, matName);
		std::cout << "Name: " << matName.C_Str() << std::endl;
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
			std::cout << "Shininess " << shiny << std::endl;
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
				std::cout << "Color Texture: " << file_path << std::endl;
				materials[materialIndex].texture = texManager.loadTexture(file_path, true);
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
				std::cout << "Normal Texture: " << file_path << std::endl;
				materials[materialIndex].normals = texManager.loadTexture(file_path, false);
			}
		}

		if(currentMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
		{
			//std::cout << "Texture Exists" << std::endl;
			aiString texture_path;
			if(currentMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string file_path = directory + "/" + texture_path.data;
				std::cout << "Emissive Texture: " << file_path << std::endl;
				materials[materialIndex].emissive = texManager.loadTexture(file_path, true);
			}
		}

		//materials[materialIndex].print();
	}
}


void Scene::processLights(const aiScene* scene)
{
	std::cout << "Number of lights: " << scene->mNumLights << std::endl;
	for(int i = 0; i < scene->mNumLights; i++)
	{
		const aiLight* currentLight = scene->mLights[i];
		std::cout << "Name: " << currentLight->mName.C_Str() << std::endl;
		std::cout << "Type: " << currentLight->mType << std::endl;
		
		std::cout << "constant Attenuation: "<< currentLight->mAttenuationConstant << std::endl;
		std::cout << "linear Attenuation: "<< currentLight->mAttenuationLinear << std::endl;
		std::cout << "quadratic Attenuation: "<< currentLight->mAttenuationQuadratic << std::endl;

		int index = lights.size();
		lights.push_back(LightData());
		lights[index].name = std::string(currentLight->mName.C_Str());
		aiMatrix4x4 transform = getTransformation(scene, lights[index].name);
		aiVector3D transformedLocation = transform * currentLight->mPosition;

		std::cout << "Location: " << transformedLocation.x << "," << transformedLocation.y << "," << transformedLocation.z << std::endl;
		lights[index].location = glm::vec3(transformedLocation.x, transformedLocation.y, transformedLocation.z);
		lights[index].diffuse = glm::vec3(currentLight->mColorDiffuse.r, currentLight->mColorDiffuse.g, currentLight->mColorDiffuse.b );
		lights[index].specular = glm::vec3(currentLight->mColorSpecular.r, currentLight->mColorSpecular.g, currentLight->mColorSpecular.b);
		lights[index].linearAttenuation = currentLight->mAttenuationLinear;
		lights[index].quadraticAttenuation = currentLight->mAttenuationQuadratic;

	}
}

aiMatrix4x4 Scene::getTransformation(const aiScene * scene, std::string &name)
{
	aiNode * root = scene->mRootNode;
	return getTransformationHelper(root->FindNode(name.c_str()));
}

aiMatrix4x4 Scene::getTransformationHelper(aiNode * node)
{
	if(node->mParent == NULL)
		return node->mTransformation;
	return getTransformationHelper(node->mParent) * node->mTransformation;
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

ComponentWrapper * Scene::getMeshByName(std::string & name, int entityID)
{
	bool found = false;
	int i = 0; 
	while(!found && i < meshes.size())
	{
		if(name.compare(meshes[i].name) == 0)
			found = true;
		i++;
	}
	if(!found)
	{
		return NULL;
	}
	meshes[i].count++;
	return createMeshWrapper(i, entityID);

}
int Scene::getUnusedMeshCount()
{
	int unusedCount = 0;
	for(int i = 0; i < meshes.size();i++)
	{
		if(meshes[i].count == 0)
			unusedCount++;
	}	
	return unusedCount;
}
ComponentWrapper * Scene::getUnusedMesh(int entityID) // gets first unused 
{
	bool found = false;
	int i = 0;
	while(!found && i < meshes.size())
	{
		if(meshes[i].count == 0)
			found = true;
		else i++;
	}
	if(!found)
	{
		return NULL;
	}
	meshes[i].count++;
	return createMeshWrapper(i,entityID);
}

Mesh::Mesh(){}



ComponentWrapper * Scene::createMeshWrapper(int index, int entityID)
{
	
	glm::vec3 defaultOrientation = glm::vec3(0.0,0.0,0.0);
	Transform t = Transform(meshes[index].location, defaultOrientation, 1.0,entityID);
	Renderable r  = Renderable(meshes[index].VBO, meshes[index].IBO, meshes[index].indexCount, 0, materials[meshes[index].materialIndex], entityID);
	if(r.material.normals == 0)
		r.program = ShaderManager::deferredBasic;
	else r.program = ShaderManager::deferredNormal;
	ComponentWrapper *wrapper = new ComponentWrapper();
	std::string lightName = meshes[index].name + "Light";
	for(int i = 0; i < lights.size(); i++)
	{

		if(lights[i].name.compare(lightName) == 0)
		{
			//std::cout << "match found" << std::endl;
			lights[i].count++;
			Light l = Light();
			l.ownerID = entityID;
			l.location = lights[i].location - glm::vec3(meshes[index].location) * t.scale;
			// std::cout << "light position: " << l.location.x << ", " << l.location.y << ", " << l.location.z << std::endl;
			// std::cout << "light original position: " << lights[i].location.x << ", " << lights[i].location.y << ", " << lights[i].location.z << std::endl;
			l.diffuse = lights[i].diffuse;
			l.specular = lights[i].specular;
			l.linearAttenuation = lights[i].linearAttenuation;
			l.quadraticAttenuation = lights[i].quadraticAttenuation;

			wrapper->hasLight = true;
			wrapper->l = l;
		}
	}
	wrapper->hasRenderable = true;
	wrapper->r = r;
	wrapper->hasTransform = true;
	wrapper->t = t;
	return wrapper;
}

int Scene::getUnusedLightCount()
{
	int unusedCount = 0;
	for(int i = 0; i < lights.size();i++)
	{
		if(lights[i].count == 0)
			unusedCount++;
	}	
	return unusedCount;
}
ComponentWrapper * Scene::getUnusedLight(int entityID)
{
	bool found = false;
	int i = 0;
	while(!found && i < lights.size())
	{
		if(lights[i].count == 0)
			found = true;
		else i++;
	}
	if(!found)
	{
		return NULL;
	}
	lights[i].count++;
	return createLightWrapper(i,entityID);
}
ComponentWrapper * Scene::createLightWrapper(int index, int entityID)
{
	glm::vec3 zeroVec3 = glm::vec3(0.0,0.0,0.0);
	glm::vec4 lightPosition = glm::vec4(lights[index].location,1.0);
	Transform t = Transform(lightPosition, zeroVec3, 1.0,entityID);
	Light l = Light(zeroVec3, lights[index].diffuse, lights[index].specular, lights[index].linearAttenuation, lights[index].quadraticAttenuation, entityID);
	ComponentWrapper *wrapper = new ComponentWrapper();

	wrapper->hasLight = true;
	wrapper->l = l;
	wrapper->hasRenderable = false;
	wrapper->hasTransform = true;
	wrapper->t = t;
	return wrapper;
}



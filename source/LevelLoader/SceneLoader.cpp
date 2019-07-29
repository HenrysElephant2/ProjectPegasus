#include "LevelLoader.h"

Scene::Scene(){
	texManager = TextureLoader();
	entityManager = EntityManager::getEntityManager();
}

bool Scene::openFile(std::string& filename)
{
	Assimp::Importer Importer;
	const aiScene* scene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights | aiProcess_FlipUVs | aiProcess_GenUVCoords);

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

	
	std::cout << "Finished Reading Scene" << std::endl;
	processMaterials(scene, filename);
	processLights(scene);
	
}

void Scene::processNodes(aiNode * node, const aiScene* scene)
{
	//std::cout << "Node name: " << node->mName.C_Str() << ", Number of Meshes: " << node->mNumMeshes << std::endl;
	std::string nodeName = std::string(node->mName.C_Str());
	for(int i = 0; i < node->mNumMeshes; i++)
	{
		if(scene->mMeshes[node->mMeshes[i]]->HasBones())
			createSkinnedMesh(scene->mMeshes[node->mMeshes[i]],nodeName, scene);
		else {
			createMesh(scene->mMeshes[node->mMeshes[i]],nodeName, scene);
		}
	}
	for(int i = 0; i < node->mNumChildren; i++)
	{
		processNodes(node->mChildren[i], scene);
	}
}

void Scene::createMesh(const aiMesh* m, std::string &name, const aiScene * scene)
{
	std::cout << "Reading Mesh" << std::endl;
	int index = meshes.size();
	meshes.push_back(Mesh());
	meshes[index].name = name;
	std::cout << "Mesh Name: " << meshes[index].name << " Original name: " << name << std::endl;
	meshes[index].materialIndex = m->mMaterialIndex + materials.size();
	meshes[index].hasTangents = m->HasTangentsAndBitangents();
	//std::cout << "Has Tangents and Bitangents? " << m->HasTangentsAndBitangents();
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

	std::cout << "Reading vertices" << std::endl;
	for(unsigned int i = 0; i < m->mNumVertices; i++)
	{
		aiVector3D position = transform * (m->mVertices[i]);
		//std::cout << "Read Position" << std::endl;
		const aiVector3D* uv = m->HasTextureCoords(0)? &(m->mTextureCoords[0][i]) : &zero;
		//std::cout << "Read UV" << std::endl;
		aiVector3D normal = normalMatrix * (m->mNormals[i]);
		//std::cout << "Read normal" << std::endl;
		aiVector3D tangent = normalMatrix * (m->mTangents[i]);
		//std::cout << "Read tangent" << std::endl;
		aiVector3D bitangent = normalMatrix * (m->mBitangents[i]);
		//std::cout << "Read normals" << std::endl;

		Vertex vert = Vertex(glm::vec4(position.x, position.y, position.z, 1.0), 
							 glm::vec2(uv->x, uv->y), 
							 glm::vec3(normal.x,normal.y, normal.z),
							 glm::vec3(tangent.x, tangent.y, tangent.z),
							 glm::vec3(bitangent.x, bitangent.y, bitangent.z));
		vertices.push_back(vert);
		averageLocation = averageLocation + vert.position;
	}
	std::cout << "Finished Reading vertices" << std::endl;
	averageLocation.x = averageLocation.x / meshes[index].numVertices;
	averageLocation.y = averageLocation.y / meshes[index].numVertices;
	averageLocation.z = averageLocation.z / meshes[index].numVertices;
	averageLocation.w = averageLocation.w / meshes[index].numVertices;
	meshes[index].location = averageLocation;
	// std::cout << "Mesh location: " << averageLocation.x << " " << averageLocation.y << " " << averageLocation.z << std::endl;

	std::cout << "averageLocation: " << averageLocation.x << ", " << averageLocation.y << ", " << averageLocation.z << ", " << averageLocation.w << std::endl;

	for(int i = 0; i < meshes[index].numVertices; i++)
	{
		vertices[i].position -= averageLocation;
		vertices[i].position.w = averageLocation.w;
		//std::cout << vertices[i].position.x << ", " << vertices[i].position.y << ", " << vertices[i].position.z << std::endl;
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


void Scene::createSkinnedMesh(const aiMesh* m, std::string &name, const aiScene * scene)
{
	std::cout << "Reading Mesh" << std::endl;
	int index = skinnedMeshes.size();
	skinnedMeshes.push_back(SkinnedMesh());
	skinnedMeshes[index].name = name;
	std::cout << "Mesh Name: " << skinnedMeshes[index].name << " Original name: " << name << std::endl;
	skinnedMeshes[index].materialIndex = m->mMaterialIndex + materials.size();
	skinnedMeshes[index].hasTangents = m->HasTangentsAndBitangents();
	skinnedMeshes[index].numVertices = m->mNumVertices;
	skinnedMeshes[index].indexCount = m->mNumFaces * 3;

	aiMatrix4x4 transform = getTransformation(scene, skinnedMeshes[index].name);
	aiMatrix4x4 normalMatrix = transform;
	normalMatrix.Inverse().Transpose();

	aiVector3D loc;
	aiQuaternion rot;
	aiVector3D scale;
	transform.Decompose(scale,rot,loc);


	std::vector<SkinnedVertex> vertices;
	std::vector<unsigned int> indices;
	const aiVector3D zero(0.0f,0.0f,0.0f);
	glm::vec4 averageLocation = glm::vec4(0.0,0.0,0.0,0.0); // used to compute a transform component for the mesh

	for(unsigned int i = 0; i < m->mNumVertices; i++)
	{
		//std::cout << "Vertex: " << (transform * m->mVertices[i]).x << ", " << (transform * m->mVertices[i]).y << ", " << (transform * m->mVertices[i]).z <<std::endl;
		aiVector3D position = /*transform */ (m->mVertices[i]);
		const aiVector3D* uv = m->HasTextureCoords(0)? &(m->mTextureCoords[0][i]) : &zero;
		aiVector3D normal = /*normalMatrix */ (m->mNormals[i]);
		aiVector3D tangent = /*normalMatrix */ (m->mTangents[i]);
		aiVector3D bitangent = /*normalMatrix */ (m->mBitangents[i]);

		SkinnedVertex vert = SkinnedVertex(glm::vec4(position.x, position.y, position.z, 1.0), 
							 glm::vec2(uv->x, uv->y), 
							 glm::vec3(normal.x,normal.y, normal.z),
							 glm::vec3(tangent.x, tangent.y, tangent.z),
							 glm::vec3(bitangent.x, bitangent.y, bitangent.z));
		vertices.push_back(vert);
		averageLocation = averageLocation + vert.position;
	}
	averageLocation.x = averageLocation.x / skinnedMeshes[index].numVertices;
	averageLocation.y = averageLocation.y / skinnedMeshes[index].numVertices;
	averageLocation.z = averageLocation.z / skinnedMeshes[index].numVertices;
	averageLocation.w = averageLocation.w / skinnedMeshes[index].numVertices;
	skinnedMeshes[index].location.x = loc.x;
	skinnedMeshes[index].location.y = loc.y;
	skinnedMeshes[index].location.z = loc.z;
	skinnedMeshes[index].location.w = 1.0;
	skinnedMeshes[index].rotation.x = 0.0;//-3.14 / 2.0;//rot.x;
	skinnedMeshes[index].rotation.y = 0.0;//rot.y;
	skinnedMeshes[index].rotation.z = 0.0;//rot.z;
	skinnedMeshes[index].rotation.w = 0.0;//rot.w;
	skinnedMeshes[index].scale = 1.0;//(scale.x + scale.y + scale.z) / 3;



	std::cout << averageLocation.x << ", " << averageLocation.y << ", " << averageLocation.z << ", " << averageLocation.w << std::endl;

	// for(int i = 0; i < skinnedMeshes[index].numVertices; i++)
	// {
	// 	vertices[i].position -= averageLocation;
	// 	vertices[i].position.w = averageLocation.w;
	// }


	for(unsigned int i = 0; i < m->mNumFaces; i++)
	{
		const aiFace &face = m->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	//BoneHierarchy bones = BoneHierarchy();
	// load bones into VBO

	std::cout << "loading bones" << std::endl;
	for(int i = 0; i < m->mNumBones; i++)
	{
		std::string boneName = m->mBones[i]->mName.C_Str();
		std::cout << "Bone: " << boneName << std::endl;

		//add bone to bone Hierarchy
		glm::mat4 boneTransform = convertToGLMMatrix(m->mBones[i]->mOffsetMatrix);

		skinnedMeshes[index].bones.addBone(boneName, boneTransform);

		for(int j = 0; j < m->mBones[i]->mNumWeights; j++)
		{
			int id = m->mBones[i]->mWeights[j].mVertexId;
			float weight = m->mBones[i]->mWeights[j].mWeight;
			//std::cout << "ID: " << id << " Weight: " << weight << std::endl;
			if(id >= 0 && id < vertices.size())
			{
				//std::cout << "adding bone weight of " << weight << "to " << id << std::endl;
				if(vertices[id].boneWeights.x == 0)
				{
					vertices[id].boneWeights.x = weight;
					vertices[id].boneIDs.x = i;
					//std::cout << "trigger" << std::endl;
				}
				else if(vertices[id].boneWeights.y == 0)
				{
					vertices[id].boneWeights.y = weight;
					vertices[id].boneIDs.y = i;
				}
				else if(vertices[id].boneWeights.z == 0)
				{
					vertices[id].boneWeights.z = weight;
					vertices[id].boneIDs.z = i;
				}
				else if(vertices[id].boneWeights.w == 0)
				{
					vertices[id].boneWeights.w = weight;
					vertices[id].boneIDs.w = i;
				}
			}
		}
	}
	// for(int i = 0; i < vertices.size(); i++)
	// {
	// 	std::cout << "sum: " << vertices[i].boneWeights.x + vertices[i].boneWeights.y + vertices[i].boneWeights.z + vertices[i].boneWeights.w << std::endl;
	// }
	std::cout << "loaded bones" << std::endl;
	skinnedMeshes[index].bones.constructHierarchy(scene->mRootNode);
	std::cout << "built Hierarchy" << std::endl;
	glGenBuffers(1, &(skinnedMeshes[index].VBO));
  	glBindBuffer(GL_ARRAY_BUFFER, skinnedMeshes[index].VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkinnedVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &(skinnedMeshes[index].IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skinnedMeshes[index].IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    std::cout << "built VBOS" << std::endl;
    addAnimations(scene, &skinnedMeshes[index].bones);
    std::cout << "done" << std::endl;
}
	
void Scene::addAnimations(const aiScene * scene, BoneHierarchy * bones)
{	
	for(int animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
	{
		aiAnimation * currentAnimation = scene->mAnimations[animationIndex];
		std::cout << "Testing animation: " << currentAnimation->mName.C_Str() << std::endl;
		if(determineAnimationAssociation(currentAnimation, bones))
		{
			std::string animationName = currentAnimation->mName.C_Str();
			bones->addAnimation(animationName, currentAnimation->mDuration / currentAnimation->mTicksPerSecond);
			std::cout << "Adding animation: " << animationName << std::endl;

			for(int i = 0; i < currentAnimation->mNumChannels; i++)
			{
				std::string boneName = currentAnimation->mChannels[i]->mNodeName.C_Str();

				// add position key to animation
				for(int j = 0; j < currentAnimation->mChannels[i]->mNumPositionKeys; j++)
				{
					VecKeyframe key = VecKeyframe();
					key.time = currentAnimation->mChannels[i]->mPositionKeys[j].mTime / currentAnimation->mTicksPerSecond;
					aiVector3D * temp = &currentAnimation->mChannels[i]->mPositionKeys[j].mValue;
					key.value = glm::vec4(temp->x, temp->y, temp->z, 1.0);
					bones->addPositionKey(animationName, key, boneName);
				}

				// add rotation key to animation
				for(int j = 0; j < currentAnimation->mChannels[i]->mNumRotationKeys; j++)
				{
					QuatKeyframe key = QuatKeyframe();
					key.time = currentAnimation->mChannels[i]->mRotationKeys[j].mTime / currentAnimation->mTicksPerSecond;
					aiQuaternion * temp = &currentAnimation->mChannels[i]->mRotationKeys[j].mValue;
					key.value = glm::quat(temp->w, temp->x, temp->y, temp->z);
					bones->addRotationKey(animationName, key, boneName);
				}

				// add scale keys to animation
				for(int j = 0; j < currentAnimation->mChannels[i]->mNumScalingKeys; j++)
				{
					VecKeyframe key = VecKeyframe();
					key.time = currentAnimation->mChannels[i]->mScalingKeys[j].mTime / currentAnimation->mTicksPerSecond;
					aiVector3D * temp = &currentAnimation->mChannels[i]->mScalingKeys[j].mValue;
					key.value = glm::vec4(temp->x, temp->y, temp->z, 1.0);
					bones->addScaleKey(animationName, key, boneName);
				}
			}
		}
	}
}

bool Scene::determineAnimationAssociation(aiAnimation * animation, BoneHierarchy * bones)
{
	//return true;
	for(int i = 0; i < animation->mNumChannels; i++)
	{
		std::string name = animation->mChannels[i]->mNodeName.C_Str();
		if(bones->getBoneIndex(name) != -1)
			return true;
	}
	return false;
}

inline glm::mat4 Scene::convertToGLMMatrix(aiMatrix4x4 &original)
{
	glm::mat4 target;
	target = glm::transpose(glm::make_mat4(&original.a1));
	return target;
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

		if(currentMaterial->GetTextureCount(aiTextureType_REFLECTION) > 0)
		{
			//std::cout << "Texture Exists" << std::endl;
			aiString texture_path;
			if(currentMaterial->GetTexture(aiTextureType_REFLECTION, 0, &texture_path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string file_path = directory + "/" + texture_path.data;
				std::cout << "Reflective Texture: " << file_path << std::endl;
				materials[materialIndex].reflective = texManager.loadTexture(file_path, false);
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

		std::cout << "Diffuse: " << currentLight->mColorDiffuse.r << "," << currentLight->mColorDiffuse.g << "," << currentLight->mColorDiffuse.b << std::endl;
		std::cout << "Location: " << transformedLocation.x << "," << transformedLocation.y << "," << transformedLocation.z << std::endl;
		lights[index].location = glm::vec3(transformedLocation.x, transformedLocation.y, transformedLocation.z);
		lights[index].diffuse = glm::vec3(currentLight->mColorDiffuse.r, currentLight->mColorDiffuse.g, currentLight->mColorDiffuse.b );
		lights[index].specular = glm::vec3(currentLight->mColorSpecular.r, currentLight->mColorSpecular.g, currentLight->mColorSpecular.b);
		lights[index].linearAttenuation = isinf(currentLight->mAttenuationLinear)?1.0:currentLight->mAttenuationLinear;
		lights[index].quadraticAttenuation = isinf(currentLight->mAttenuationQuadratic)?1.0:currentLight->mAttenuationQuadratic;
		if(index == 0) lights[index].directional = true;
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

// //this will be deleted. this is only for testing purposes
// void Scene::render()
// {
// 	for(int i = 0; i < meshes.size(); i++)
// 	{
// 		glBindBuffer( GL_ARRAY_BUFFER, meshes[i].VBO );
// 		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, meshes[i].IBO );
// 		glVertexAttribPointer( vertexAttrib1, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0) );
// 		glVertexAttribPointer( rgbaAttrib1,   4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0 *sizeof(GLfloat)) );
// 		glVertexAttribPointer( normAttrib1,   3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(6 *sizeof(GLfloat)) );
// 		glVertexAttribPointer( tanAttrib1,    3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(9*sizeof(GLfloat)) );
// 		glVertexAttribPointer( bitanAttrib1,  3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(12*sizeof(GLfloat)) );
// 		glVertexAttribPointer( texAttrib1,    2, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)) );
// 		glDrawElements( GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT, 0 );
// 	}
// }


int Scene::populateByName(std::string &name, ECSEngine * ecs)
{
	glm::vec3 defaultOrientation = glm::vec3(0.0,0.0,0.0);

	for(int i = 0; i < skinnedMeshes.size(); i++)
	{
		if(skinnedMeshes[i].name.compare(name) == 0)
		{
			skinnedMeshes[i].count++;
			int entityID = ecs->addEntity();
			SkinnedRenderable r = SkinnedRenderable(skinnedMeshes[i].VBO, skinnedMeshes[i].IBO, skinnedMeshes[i].indexCount, 
													materials[skinnedMeshes[i].materialIndex].normals == 0? ShaderManager::skinnedBasic:ShaderManager::skinnedNormalMapped,
													materials[skinnedMeshes[i].materialIndex], entityID, skinnedMeshes[i].bones);
			//std::cout << "Before: " << skinnedMeshes[i].bones.print() << " After: " << r.bones.print() << std::endl;
			entityManager->addComponent(entityID, r);
			glm::vec3 rot = glm::vec3(skinnedMeshes[i].rotation);
			Transform t = Transform(skinnedMeshes[i].location, rot, skinnedMeshes[i].scale, entityID);
			entityManager->addComponent(entityID, t);

			associateLight(skinnedMeshes[i].name, t, entityID, ecs);
			return entityID;
		}
	}

	for(int i = 0; i < meshes.size(); i++)
	{
		if(meshes[i].name.compare(name) == 0)
		{
			meshes[i].count++;
			int entityID = ecs->addEntity();
			Renderable r = Renderable(meshes[i].VBO, meshes[i].IBO, meshes[i].indexCount, 
													materials[meshes[i].materialIndex].normals == 0? ShaderManager::deferredBasic:ShaderManager::deferredNormal,
													materials[meshes[i].materialIndex], entityID);
			entityManager->addComponent(entityID, r);

			Transform t = Transform(meshes[i].location, defaultOrientation, 1.0, entityID);
			entityManager->addComponent(entityID, t);

			associateLight(meshes[i].name, t, entityID, ecs);
			return entityID;
		}
	}

	for(int i = 0; i < lights.size(); i++)
	{
		if(lights[i].name.compare(name) == 0)
		{
			lights[i].count++;

			int entityID = ecs->addEntity();
			glm::vec4 lightPosition = glm::vec4(lights[i].location,1.0);
			Transform t = Transform(lightPosition, defaultOrientation, 1.0,entityID);
			Light l = Light(lights[i].directional, defaultOrientation, lights[i].diffuse, lights[i].specular, lights[i].linearAttenuation, lights[i].quadraticAttenuation, entityID);
			entityManager->addComponent(entityID, t);
			entityManager->addComponent(entityID, l);
			return entityID;
		}
	}

	return -1;
}

void Scene::populate(ECSEngine * ecs)
{
	glm::vec3 defaultOrientation = glm::vec3(0.0,0.0,0.0);

	for(int i = 0; i < skinnedMeshes.size(); i++)
	{
		if(skinnedMeshes[i].count == 0)
		{
			skinnedMeshes[i].count++;
			int entityID = ecs->addEntity();
			SkinnedRenderable r = SkinnedRenderable(skinnedMeshes[i].VBO, skinnedMeshes[i].IBO, skinnedMeshes[i].indexCount, 
													materials[skinnedMeshes[i].materialIndex].normals == 0? ShaderManager::skinnedBasic:ShaderManager::skinnedNormalMapped,
													materials[skinnedMeshes[i].materialIndex], entityID, skinnedMeshes[i].bones);
			std::cout << "Before: " << skinnedMeshes[i].bones.print() << " After: " << r.bones.print() << std::endl;
			entityManager->addComponent(entityID, r);
			glm::vec3 rot = glm::vec3(skinnedMeshes[i].rotation);
			Transform t = Transform(skinnedMeshes[i].location, rot, skinnedMeshes[i].scale, entityID);
			entityManager->addComponent(entityID, t);

			associateLight(skinnedMeshes[i].name, t, entityID, ecs);
		}
	}

	for(int i = 0; i < meshes.size(); i++)
	{
		if(meshes[i].count == 0)
		{
			meshes[i].count++;
			int entityID = ecs->addEntity();
			Renderable r = Renderable(meshes[i].VBO, meshes[i].IBO, meshes[i].indexCount, 
													materials[meshes[i].materialIndex].normals == 0? ShaderManager::deferredBasic:ShaderManager::deferredNormal,
													materials[meshes[i].materialIndex], entityID);
			entityManager->addComponent(entityID, r);

			Transform t = Transform(meshes[i].location, defaultOrientation, 1.0, entityID);
			entityManager->addComponent(entityID, t);

			associateLight(meshes[i].name, t, entityID, ecs);
		}
	}

	for(int i = 0; i < lights.size(); i++)
	{
		if(lights[i].count == 0)
		{
			lights[i].count++;

			int entityID = ecs->addEntity();
			glm::vec4 lightPosition = glm::vec4(lights[i].location,1.0);
			Transform t = Transform(lightPosition, defaultOrientation, 1.0,entityID);
			Light l = Light(lights[i].directional, defaultOrientation, lights[i].diffuse, lights[i].specular, lights[i].linearAttenuation, lights[i].quadraticAttenuation, entityID);
			entityManager->addComponent(entityID, t);
			entityManager->addComponent(entityID, l);
		}
	}


}

void Scene::associateLight(std::string &meshName, Transform &t, int entityID, ECSEngine * ecs)
{
	std::string lightName = meshName + "Light";
	for(int i = 0; i < lights.size(); i++)
	{

		if(lights[i].name.compare(lightName) == 0)
		{
			std::cout << "Light Match found" << std::endl;
			lights[i].count++;
			Light l = Light();
			l.ownerID = entityID;
			l.location = lights[i].location - glm::vec3(t.position) * t.scale;
			l.diffuse = lights[i].diffuse;
			l.specular = lights[i].specular;
			l.linearAttenuation = lights[i].linearAttenuation;
			l.quadraticAttenuation = lights[i].quadraticAttenuation;
			l.directional = lights[i].directional;

			entityManager->addComponent(entityID, l);
		}
	}
}



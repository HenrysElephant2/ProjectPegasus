#include "ShaderManager.h"

Shader::Shader(GLuint programID)
{
	program = programID;
	glUseProgram(program);
	// model, view, and projection matrix uniforms
	modelMatrixLoc = glGetUniformLocation(program, MODEL_VARIABLE_NAME);
	normalMatrixLoc = glGetUniformLocation(program, NORMAL_VARIABLE_NAME);
	viewMatrixLoc = glGetUniformLocation(program, VIEW_VARIABLE_NAME);
	projectionMatrixLoc = glGetUniformLocation(program, PROJECTION_VARIABLE_NAME);

	//Material uniforms
	ambientColorLoc = glGetUniformLocation(program, AMBIENT_VARIABLE_NAME);
	diffuseColorLoc = glGetUniformLocation(program, DIFFUSE_VARIABLE_NAME);
	specularColorLoc = glGetUniformLocation(program, SPECULAR_VARIABLE_NAME);
	transparentColorLoc = glGetUniformLocation(program, TRANSPARENT_VARIABLE_NAME);
	opacityLoc = glGetUniformLocation(program, OPACITY_VARIABLE_NAME);
	shininessLoc = glGetUniformLocation(program, SHININESS_VARIABLE_NAME);
	colorTextureLoc = glGetUniformLocation(program, COLOR_TEXTURE_VARIABLE_NAME);
	normalTextureLoc = glGetUniformLocation(program, NORMAL_TEXTURE_VARIABLE_NAME);
	emissiveTextureLoc = glGetUniformLocation(program, EMISSIVE_TEXTURE_VARIABLE_NAME);

	//load light Array locations
	std::string testName = LIGHT_ARRAY_VARIABLE_NAME + std::string("[0].") + LIGHT_POSITION_VARIABLE_NAME;
	if(glGetUniformLocation(program, testName.c_str()) != -1)
	{
		for(int i = 0; i < MAX_LIGHTS; i++)
		{
			std::string positionName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_POSITION_VARIABLE_NAME);
			lightPositionLoc.push_back(glGetUniformLocation(program, positionName.c_str()));

			std::string diffuseName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_DIFFUSE_VARIABLE_NAME);
			lightDiffuseLoc.push_back(glGetUniformLocation(program, diffuseName.c_str()));

			std::string specularName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_SPECULAR_VARIABLE_NAME);
			lightSpecularLoc.push_back(glGetUniformLocation(program, specularName.c_str()));

			std::string linearName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_LINEAR_VARIABLE_NAME);
			lightLinearAttenuationLoc.push_back(glGetUniformLocation(program, linearName.c_str()));

			std::string quadraticName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_QUADRATIC_VARIABLE_NAME);
			lightQuadraticAttenuationLoc.push_back(glGetUniformLocation(program, quadraticName.c_str()));

			std::string directionalName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_DIRECTIONAL_VARIABLE_NAME);
			lightDirectionalLoc.push_back(glGetUniformLocation(program, directionalName.c_str()));

			std::string directionName = std::string(LIGHT_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]." + std::string(LIGHT_DIRECTION_VARIABLE_NAME);
			lightDirectionLoc.push_back(glGetUniformLocation(program, directionName.c_str()));
		}
	}

	//check for and load bone transform array locations
	testName = std::string(BONE_ARRAY_VARIABLE_NAME) + "[0]";
	if(glGetUniformLocation(program,testName.c_str()) != -1)
	{
		for(int i = 0; i < MAX_BONES; i++)
		{
			std::string boneTransformName = std::string(BONE_ARRAY_VARIABLE_NAME) + "[" + std::to_string(i) + "]";
			boneTransformLocations.push_back(glGetUniformLocation(program,boneTransformName.c_str()));
		}
	}
	//lightArrayLoc = glGetUniformLocation(program, LIGHT_ARRAY_VARIABLE_NAME);

	lightCountLoc = glGetUniformLocation(program, LIGHT_COUNT_VARIABLE_NAME);
}

Shader::~Shader()
{	
	//std::cout << "Destructorcalled" << std::endl;
	glDeleteProgram(program);
}

void Shader::bindMaterial(Material * mat)
{
	if(ambientColorLoc != -1){glUniform3f(ambientColorLoc, mat->ambient.x, mat->ambient.y, mat->ambient.z);}
	if(diffuseColorLoc != -1){glUniform3f(diffuseColorLoc, mat->diffuse.x, mat->diffuse.y, mat->diffuse.z);}
	if(specularColorLoc != -1){glUniform3f(specularColorLoc, mat->specular.x, mat->specular.y, mat->specular.z);}
	if(transparentColorLoc != -1){glUniform3f(transparentColorLoc, mat->transparentColor.x, mat->transparentColor.y, mat->transparentColor.z);}
	if(opacityLoc != -1){glUniform1f(opacityLoc, mat->opacity);}
	if(shininessLoc != -1){glUniform1f(shininessLoc, mat->shininess);}
	if(colorTextureLoc != -1) {glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, mat->texture); glUniform1i(colorTextureLoc, 0);}
	if(normalTextureLoc != -1) {glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, mat->normals); glUniform1i(normalTextureLoc, 1);}
	if(emissiveTextureLoc != -1) {glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, mat->emissive); glUniform1i(emissiveTextureLoc, 2);}
}

void Shader::loadModelMatrix(glm::mat4 * modelMatrix)
{
	if(modelMatrixLoc != -1)
		glUniformMatrix4fv(modelMatrixLoc,1,GL_FALSE,glm::value_ptr(*modelMatrix));
}

void Shader::loadNormalMatrix(glm::mat3 *normalMatrix)
{
	if(normalMatrixLoc != -1)
		glUniformMatrix3fv(normalMatrixLoc,1,GL_FALSE,glm::value_ptr(*normalMatrix));
}

void Shader::loadViewMatrix(glm::mat4 * viewMatrix)
{
	if(viewMatrixLoc != -1)
		glUniformMatrix4fv(viewMatrixLoc,1,GL_FALSE,glm::value_ptr(*viewMatrix));
}

void Shader::loadProjectionMatrix(glm::mat4 * projectionMatrix)
{
	if(projectionMatrixLoc != -1)
		glUniformMatrix4fv(projectionMatrixLoc,1,GL_FALSE,glm::value_ptr(*projectionMatrix));
}

void Shader::loadLight(Light * light, Transform * t, int index)
{
	if(index < lightPositionLoc.size())
	{
		glUniform3f(lightPositionLoc[index], light->location.x + t->position.x*t->scale, light->location.y + t->position.y*t->scale, light->location.z + t->position.z*t->scale);
		glUniform3f(lightDiffuseLoc[index], light->diffuse.x, light->diffuse.y, light->diffuse.z);
		glUniform3f(lightSpecularLoc[index], light->specular.x, light->specular.y, light->specular.z);
		glUniform1f(lightLinearAttenuationLoc[index], light->linearAttenuation);
		glUniform1f(lightQuadraticAttenuationLoc[index], light->quadraticAttenuation);
		glUniform1f(lightDirectionalLoc[index], light->directional);
		glUniform3f(lightDirectionLoc[index], light->direction.x, light->direction.y, light->direction.z);
	}
}

void Shader::loadLightCount(int count)
{
	if(lightCountLoc != -1)
		glUniform1i(lightCountLoc, count);
}

void Shader::loadBones(BoneHierarchy * bones)
{
	for(int i = 0; i < bones->getBoneCount() && i < boneTransformLocations.size(); i++)
	{
		glUniformMatrix4fv(boneTransformLocations[i],1,GL_FALSE,glm::value_ptr(*bones->getCurrentBoneTransform(i)));
		//std::cout << glm::to_string(*bones->getCurrentBoneTransform(i)) << std::endl;
	}
}

void Shader::bind()
{
	GLuint err;
	if( (err = glGetError()) != GL_NO_ERROR ) std::cout << "Before bind of program " << program << " - " << err <<std::endl;
	glUseProgram(program);
	if( (err = glGetError()) != GL_NO_ERROR ) std::cout << "Error binding program " << program << " - " << err <<std::endl;
}

GLuint Shader::getProgramID()
{
	return program;
}


// shader manager code

ShaderManager::ShaderManager()
{
	current = 0;
}
ShaderManager::~ShaderManager()
{
	for(int i = 0; i < shaders.size(); i++)
	{
		delete shaders[i];
	}
}
int ShaderManager::createProgram(std::string & vertexShader, std::string & fragShader)
{
	//create program
	GLuint program = glCreateProgram();

	//read shaders
	GLuint vertex = loadShaderFromFile(GL_VERTEX_SHADER, vertexShader);
	GLuint fragment = loadShaderFromFile(GL_FRAGMENT_SHADER, fragShader);

	//attach shaders
	glAttachShader(program,vertex);
	glAttachShader(program,fragment);

	//link program
	glLinkProgram(program);

	// print program log in case of errors

	//add program to vector
	Shader* s = new Shader(program);
	int index = shaders.size();
	shaders.push_back(s);

	return index;
}
int ShaderManager::createProgram(std::string & vertexShader, std::string & geometryShader, std::string & fragShader)
{
	//create program
	GLuint program = glCreateProgram();

	//read shaders
	GLuint vertex = loadShaderFromFile(GL_VERTEX_SHADER, vertexShader);
	GLuint geometry = loadShaderFromFile(GL_GEOMETRY_SHADER, geometryShader);
	GLuint fragment = loadShaderFromFile(GL_FRAGMENT_SHADER, fragShader);

	//attach shaders
	glAttachShader(program,vertex);
	glAttachShader(program,geometry);
	glAttachShader(program,fragment);

	//link program
	glLinkProgram(program);

	// print program log in case of errors

	int logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1)
	{
		char * log = (char *)malloc(logLength+1);
		if(log == NULL) 
			{std::cout << "Unable to allocate memory to print log" << std::endl;}
		else {
			glGetProgramInfoLog(program,logLength,NULL,log);
			log[logLength] = 0;
			std::cout << "Log for Linking program: " << log << std::endl;
		}
	}
	glGetProgramiv(program, GL_LINK_STATUS, &logLength);
	if(logLength == 0) {std::cout << "Error Linking Program" << std::endl;}

	//add program to the vector
	Shader * s = new Shader(program);
	int index = shaders.size();
	shaders.push_back(s);

	return index;
}


GLuint ShaderManager::loadShaderFromFile(GLenum type, std::string & filename)
{
	GLuint shader = glCreateShader(type);
	FILE* file = std::fopen(filename.c_str(), "r");
	if(file == NULL)
	{
		std::cout << "Could not open file: " << filename << std::endl;
		return 0;
	}
	std::fseek(file,0,SEEK_END);
	int n = std::ftell(file);
	std::fseek(file,0,SEEK_SET);
	char * buffer = (char*)malloc(n+1);
	if(buffer == NULL)
	{
		std::cout << "Unable to allocate number of bytes needed for " << filename << std::endl;
		return 0; 
	}
	int read = std::fread(buffer, n, 1, file);
	if(read != 1)
	{
		std::cout << "Problem reading file: " << filename << std::endl;
	}
	buffer[n] = 0;
	std::fclose(file);

	glShaderSource(shader,1,(const char**)&buffer, NULL);

	free(buffer);
	glCompileShader(shader);

	int logLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1)
	{
		char * log = (char *)malloc(logLength+1);
		if(log == NULL) 
			{std::cout << "Unable to allocate memory to print log" << std::endl;}
		else {
			glGetShaderInfoLog(shader,logLength,NULL,log);
			log[logLength] = 0;
			std::cout << "Log for "<< filename <<":" << log << std::endl;
		}
	}
	glGetShaderiv(shader, GL_COMPILE_STATUS, &logLength);
	if(logLength == 0) {std::cout << "Error Compiling " << filename << std::endl;}

	return shader;
}

void ShaderManager::bindShader(int programIndex)
{
	if(programIndex < shaders.size())
	{
		current = programIndex;
		shaders[current]->bind();
	}

}

void ShaderManager::bindMaterial(Material * mat)
{
	if(current < shaders.size())
		shaders[current]->bindMaterial(mat);
}

void ShaderManager::loadModelMatrix(glm::mat4 *modelMatrix)
{
	if(current < shaders.size())
		shaders[current]->loadModelMatrix(modelMatrix);
}

void ShaderManager::loadNormalMatrix(glm::mat3 *normalMatrix)
{
	if(current < shaders.size())
		shaders[current]->loadNormalMatrix(normalMatrix);
}

void ShaderManager::loadViewMatrix(glm::mat4 *viewMatrix)
{
	if(current < shaders.size())
		shaders[current]->loadViewMatrix(viewMatrix);
}

void ShaderManager::loadProjectionMatrix(glm::mat4 *projectionMatrix)
{
	if(current < shaders.size())
		shaders[current]->loadProjectionMatrix(projectionMatrix);
}

void ShaderManager::loadLight(Light * light, Transform * t, int index)
{
	if(current < shaders.size())
		shaders[current]->loadLight(light,t,index);
}

void ShaderManager::loadLightCount(int count)
{
	if(current < shaders.size())
		shaders[current]->loadLightCount(count);
}

void ShaderManager::loadBones(BoneHierarchy * bones)
{
	if(current < shaders.size())
		shaders[current]->loadBones(bones);
}

GLuint ShaderManager::getProgramID()
{
	return shaders[current]->getProgramID();
}

ShaderManager * ShaderManager::singletonObject = NULL;

ShaderManager * ShaderManager::createShaderManager()
{
	if(!singletonObject)
		singletonObject = new ShaderManager();
	return singletonObject;
}

int ShaderManager::deferredBasic = -1;
int ShaderManager::deferredNormal = -1;
int ShaderManager::shadingPass = -1;
int ShaderManager::drawQuad = -1;
int ShaderManager::HDR = -1;
int ShaderManager::applyBloom = -1;
int ShaderManager::blur = -1;
int ShaderManager::skinnedBasic = -1;
int ShaderManager::skinnedNormalMapped = -1;
int ShaderManager::displayParticles = -1;
int ShaderManager::volumetricLightScattering = -1;
int ShaderManager::skybox = -1;

void ShaderManager::loadShaders(ShaderManager* sm)
{
	std::string defaultVert = "Shaders/deferredBasic.vert";
	std::string defaultFrag = "Shaders/deferredBasic.frag";
	deferredBasic = sm->createProgram(defaultVert,defaultFrag);

	std::string normalVert = "Shaders/deferredNormalMapped.vert";
	std::string normalFrag = "Shaders/deferredNormalMapped.frag";
	deferredNormal = sm->createProgram(normalVert,normalFrag);

	std::string shadingVert = "Shaders/PhongShading.vert";
	std::string shadingFrag = "Shaders/PhongShading.frag";
	shadingPass = sm->createProgram(shadingVert,shadingFrag);

	std::string quadVert = "Shaders/drawQuad.vert";
	std::string quadFrag = "Shaders/drawQuad.frag";
	drawQuad = sm->createProgram(quadVert,quadFrag);

	std::string hdrVert = "Shaders/HDR.vert";
	std::string hdrFrag = "Shaders/HDR.frag";
	HDR = sm->createProgram(hdrVert,hdrFrag);

	std::string blurVert = "Shaders/blur.vert";
	std::string blurFrag = "Shaders/blur.frag";
	blur = sm->createProgram(blurVert,blurFrag);

	std::string applyBloomVert = "Shaders/applyBloom.vert";
	std::string applyBloomFrag = "Shaders/applyBloom.frag";
	applyBloom = sm->createProgram(applyBloomVert,applyBloomFrag);

	std::string skinnedBasicVert = "Shaders/SkinnedBasic.vert";
	std::string skinnedBasicFrag = "Shaders/deferredBasic.frag";
	skinnedBasic = sm->createProgram(skinnedBasicVert, skinnedBasicFrag);

	std::string displayParticlesVert = "Shaders/sparkParticles.vert";
	std::string displayParticlesFrag = "Shaders/sparkParticles.frag";
	displayParticles = sm->createProgram(displayParticlesVert, displayParticlesFrag);

	std::string vlsVert = "Shaders/drawQuad.vert";
	std::string vlsFrag = "Shaders/VolumetricLightScattering.frag";
	volumetricLightScattering = sm->createProgram(vlsVert, vlsFrag);

	std::string skyboxVert = "Shaders/deferredBasic.vert";
	std::string skyboxFrag = "Shaders/Skybox.frag";
	skybox = sm->createProgram(skyboxVert, skyboxFrag);

	if( glGetError() != GL_NO_ERROR ) std::cout << "Error creating a shader" << std::endl;
}





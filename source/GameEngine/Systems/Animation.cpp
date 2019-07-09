#include "Animation.h"


glm::mat4 Animation::getBoneTransform(float time, int bone)
{
	//std::cout << "bone " << bone << std::endl;
	//std::cout << "help2" << boneKeyframes.size() << bone << std::endl;
	//TODO
	glm::mat4 transform = glm::mat4(1.0);

	//find best two scale keys
	glm::mat4 s = glm::scale(glm::mat4(1.0), glm::vec3(getVecKeyframeValue(&(boneKeyframes[bone].scaleKeys), time, false)));
	


	//find best two rotation keys
	glm::quat rotate = getQuatKeyframeValue(&boneKeyframes[bone].rotationKeys, time);
	//rotate = glm::quat(1.0,0.0,0.0,0.0);
	glm::mat4 r = glm::mat4_cast(rotate);

	
	//find best two position keys
	glm::mat4 t = glm::translate(glm::mat4(1.0), glm::vec3(getVecKeyframeValue(&boneKeyframes[bone].positionKeys, time, true)));

	// create matrix
	//std::cout << "returning matrix" << std::endl;
	//return transform*r;
	return t*r * s;
	//return transform;
}

glm::vec4 Animation::getVecKeyframeValue(std::vector<VecKeyframe> * keys, float time, bool zeroIfNone)
{
	//return glm::vec4(1.0);
	int size = keys->size();
	if(size == 0)
		{return zeroIfNone? glm::vec4(0.0,0.0,0.0,1.0):glm::vec4(1.0,1.0,1.0,1.0);}
	else if(size == 1)
		{return keys->at(0).value;}
	int beforeIndex = 0;
	int i = 0;
	while(i < size)
	{
		if(keys->at(i).time >= time)
			break;
		if(i != size - 1)
		beforeIndex = i;
		i++;
	}
	//return glm::vec4(1.0);
	if(i == size)
		return keys->at(i-1).value;
	double totalTime = keys->at(i).time - keys->at(beforeIndex).time;
	float multiplier = (time - keys->at(beforeIndex).time) / totalTime;
	//std::cout << beforeIndex << " < " << i << " size: " << keys->size() << std::endl;
	glm::vec4 result = glm::mix(keys->at(beforeIndex).value, keys->at(i).value, multiplier);//(float)(1.0 - multiplier) * keys->at(beforeIndex).value + multiplier * keys->at(i).value;
	//std::cout << result.x << ", " << result.y << ", " << result.z <<std::endl;
	return result;
}

glm::quat Animation::getQuatKeyframeValue(std::vector<QuatKeyframe> * keys, float time)
{
	//return glm::vec4(1.0);
	int size = keys->size();
	if(size == 0)
		{return glm::quat(0.0,0.0,0.0,1.0);}
	else if(size == 1)
		{return keys->at(0).value;}
	int beforeIndex = 0;
	int i = 0;
	while(i < size)
	{
		if(keys->at(i).time >= time)
			break;
		if(i != size - 1)
		beforeIndex = i;
		i++;
	}
	//return glm::vec4(1.0);
	if(i == size)
		return keys->at(i-1).value;
	double totalTime = keys->at(i).time - keys->at(beforeIndex).time;
	float multiplier = (time - keys->at(beforeIndex).time) / totalTime;
	//std::cout << beforeIndex << " < " << i << " size: " << keys->size() << std::endl;
	glm::quat result = glm::slerp(keys->at(beforeIndex).value, keys->at(i).value, multiplier);
	//std::cout << result.x << ", " << result.y << ", " << result.z <<std::endl;
	return result;
}

void Animation::addPositionKey(VecKeyframe &key, int bone)
{
	//std::cout << "Adding position key at index " << bone << " size: " << boneKeyframes.size() << std::endl;
	boneKeyframes[bone].positionKeys.push_back(key);
	if(key.time > animationLength)
		animationLength = key.time;
}
void Animation::addRotationKey(QuatKeyframe &key, int bone)
{
	//std::cout << "Adding rotation key at index " << bone << " size: " << boneKeyframes.size() << std::endl;
	boneKeyframes[bone].rotationKeys.push_back(key);
	if(key.time > animationLength)
		animationLength = key.time;
}
void Animation::addScaleKey(VecKeyframe &key, int bone)
{
	//std::cout << "Adding scale key at index " << bone << " size: " << boneKeyframes.size() << std::endl;
	boneKeyframes[bone].scaleKeys.push_back(key);
	if(key.time > animationLength)
		animationLength = key.time;
}


void BoneHierarchy::addBone(std::string &boneName, glm::mat4 &offsetMatrix)
{
	Bone newBone = Bone();
	newBone.boneName = boneName;
	newBone.offsetMatrix = offsetMatrix;
	bones.push_back(newBone);
}

int BoneHierarchy::getBoneIndex(std::string &name)
{
	for(int i = 0; i < bones.size(); i++)
	{
		if(bones[i].boneName.compare(name) == 0)
			return i;
	}
	return -1;
}

void BoneHierarchy::constructHierarchy(aiNode * rootNode)
{
	
	std::cout << "num bones:" << bones.size() << std::endl;
	aiNode * hierarchyRoot = findHierarchyRootNode(rootNode);
	std::string rootName = hierarchyRoot->mName.C_Str();
	root = getBoneIndex(rootName);
	aiMatrix4x4 temp = getTransformation(rootNode, rootName);
	glm::mat4 tempglm = convertToGLMMatrix(temp);
	constructHierarchyHelper(hierarchyRoot, root, glm::mat4(1.0));//convertToGLMMatrix(hierarchyRoot->mTransformation));
	std::cout << "num children:" << bones[root].children.size() << std::endl;
	std::cout << "num bones:" << bones.size() << std::endl;

	//bones[root].transform = glm::mat4(1.0);//convertToGLMMatrix(hierarchyRoot->mTransformation);
	bones[root].transform = convertToGLMMatrix(hierarchyRoot->mTransformation);
	bones[root].inverseBind = glm::inverse(bones[root].transform);

	aiMatrix4x4 global = rootNode->mTransformation;
	global.Inverse();
	globalInverse = (convertToGLMMatrix(global));
	global = getTransformation(rootNode, rootName);
	baseOffset = convertToGLMMatrix(global);
}

inline glm::mat4 BoneHierarchy::convertToGLMMatrix(aiMatrix4x4 &original)
{
	glm::mat4 target;
	target = glm::transpose(glm::make_mat4(&original.a1));
	return target;
}

aiMatrix4x4 BoneHierarchy::getTransformation(aiNode * root, std::string &name)
{
	return getTransformationHelper(root->FindNode(name.c_str()));
}

aiMatrix4x4 BoneHierarchy::getTransformationHelper(aiNode * node)
{
	if(node->mParent == NULL)
		return node->mTransformation;
	return getTransformationHelper(node->mParent) * node->mTransformation;
}

aiNode* BoneHierarchy::findHierarchyRootNode(aiNode * rootNode)
{
	std::cout << "root: " << rootNode->FindNode(bones[0].boneName.c_str())->mName.C_Str();
	return rootNode->FindNode(bones[0].boneName.c_str());
	// std::string name = std::string(rootNode->mName.C_Str());
	// if(getBoneIndex(name) != -1)
	// 	return rootNode;
	// else {
	// 	for(int i = 0; i < rootNode->mNumChildren; i++)
	// 	{
	// 		aiNode * ret = findHierarchyRootNode(rootNode->mChildren[i]);
	// 		if(ret != NULL)
	// 		{
	// 			return ret;
	// 		}
	// 	}
	// 	return NULL;
	// }

}
void BoneHierarchy::constructHierarchyHelper(aiNode * boneNode, int boneIndex, glm::mat4 offset)
{
	for(int i = 0; i < boneNode->mNumChildren; i++)
	{
		std::string childName = boneNode->mChildren[i]->mName.C_Str();
		int childBone = getBoneIndex(childName);
		//std::cout << "Child index: " << childBone << " Name: " << childName << std::endl;
		//constructHierarchyHelper(boneNode->mChildren[i], boneIndex);
		if(childBone != -1)
		{
			bones[boneIndex].children.push_back(childBone);
			//bones[childBone].offsetMatrix = bones[childBone].offsetMatrix;
			bones[childBone].transform = convertToGLMMatrix(boneNode->mChildren[i]->mTransformation);

			glm::mat4 bindTransform = offset * bones[childBone].transform;
			bones[childBone].inverseBind = glm::inverse(bindTransform);
			constructHierarchyHelper(boneNode->mChildren[i], childBone, bindTransform);
		}
		else {
			constructHierarchyHelper(boneNode->mChildren[i], boneIndex, offset * convertToGLMMatrix(boneNode->mTransformation));
		}
	}
}

// add an animation to the bone hierarchy
void BoneHierarchy::addAnimation(std::string &animationName, double length)
{
	animations.push_back(Animation(animationName,length,bones.size()));
}
//add keyframes to an animation
void BoneHierarchy::addPositionKey(std::string &animationName, VecKeyframe &key, std::string &boneName)
{
	int animationIndex = -1;
	for(int i = 0; i < animations.size(); i++)
	{
		if(animations[i].animationName.compare(animationName) == 0)
		{
			animationIndex = i;
			break;
		}
	}
	if(animationIndex > -1)
	{
		int boneIndex = getBoneIndex(boneName);
		if(boneIndex != -1)
			animations[animationIndex].addPositionKey(key, boneIndex);
	}
}
void BoneHierarchy::addRotationKey(std::string &animationName, QuatKeyframe &key, std::string &boneName)
{
	int animationIndex = -1;
	for(int i = 0; i < animations.size(); i++)
	{
		if(animations[i].animationName.compare(animationName) == 0)
		{
			animationIndex = i;
			break;
		}
	}
	if(animationIndex > -1)
	{
		int boneIndex = getBoneIndex(boneName);
		if(boneIndex != -1)
			animations[animationIndex].addRotationKey(key, boneIndex);
	}
}
void BoneHierarchy::addScaleKey(std::string &animationName, VecKeyframe &key, std::string &boneName)
{
	int animationIndex = -1;
	for(int i = 0; i < animations.size(); i++)
	{
		if(animations[i].animationName.compare(animationName) == 0)
		{
			animationIndex = i;
			break;
		}
	}
	if(animationIndex > -1)
	{
		int boneIndex = getBoneIndex(boneName);
		if(boneIndex != -1)
			animations[animationIndex].addScaleKey(key, boneIndex);
	}
}

//functions for when the ECS is running
void BoneHierarchy::updateAnimation(double dt)
{
	//std::cout << "Animations: " << animations.size()  << " " << dt << std::endl;
	//currentAnimation = animations.size() - 1;
	currentTime += dt;
	if(currentAnimation < animations.size() && currentAnimation >= 0)
	{
		//std::cout << "Length" << animations[currentAnimation].animationLength  << " " << currentTime << std::endl;
		while(currentTime > animations[currentAnimation].animationLength)
			currentTime -= animations[currentAnimation].animationLength;
		glm::mat4 temp = glm::mat4(1.0);
		setCurrentTransform(root, glm::rotate(glm::mat4(1.0), -3.14f, glm::vec3(1.0,0.0,0.0)) /* baseOffset*/);//glm::mat4(1.0));
	}
	//std::cout << root << std::endl;
	
}

void BoneHierarchy::setCurrentTransform(int bone, glm::mat4 parentTransform)
{
	//std::cout << animations.size() << " - " << currentAnimation << std::endl;
	glm::mat4 boneTransform = parentTransform /* bones[bone].transform */* animations[currentAnimation].getBoneTransform((float)currentTime, bone);//* bones[bone].offsetMatrix;
	bones[bone].currentTransformation = globalInverse * boneTransform * (bones[bone].offsetMatrix);
	//std::cout << bones.size() << " " << bones[bone].children.size() << " " << animations.size() << std::endl;
	for(int i = 0; i < bones[bone].children.size(); i++)
	{
		
		setCurrentTransform(bones[bone].children[i], boneTransform);
	}
}
//change the current animation
void BoneHierarchy::changeAnimation(std::string & animationName)
{

}

int BoneHierarchy::getBoneCount()
{
	return bones.size();
}

glm::mat4 * BoneHierarchy::getCurrentBoneTransform(int index)
{
	if(index >= 0 && index < bones.size())
		return &bones[index].currentTransformation;
	return NULL;
}





#include "Animation.h"


glm::mat4 Animation::getBoneTransform(double time, int bone)
{
	//TODO
	return glm::mat4();
}

void Animation::addPositionKey(Keyframe &key, int bone)
{
	//std::cout << "Adding position key at index " << bone << " size: " << boneKeyframes.size() << std::endl;
	boneKeyframes[bone].positionKeys.push_back(key);
}
void Animation::addRotationKey(Keyframe &key, int bone)
{
	boneKeyframes[bone].rotationKeys.push_back(key);
}
void Animation::addScaleKey(Keyframe &key, int bone)
{
	boneKeyframes[bone].scaleKeys.push_back(key);
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
	constructHierarchyHelper(hierarchyRoot, root);
	std::cout << "num children:" << bones[root].children.size() << std::endl;
	std::cout << "num bones:" << bones.size() << std::endl;
}
aiNode* BoneHierarchy::findHierarchyRootNode(aiNode * rootNode)
{
	std::string name = std::string(rootNode->mName.C_Str());
	if(getBoneIndex(name) != -1)
		return rootNode;
	else {
		for(int i = 0; i < rootNode->mNumChildren; i++)
		{
			aiNode * ret = findHierarchyRootNode(rootNode->mChildren[i]);
			if(ret != NULL)
			{
				return ret;
			}
		}
		return NULL;
	}

}
void BoneHierarchy::constructHierarchyHelper(aiNode * boneNode, int boneIndex)
{
	for(int i = 0; i < boneNode->mNumChildren; i++)
	{
		std::string childName = boneNode->mChildren[i]->mName.C_Str();
		int childBone = getBoneIndex(childName);
		std::cout << "Child index: " << childBone << " Name: " << childName << std::endl;
		if(childBone != -1)
		{
			bones[boneIndex].children.push_back(childBone);
			constructHierarchyHelper(boneNode->mChildren[i], childBone);
		}
	}
}

// add an animation to the bone hierarchy
void BoneHierarchy::addAnimation(std::string &animationName, double length)
{
	animations.push_back(Animation(animationName,length,bones.size()));
}
//add keyframes to an animation
void BoneHierarchy::addPositionKey(std::string &animationName, Keyframe &key, std::string &boneName)
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
		int boneIndex;
		if((boneIndex = getBoneIndex(boneName)) != -1)
			animations[animationIndex].addPositionKey(key, boneIndex);
	}
}
void BoneHierarchy::addRotationKey(std::string &animationName, Keyframe &key, std::string &boneName)
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
		int boneIndex;
		if((boneIndex = getBoneIndex(boneName)) != -1)
			animations[animationIndex].addRotationKey(key, boneIndex);
	}
}
void BoneHierarchy::addScaleKey(std::string &animationName, Keyframe &key, std::string &boneName)
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
		int boneIndex;
		if((boneIndex = getBoneIndex(boneName)) != -1)
			animations[animationIndex].addScaleKey(key, boneIndex);
	}
}

//functions for when the ECS is running
void BoneHierarchy::updateAnimation(double dt)
{
	currentTime += dt;
	if(currentAnimation < animations.size() && currentAnimation >= 0)
		if(currentTime > animations[currentAnimation].animationLength)
			currentTime -= animations[currentAnimation].animationLength;
	//std::cout << root << std::endl;
	setCurrentTransform(root, glm::mat4());
}

void BoneHierarchy::setCurrentTransform(int bone, glm::mat4 parentTransform)
{
	//std::cout << bones.size() << std::endl;
	glm::mat4 boneTransform = parentTransform * animations[currentAnimation].getBoneTransform(currentTime, bone);
	bones[bone].currentTransformation = boneTransform;
	for(int i = 0; i < bones[bone].children.size(); i++)
	{
		//std::cout << bones.size() << " " << bones[bone].children.size() << " " << /*bones[bone].children[i]*/i << std::endl;
		setCurrentTransform(bones[bone].children[i], boneTransform);
	}
}
//change the current animation
void BoneHierarchy::changeAnimation(std::string & animationName)
{

}




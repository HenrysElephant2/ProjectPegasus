#ifndef ANIMATION_H
#define ANIMATION_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <assimp/scene.h>

struct Keyframe {
	double time;
	// glm::vec4 value;
};

struct VecKeyframe : Keyframe {
	glm::vec4 value;
};

struct QuatKeyframe : Keyframe {
	glm::quat value;
};

class Animation {
private:
	
	struct BoneAnimation{
		std::vector<VecKeyframe> positionKeys;
		std::vector<QuatKeyframe> rotationKeys;
		std::vector<VecKeyframe> scaleKeys;
	};

	
	std::vector<BoneAnimation> boneKeyframes;
	
	glm::vec4 getVecKeyframeValue(std::vector<VecKeyframe> *keys, float time, bool zeroIfNone);
	glm::quat getQuatKeyframeValue(std::vector<QuatKeyframe> *keys, float time);

public:
	double animationLength;
	std::string animationName;
	Animation(std::string & name, double length, int numBones){
		animationName = name;
		animationLength = length;
		for(int i = 0; i < numBones; i++)
			boneKeyframes.push_back(BoneAnimation());
		//boneKeyframes.reserve(numBones);
	}

	glm::mat4 getBoneTransform(float time, int bone);

	void addPositionKey(VecKeyframe &key, int bone);
	void addRotationKey(QuatKeyframe &key, int bone);
	void addScaleKey(VecKeyframe &key, int bone);

};



class BoneHierarchy {
private:
	struct Bone {
		std::string boneName;

		//int parent;
		std::vector<int> children;
		glm::mat4 currentTransformation = glm::mat4(1.0);
		glm::mat4 offsetMatrix = glm::mat4(1.0);
		glm::mat4 inverseBind = glm::mat4(1.0);
		glm::mat4 transform = glm::mat4(1.0);
	};

	std::vector<Bone> bones;
	int root;
	std::vector<Animation> animations;
	int currentAnimation = 0;
	double currentTime = 0;

	glm::mat4 globalInverse;
	glm::mat4 baseOffset;
	// Uint64 previousTime;
	// Uint64 frequency;

	inline glm::mat4 convertToGLMMatrix(aiMatrix4x4 &original);

	void constructHierarchyHelper(aiNode * boneNode, int boneIndex, glm::mat4 offset);
	void setCurrentTransform(int bone, glm::mat4 parentTransform);
	aiNode* findHierarchyRootNode(aiNode * rootNode);
	aiMatrix4x4 getTransformation(aiNode * root, std::string &name);
	aiMatrix4x4 getTransformationHelper(aiNode * node);
public:

	BoneHierarchy()
	{
		//frequency = SDL_GetPerformanceFrequency();
	}
	BoneHierarchy( const BoneHierarchy &obj)
	{
		bones = obj.bones;
		root = obj.root;
		animations = obj.animations;
		currentAnimation = obj.currentAnimation;
		currentTime = obj.currentTime;
	}
	//add a bone to the heirarchy
	void addBone(std::string &boneName, glm::mat4 &offsetMatrix);

	int getBoneIndex(std::string &name);

	void constructHierarchy(aiNode * rootNode);

	// add an animation to the bone heirarchy
	void addAnimation(std::string &animationName, double length);
	//add keyframes to an animation
	void addPositionKey(std::string &animationName, VecKeyframe &key, std::string &boneName);
	void addRotationKey(std::string &animationName, QuatKeyframe &key, std::string &boneName);
	void addScaleKey(std::string &animationName, VecKeyframe &key, std::string &boneName);

	//functions for when the ECS is running
	void updateAnimation(double dt);

	void changeAnimation(std::string & animationName);

	int getBoneCount();
	glm::mat4 * getCurrentBoneTransform(int index);

	int print() 
	{
		return bones[0].children.size();
	}

};



#endif
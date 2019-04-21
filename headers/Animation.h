#ifndef ANIMATION_H
#define ANIMATION_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <assimp/scene.h>

struct Keyframe {
		double time;
		glm::vec4 value;
	};

class Animation {
private:
	
	struct BoneAnimation{
		std::vector<Keyframe> positionKeys;
		std::vector<Keyframe> rotationKeys;
		std::vector<Keyframe> scaleKeys;
	};

	
	std::vector<BoneAnimation> boneKeyframes;
	
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

	glm::mat4 getBoneTransform(double time, int bone);

	void addPositionKey(Keyframe &key, int bone);
	void addRotationKey(Keyframe &key, int bone);
	void addScaleKey(Keyframe &key, int bone);

};



class BoneHierarchy {
private:
	struct Bone {
		std::string boneName;

		//int parent;
		std::vector<int> children;
		glm::mat4 currentTransformation;
		glm::mat4 offsetMatrix;
	};

	std::vector<Bone> bones;
	int root;
	std::vector<Animation> animations;
	int currentAnimation;
	double currentTime;
	// Uint64 previousTime;
	// Uint64 frequency;

	void constructHierarchyHelper(aiNode * boneNode, int boneIndex);
	void setCurrentTransform(int bone, glm::mat4 parentTransform);
	aiNode* findHierarchyRootNode(aiNode * rootNode);
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
	void addPositionKey(std::string &animationName, Keyframe &key, std::string &boneName);
	void addRotationKey(std::string &animationName, Keyframe &key, std::string &boneName);
	void addScaleKey(std::string &animationName, Keyframe &key, std::string &boneName);

	//functions for when the ECS is running
	void updateAnimation(double dt);

	void changeAnimation(std::string & animationName);

	int print() 
	{
		return bones[0].children.size();
	}

};



#endif
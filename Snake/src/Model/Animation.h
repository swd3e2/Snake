#pragma once

#include "ImportData.h"

class Animation
{
public:
	struct AnimationIndex {
		double rotationIndex = 0;
		double translationIndex = 0;
		double scaleIndex = 0;
	};

	struct AnimationNodeData {
		std::vector<std::pair<double, glm::quat>> rotations;
		std::vector<std::pair<double, glm::vec3>> positions;
		std::vector<std::pair<double, glm::vec3>> scale;
	};

	struct AnimationData
	{
		std::map<int, std::shared_ptr<AnimationNodeData>> data;
		std::string name;
		double duration = 0.0;
	};
public:
	int rootJoint;
	float blendingFactor = 1.0f;
	double currentAnimationTime = 0.0;
	int currentAnimation = 0;
	std::vector<AnimationIndex> animationIndices;
	std::vector<glm::mat4> inverseBindMatrices;
	std::vector<int> joints;
	std::vector<std::shared_ptr<AnimationData>> animations;
	std::vector<glm::mat4> nodeInfo;
	double frameRate = 1.0 / 60.0;
	double currentFrame = 0.0;
	bool shouldPlay = true;
	bool stoped = false;
public:
	Animation(const std::shared_ptr<Import::Model>& mesh)
	{
		if (mesh->rootJoint == -1) {
			return;
		}

		rootJoint = mesh->rootJoint;
		inverseBindMatrices = mesh->inverseBindMatrices;

		for (auto& importAnimation : mesh->animations) {
			std::shared_ptr<AnimationData> animation = std::make_shared<AnimationData>();
			animation->duration = importAnimation->duration;
			animation->name = importAnimation->name;

			for (auto& animationData : importAnimation->data) {
				std::shared_ptr<AnimationNodeData> nodeData = std::make_shared<AnimationNodeData>();

				for (auto& it : animationData.second->positions) {
					nodeData->positions.push_back(std::make_pair(it.first, it.second));
				}
				for (auto& it : animationData.second->rotations) {
					nodeData->rotations.push_back(std::make_pair(it.first, it.second));
				}
				for (auto& it : animationData.second->scale) {
					nodeData->scale.push_back(std::make_pair(it.first, it.second));
				}

				animation->data[animationData.first] = nodeData;
			}
			animations.push_back(animation);
		}

		nodeInfo.resize(mesh->joints.size());
		animationIndices.resize(mesh->joints.size());
	}

	void pause()
	{
		shouldPlay = false;
	}

	void restart()
	{
		stop();
		play();
	}

	void play()
	{
		shouldPlay = true;
		stoped = false;
	}

	void stop()
	{
		currentFrame = 0.0;
		currentAnimationTime = 0.0;
		resetAllNodeTRSIndices();
		shouldPlay = false;
		stoped = true;
	}

	void resetAllNodeTRSIndices()
	{
		for (auto& it : animationIndices) {
			it.rotationIndex = it.scaleIndex = it.translationIndex = 0;
		}
	}
};
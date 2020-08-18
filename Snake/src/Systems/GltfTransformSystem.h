#pragma once

#include "ISystem.h"
#include <memory>

class GltfTransformSystem : public ISystem 
{
public:
    GltfTransformSystem(SceneManager* sceneManager) :
        ISystem(sceneManager)
    {}

	virtual void update(double dt) override 
	{
		entt::registry* registry = sceneManager->getCurrentScene()->getRegistry();

		/*auto group = registry->group<RenderComponent>();

		for (auto entity : group) {
			auto& render = group.get<RenderComponent>(entity);
             updateTransform(render.model->rootNode, render.model, glm::mat4(1.0f));
		}*/
	}
private:
    void updateTransform(const int nodeId, const std::shared_ptr<Model>& renderable, const glm::mat4& parentTransform)
    {
        const std::vector<std::shared_ptr<Node>>& nodes = renderable->nodes;
        if (nodes.size() <= nodeId) return;

        const std::shared_ptr<Node>& node = nodes[nodeId];

        if (node->transform.hasChanged == true) {
            glm::mat4 localTransform = glm::mat4(1.0f);
            localTransform = glm::translate(localTransform, node->transform.translation);
            localTransform = localTransform * glm::toMat4(node->transform.rotation);
            localTransform = glm::scale(localTransform, node->transform.scale);

            node->transform.worldTransform = parentTransform * localTransform;
            node->transform.normalTransform = glm::inverse(node->transform.worldTransform);
            node->transform.hasChanged = false;
        }

        for (auto& it : node->childs) {
            updateTransform(it, renderable, node->transform.worldTransform);
        }
    }
};
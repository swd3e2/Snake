#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "Components.h"
#include "FileSystem/File.h"
#include <iostream>
#include "Import/ModelLoader.h"
#include "Graphics/Renderer/CommonTypes.h"

class SceneLoader 
{
public:
    void loadFromFile(const std::string& filename, entt::registry* registry) {
        using nlohmann::json;

        File file(filename);
        if (!file.exists()) {
            std::cout << "No such file: " << filename << std::endl;
            return;
        }

        nlohmann::basic_json<> projectData = json::parse(file.getConent());

        for (nlohmann::basic_json<>& it : projectData["Entities"]) {
            entt::entity entity = registry->create();

            if (it.find("Transform") != it.end()) {
                TransformComponent& transform = registry->emplace<TransformComponent>(entity,
                    glm::vec3(
                        it["Transform"]["translation"][0].get<float>(),
                        it["Transform"]["translation"][1].get<float>(),
                        it["Transform"]["translation"][2].get<float>()
                    ),
                    glm::vec3(
                        it["Transform"]["rotation"][0].get<float>(),
                        it["Transform"]["rotation"][1].get<float>(),
                        it["Transform"]["rotation"][2].get<float>()
                    ),
                    glm::vec3(
                        it["Transform"]["scale"][0].get<float>(),
                        it["Transform"]["scale"][1].get<float>(),
                        it["Transform"]["scale"][2].get<float>()
                    )
                    );
                transform.matrix = glm::translate(glm::mat4(1.0f), transform.translation);
                transform.matrix = transform.matrix * glm::eulerAngleXYZ(transform.rotation.x * DEG2RAD, transform.rotation.y * DEG2RAD, transform.rotation.z * DEG2RAD);
                transform.matrix = glm::scale(transform.matrix, transform.scale);
            }

            if (it.find("Render") != it.end()) {
                registry->emplace<RenderComponent>(entity, ModelLoader::instance()->loadFromFile(it["Render"]["filename"].get<std::string>()));
            }

            if (it.find("Script") != it.end()) {
                registry->emplace<ScriptComponent>(entity, it["Script"]["filename"].get<std::string>());
            }
        }
    }
};
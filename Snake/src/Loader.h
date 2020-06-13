#include <string>
#include <nlohmann/json.hpp>
#include "Components.h"
#include "Model/Import/GltfImporter.h"
#include "FileSystem/File.h"
#include <iostream>

class Loader {
private:
public:
    void loadFromFile(const std::string& filename, entt::registry* registry) {
        using nlohmann::json;

        File file(filename);
        auto projectData = json::parse(file.getConent());

        for (auto& it : projectData["Entities"]) {
            entt::entity entity = registry->create();

            if (it.find("Transform") != it.end()) {
                Transform& transform = registry->emplace<Transform>(entity,
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
                transform.matrix = glm::mat4(1.0f);
                transform.matrix = glm::translate(transform.matrix, transform.translation);
                transform.matrix = transform.matrix * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
                transform.matrix = glm::scale(transform.matrix, transform.scale);
            }

            if (it.find("Render") != it.end()) {
                GltfImporter importer;
                std::shared_ptr<Import::Model> model = importer.import(it["Render"]["filename"].get<std::string>().c_str());
                registry->emplace<Render>(entity, std::make_shared<Model>(model));
            }

            if (it.find("Script") != it.end()) {
                registry->emplace<Script>(entity, it["Script"]["filename"].get<std::string>());
            }
        }
    }
};
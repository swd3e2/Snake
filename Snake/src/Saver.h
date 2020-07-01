#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include "Components.h"
#include <fstream>

class Saver {
private:
public:
    void saveToFile(const std::string& filename, entt::registry* registry) {
        using nlohmann::json;

        json data;

        int cntr = 0;
        registry->each([&](const entt::entity& entity) {
			if (registry->has<Render>(entity)) {
                const Render& render = registry->get<Render>(entity);
                data["Entities"][cntr]["Render"]["filename"] = render.model->getImportFilename();
            }
            if (registry->has<Transform>(entity)) {
                const Transform& transform = registry->get<Transform>(entity);
                data["Entities"][cntr]["Transform"] = {
                    { "rotation", { transform.rotation.x, transform.rotation.y, transform.rotation.z }},
                    { "translation", { transform.translation.x, transform.translation.y, transform.translation.z }},
                    { "scale", { transform.scale.x, transform.scale.y, transform.scale.z }}
                };
            }
            if (registry->has<Script>(entity)) {
                const Script& script = registry->get<Script>(entity);
                data["Entities"][cntr]["Script"]["filename"] = script.filename;
			}
			if (registry->has<PlayerComponent>(entity)) {
				const PlayerComponent& player = registry->get<PlayerComponent>(entity);
				data["Entities"][cntr]["Player"]["active"] = player.active;
				data["Entities"][cntr]["Player"]["speed"] = player.speed;
			}
			if (registry->has<CameraComponent>(entity)) {
				const CameraComponent& camera = registry->get<CameraComponent>(entity);
                std::vector<float> temp;
                for (int i = 0; i < 4; i++) {
                    temp.push_back(camera.projectionMatrix[i].x);
                    temp.push_back(camera.projectionMatrix[i].y);
                    temp.push_back(camera.projectionMatrix[i].z);
                    temp.push_back(camera.projectionMatrix[i].w);
                }

                data["Entities"][cntr]["Camera"]["projection"] = temp;

				for (int i = 0; i < 4; i++) {
					temp.push_back(camera.viewMatrix[i].x);
					temp.push_back(camera.viewMatrix[i].y);
					temp.push_back(camera.viewMatrix[i].z);
					temp.push_back(camera.viewMatrix[i].w);
				}

				data["Entities"][cntr]["Camera"]["view"] = temp;
			}
            cntr++;
		});

        std::ofstream fileStream(filename, std::ios::out);
        fileStream << data.dump(2);
        fileStream.close();
    }
};
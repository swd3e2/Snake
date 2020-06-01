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
                data["Entites"][cntr]["Render"]["filename"] = render.model->getImportFilename();
            }
            if (registry->has<Transform>(entity)) {
                const Transform& transform = registry->get<Transform>(entity);
                data["Entites"][cntr]["Transform"] = {
                    { "rotation", { transform.rotation.x, transform.rotation.y, transform.rotation.z }},
                    { "translation", { transform.translation.x, transform.translation.y, transform.translation.z }},
                    { "scale", { transform.scale.x, transform.scale.y, transform.scale.z }}
                };
            }
            if (registry->has<Script>(entity)) {
                const Script& script = registry->get<Script>(entity);
                data["Entites"][cntr]["Script"]["filename"] = script.filename;
            }
		});

        std::ofstream fileStream(filename, std::ios::out);
        fileStream << data.dump(2);
        fileStream.close();
    }
};
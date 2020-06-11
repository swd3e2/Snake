#pragma once

#include "QueueCommand.h"
#include <queue>
#include <vector>
#include <string>
#include "Graphics/Renderer.h"
#include "Graphics/Bindable.h"
#include <unordered_map>

class Pass {
protected:
    std::queue<std::function<void()>> commands;
    std::string name;
public:
    std::string source;
    std::string sink;
    std::vector<std::shared_ptr<Bindable>> bindables;
public:
    Pass(const std::string& name) : name(name) {}

    virtual void execute(Renderer* renderer) {
        for (auto& it : bindables) {
            it->bind(renderer->getContext());
        }

        while (!commands.empty()) {
            std::function<void()> command = commands.front();
            commands.pop();
            command();
        }
    }
    
    virtual void addCommand(std::function<void()> command) {
        commands.push(command);
    }
};
#pragma once

#include "QueueCommand.h"
#include <queue>
#include <vector>
#include <string>
#include "Graphics/Renderer.h"
#include "Graphics/Bindable.h"
#include <unordered_map>

class Pass {
private:
    std::queue<std::function<void()>> commands;
    std::string name;
public:
    std::string source;
    std::string sink;
    std::vector<Bindable*> bindables;
public:
    Pass(const std::string& name) :
        name(name) 
    {
    }

    void execute(Renderer* renderer) {
        for (auto& it : bindables) {
            it->bind(renderer->getContext());
        }

        while (!commands.empty()) {
            std::function<void()> command = commands.front();
            commands.pop();
            command();
        }
    }
    
    void addCommand(std::function<void()> command) {
        commands.push(command);
    }
};
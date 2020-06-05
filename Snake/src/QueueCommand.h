#include <functional>

struct QueueCommand {
    std::function<void()> callback;
};
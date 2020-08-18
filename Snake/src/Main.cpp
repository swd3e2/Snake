#include "Application.h"
#include "Graphics/RenderQueue/RenderQueue.h"

struct test {
    float a, b, c, d;
};

struct test2 {
    float a, b, c, d;
    float a1, b2, c3, d4;
};

int main(char* args)
{
#ifdef _DEBUG
	// disable the painfully slow MS heap check every 1024 allocs
	_CrtSetDbgFlag(0);
#endif
    Application app;
    app.run();
    return 0;
}

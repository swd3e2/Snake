#include "Application.h"

int main(char* args)
{
#ifdef _DEBUG
	// disable the painfully slow MS heap check every 1024 allocs
	_CrtSetDbgFlag(0);
#endif
    Application app;
    app.init();
    app.run();
    return 0;
}

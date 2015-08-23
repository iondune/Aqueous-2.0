
#include "CApplication.h"


int main()
{
	SingletonPointer<CApplication> Application;
	Application->Run();
	return 0;
}

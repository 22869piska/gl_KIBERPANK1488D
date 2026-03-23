#include "main.hpp"


int main()
{
	Engine* p = new Engine();
	
	p->Run();
	p->~Engine();
}

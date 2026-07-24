#include "main.hpp"


int main()
{
	Engine* p = new Engine();
	//native_ptr = p;
	
	p->Run();
	p->~Engine();
}

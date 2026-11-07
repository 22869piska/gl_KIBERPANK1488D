#include "main.hpp"


static Engine p;
int main()
{
	p.Run();
	p.~Engine();
}

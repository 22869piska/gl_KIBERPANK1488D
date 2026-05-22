#pragma once


namespace ASM
{
	extern "C" unsigned long long ClockCounter();

	extern "C" void RuntimeProc();
	extern "C" void RuntimeProc_START();
	extern "C" void RuntimeProc_END();
	void* RP_START = (void*)&ASM::RuntimeProc_START;
	void* RP_END   = (void*)&ASM::RuntimeProc_END;
	size_t RP_SIZE = (uintptr_t)RP_END - (uintptr_t)RP_START; //SIZE 

}

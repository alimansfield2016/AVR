#pragma once
#include "../../include/multiprocess/process.hpp"
// #include <avr/interrupt.h>
#include <bitset>

namespace AVR::MultiProcess
{
	enum class ProcessState : std::uint8_t{
		None,
		Executing,
		Suspended,
		Ready,
		Depends, //?
	};
	struct Process
	{
		ProcessState m_state;
		// id_type m_processID = -1;
		std::size_t m_stackBlocks;
		std::uintptr_t m_stackStart;
		std::uintptr_t m_stackSaved;
		std::size_t m_processTimeout;
	};


	extern "C" void __vector_18();
	// using scheduler = __vector_18;
	// extern "C" __attribute__((interrupt, alias("__vector_18"))) void __vector_taskScheduler();
	extern "C" void enableScheduler();
	extern "C" void disableScheduler();
	extern "C" void scheduler_frame();
	extern "C" void useless();

} // namespace AVR::MultiProcess

// extern std::bitset<(sram_size>>9)> stack_block_mask;
extern AVR::MultiProcess::Process processes[];
extern AVR::MultiProcess::id_type runningProcessID;
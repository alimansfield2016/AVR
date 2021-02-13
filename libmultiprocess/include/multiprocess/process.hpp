#pragma once

#include <cstdint>

// #include "../../src/include/process_private.hpp"

namespace AVR::MultiProcess
{
	using id_type = std::int8_t;
	extern const id_type MAX_PROCESSES;

	void init(std::size_t);
	id_type fork();
	// extern "C" void scheduler_frame();
	// template<typename ...T>
	// bool spawn(void (*fn)(T...), T..., std::size_t)
	id_type spawn(void (*fn)(), std::size_t);

	namespace thisProcess
	{
		extern id_type ID();
		extern void exit();
		extern void yield();
	} // namespace thisProcess
	
} // namespace AVR::MultiProcess

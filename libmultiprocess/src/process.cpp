#include "include/process_private.hpp"

#include <string.h>
#include <avr/core/registers.hpp>
#include <avr/io/io.hpp>
#include <avr/io/serial.hpp>

#define DEBUG(c)
// #define DEBUG(c) USART0.Print(c)


using namespace AVR::MultiProcess;

const id_type AVR::MultiProcess::MAX_PROCESSES = 4;
constexpr std::uintptr_t sram_end = 0x1100;
constexpr std::uintptr_t sram_size = 0x1000;
constexpr std::uintptr_t stack_space = sram_size/2;
constexpr std::uintptr_t stack_begin = sram_end-stack_space;
constexpr std::size_t stack_block_size = 0x0080;
std::bitset<((sram_size/2)/stack_block_size)> stack_block_mask;


Process processes[MAX_PROCESSES];
id_type runningProcessID = -1;


//inline static 
id_type alloc_pid()
{
	id_type newPID = 0;
	while(processes[newPID].m_state != ProcessState::None && newPID < MAX_PROCESSES)
		newPID++;
	if(newPID >= MAX_PROCESSES)
		return newPID = 0;
	return newPID;
}

//inline static 
bool alloc_stack(std::size_t stackSize, id_type newPID)
{
	uint8_t numChunks = stackSize/stack_block_size;
	if(stackSize%stack_block_size) numChunks++;
	int8_t free_idx = -1, idx = -1;
	uint8_t free_size = -1, size = 0;
	for(uint8_t i = 0; i < stack_block_mask.size(); i++){
		if(!stack_block_mask[i]){
			if(!size)
				idx = i;
			size++;
		} else if(size >= numChunks) {
			if(size < free_size) {
				free_size = size;
				free_idx = idx;
			}
			size = 0;
		} else {
			size = 0;
		}
	}
	if(size >= numChunks && size < free_size) {
		free_size = size;
		free_idx = idx;
	}
	if(free_idx < 0) return false;
	for(uint8_t i = 0; i < numChunks; i++)
		stack_block_mask[free_idx + i] = true;
	processes[newPID].m_stackBlocks = numChunks;
	processes[newPID].m_stackStart = stack_begin + free_idx*stack_block_size;
	processes[newPID].m_stackSaved = stack_begin + (free_idx+numChunks)*stack_block_size-1;
	return true;
}


void AVR::MultiProcess::init(std::size_t stackSize)
{
	if(runningProcessID != -1) return; 
	// divide by 256 (stack chunk size)
	std::size_t size = stackSize / stack_block_size;
	if(stackSize % stack_block_size) size++;

	processes[0] = {
		ProcessState::Executing,
		// 0,
		size,
		sram_end - size*stack_block_size,
		0,
		0
	};
	for(id_type id = 1; id < MAX_PROCESSES; id++)
		processes[id].m_state = ProcessState::None;
	runningProcessID = 0;
	//initialise timer
	TCNT0 = 0;
	TCCR0A = 0;
	TCCR0B = 0x04;
	// return;
	TIMSK0 = 0x01;
}

id_type AVR::MultiProcess::fork()
{
	id_type newPID = 0, oldPID = runningProcessID;
	disableScheduler();
	newPID = alloc_pid();
	if(!newPID) return 0;
	//allocate the best fitting chunk for stack
	uint8_t stackSize = processes[runningProcessID].m_stackBlocks*stack_block_size;

	if(!alloc_stack(stackSize, newPID)){
		enableScheduler();
		return 0;
	}
	
	//copy stack
	uintptr_t count = processes[newPID].m_stackBlocks*stack_block_size;
	for(uintptr_t i = 0; i < count; i++)
		*reinterpret_cast<std::uint8_t*>(processes[newPID].m_stackStart + i) = *reinterpret_cast<std::uint8_t*>(processes[runningProcessID].m_stackStart + i);
	processes[newPID].m_stackSaved = processes[newPID].m_stackStart + SP - processes[runningProcessID].m_stackStart;

	// memcpy(reinterpret_cast<std::uint8_t*>(processes[oldPID].m_stackStart), reinterpret_cast<std::uint8_t*>(processes[newPID].m_stackStart), count);

	runningProcessID = newPID;
	//create new scheduler frame on stack
	//execution splits at the return from this function
	//since we duplicate the scheduler frame with return value here
	scheduler_frame();
	if(runningProcessID == newPID){
		//new thread/task
		processes[oldPID].m_state = ProcessState::Suspended;
		processes[oldPID].m_stackSaved = SP;
		SP = processes[newPID].m_stackSaved;
		processes[newPID].m_state = ProcessState::Executing;
	}
	enableScheduler();
	return newPID;
}

// template<typename ...T>
// bool AVR::MultiProcess::spawn(void (*fn)(T...), T..., std::size_t stackSize)
id_type AVR::MultiProcess::spawn(void (*fn)(), std::size_t stackSize)
{
	id_type newPID = 0, oldPID = runningProcessID;
	disableScheduler();
	newPID = alloc_pid();
	if(!newPID) return 0;
	//allocate the best fitting chunk for stack
	if(!alloc_stack(stackSize, newPID)){
		enableScheduler();
		return 0;
	}

	runningProcessID = newPID;

	//create new scheduler frame on stack
	//execution splits at the return from this function
	//since we duplicate the scheduler frame with return value here
	scheduler_frame();
	if(runningProcessID == newPID){
		//new thread/task
		processes[oldPID].m_state = ProcessState::Suspended;
		processes[oldPID].m_stackSaved = SP;
		SP = processes[newPID].m_stackSaved;
		processes[newPID].m_state = ProcessState::Executing;
		enableScheduler();
		(*fn)();
		thisProcess::exit();
	}
	enableScheduler();
	return newPID;
}

extern "C" void __attribute__((interrupt, naked)) __vector_18()
{
	asm volatile(
		"sei				\n\t"
		"push r1			\n\t"
		"push r0			\n\t"
		"in r0,__SREG__		\n\t"
		"push r0			\n\t"
		"clr __zero_reg__	\n\t"
		"push r2			\n\t"
		"push r3			\n\t"
		"push r4			\n\t"
		"push r5			\n\t"
		"push r6			\n\t"
		"push r7			\n\t"
		"push r8			\n\t"
		"push r9			\n\t"
		"push r10			\n\t"
		"push r11			\n\t"
		"push r12			\n\t"
		"push r13			\n\t"
		"push r14			\n\t"
		"push r15			\n\t"
		"push r16			\n\t"
		"push r17			\n\t"
		"push r18			\n\t"
		"push r19			\n\t"
		"push r20			\n\t"
		"push r21			\n\t"
		"push r22			\n\t"
		"push r23			\n\t"
		"push r24			\n\t"
		"push r25			\n\t"
		"push r26			\n\t"
		"push r27			\n\t"
		"push r28			\n\t"
		"push r29			\n\t"
		"push r30			\n\t"
		"push r31			\n\t"
	);
	
	DEBUG('0'+runningProcessID);
	using namespace AVR::MultiProcess;
	if(runningProcessID == -1) return;
	//disable scheduler interupt
	disableScheduler();

	if(runningProcessID < 0) {	// terminate running process
		runningProcessID = ~runningProcessID;
		Process &to_terminate = processes[runningProcessID];
		to_terminate.m_state = ProcessState::None;
		//deallocate stack
		std::uint8_t idx = (sram_end - to_terminate.m_stackStart) >> 9;
		for(uint8_t i = 0; i < to_terminate.m_stackBlocks>>8; i++)
			stack_block_mask[idx+i] = false;
		
	} else {	// suspend running process
		Process &process = processes[runningProcessID];
		process.m_state = ProcessState::Suspended;
	}

	// switch context
	{
		id_type id = runningProcessID;
		while(processes[++id].m_state == ProcessState::None) {
			if(id+1 >= MAX_PROCESSES) id = -1;
		}
		processes[runningProcessID].m_stackSaved = SP;
		SP = processes[id].m_stackSaved;
		processes[id].m_state = ProcessState::Executing;
		runningProcessID = id;
	}
	//reset timer
	TCNT0 = 0;
	enableScheduler();
	//continue
	asm volatile(
		"pop r31			\n\t"
		"pop r30			\n\t"
		"pop r29			\n\t"
		"pop r28			\n\t"
		"pop r27			\n\t"
		"pop r26			\n\t"
		"pop r25			\n\t"
		"pop r24			\n\t"
		"pop r23			\n\t"
		"pop r22			\n\t"
		"pop r21			\n\t"
		"pop r20			\n\t"
		"pop r19			\n\t"
		"pop r18			\n\t"
		"pop r17			\n\t"
		"pop r16			\n\t"
		"pop r15			\n\t"
		"pop r14			\n\t"
		"pop r13			\n\t"
		"pop r12			\n\t"
		"pop r11			\n\t"
		"pop r10			\n\t"
		"pop r9				\n\t"
		"pop r8				\n\t"
		"pop r7				\n\t"
		"pop r6				\n\t"
		"pop r5				\n\t"
		"pop r4				\n\t"
		"pop r3				\n\t"
		"pop r2				\n\t"
		"pop r0				\n\t"
		"out __SREG__,r0	\n\t"
		"pop r0				\n\t"
		"pop r1				\n\t"
		"reti				\n\t"
	);
}

AVR::MultiProcess::id_type AVR::MultiProcess::thisProcess::ID()
{
	return runningProcessID;
}

void AVR::MultiProcess::thisProcess::exit()
{
	//disable scheduler interrupt
	disableScheduler();
	runningProcessID = ~runningProcessID;
	__vector_18();
}
void AVR::MultiProcess::thisProcess::yield()
{
	__vector_18();
}

void AVR::MultiProcess::enableScheduler()
{
	TCCR0B |= 0x03; // clk/64
}
void AVR::MultiProcess::disableScheduler()
{
	TCCR0B &= ~0x07; // clk/64
}

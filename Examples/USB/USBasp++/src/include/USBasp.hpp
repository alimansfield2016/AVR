#pragma once

#include <stdint.h>

#include <usb/device.hpp>


namespace USBasp
{
	void BootloaderLeave();

	extern const AVR::USB::Device pgmDevice;
	
	enum class Request : uint8_t
	{
		Connect = 1,
		Disconnect = 2,
		Transmit = 3,
		ReadFlash = 4,
		EnableProg = 5,
		WriteFlash = 6,
		ReadEeprom = 7,
		WriteEeprom = 8,
		SetLongAddress = 9,
		SetISPSCK = 10,
		TPI_Connect = 11,
		TPI_Disconnect = 12,
		TPI_RawRead = 13,
		TPI_RawWrite = 14,
		TPI_ReadBlock = 15,
		TPI_WriteBlock = 16,
		GetCapabilities = 127,
	};
} // namespace USBasp

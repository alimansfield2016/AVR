#pragma once

#include <usb/endpoint0.hpp>
#include <usb/descriptor.hpp>
#include "USBasp.hpp"

#define BLOCK_FIRST 0x01
#define BLOCK_LAST 0x02

namespace USBasp
{
	class Endpoint0 : public AVR::USB::Endpoint0
	{
		uint16_t pageAddr;
		uint16_t pageOffset;
		uint16_t pageSize;
		uint16_t nBytes;
		uint8_t buffer[8];
		uint8_t buf_len;
		uint8_t blockFlags;
		enum class State : uint8_t{
			Idle,
			PageAddressSet,
			WriteFlash,
			ReadFlash,
			WriteEeprom,
			ReadEeprom,
		}state;
		AVR::pgm_ptr<uint8_t> buf_ptr;
	public:
		Endpoint0(const AVR::USB::EndpointDescriptor *descIn, const AVR::USB::EndpointDescriptor *descOut);

		bool out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup) override;
		bool setup(uint8_t *rxBuf, uint8_t &rxLen);
		void in() override;

	private:
		void transmit_m644p(uint8_t *buf);

		void loadFlash();
		void loadEeprom();
	};
} // namespace USBasp

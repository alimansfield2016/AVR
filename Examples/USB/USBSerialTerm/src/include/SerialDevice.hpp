#pragma once

#include <usb/device.hpp>
#include <usb/descriptor.hpp>
#include <usb/interface.hpp>
#include <usb/configuration.hpp>

#define NUM_USBSERIAL 7

#define USBSERIAL_DEF(num, itfNo) \
extern const AVR::USB::EndpointDescriptor pgmSerial##num##In PROGMEM;	\
extern const AVR::USB::EndpointDescriptor pgmSerial##num##Out PROGMEM;	\
extern const AVR::pgm_array<AVR::USB::Endpoint*, 2> pgmEndpoints##itfNo PROGMEM; \
extern const AVR::USB::Interface pgmInterface##itfNo##Alt0 PROGMEM; \
extern const AVR::pgm_array<AVR::USB::Interface, 1> pgmInterface##itfNo PROGMEM;


namespace AVR::USBSerial
{
	#if (NUM_USBSERIAL) > 0
	USBSERIAL_DEF(0, 0)
	#endif
	#if (NUM_USBSERIAL) > 1
	USBSERIAL_DEF(1, 1)
	#endif
	#if (NUM_USBSERIAL) > 2
	USBSERIAL_DEF(2, 2)
	#endif
	#if (NUM_USBSERIAL) > 3
	USBSERIAL_DEF(3, 3)
	#endif
	#if (NUM_USBSERIAL) > 4
	USBSERIAL_DEF(4, 4)
	#endif
	#if (NUM_USBSERIAL) > 5
	USBSERIAL_DEF(5, 5)
	#endif
	#if (NUM_USBSERIAL) > 6
	USBSERIAL_DEF(6, 6)
	#endif
	#if (NUM_USBSERIAL) > 7
	USBSERIAL_DEF(7, 7)
	#endif
	extern const AVR::pgm_array<AVR::USB::StringDescriptorTable::Str, 4> pgmStrings PROGMEM;
	extern const AVR::USB::StringDescriptorTable pgmStringTbl PROGMEM;
	extern const AVR::pgm_array<AVR::pgm_span<AVR::USB::Interface>, NUM_USBSERIAL> pgmInterfaces PROGMEM;
	extern const AVR::USB::Configuration pgmConfiguration0 PROGMEM;
	extern const AVR::pgm_array<AVR::USB::Configuration, 1> pgmConfigurations PROGMEM;
	extern const AVR::USB::Device pgmDevice PROGMEM;
} // namespace AVR::USBSerial

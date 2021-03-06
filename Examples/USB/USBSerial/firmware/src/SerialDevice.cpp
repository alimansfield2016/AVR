#include <usb/device.hpp>
#include <usb/configuration.hpp>
#include <usb/interface.hpp>
#include <usb/endpoint.hpp>
#include <usb/descriptor.hpp>

#include "include/SerialEndpoint.hpp"
#include "include/SerialDevice.hpp"

using namespace AVR::USB;
using namespace AVR::USBSerial;

#define STR(str) sizeof(str)-2, str
#define _STR(name, str) \
const wchar_t _##name[] PROGMEM = L##str
// const StringDescriptorTable::Str name PROGMEM {STR(_##name)}
#define CUR_UBS_EPT_NO 1
#define CUR_UBS_ITF_NO 0


_STR(MfrStr, "bit.ly/amusb");
_STR(ProdStr, "libusb++");
_STR(SerStr, "IlMatto_Izzy");
_STR(ItfStr, "USBserial");

const AVR::pgm_array<StringDescriptorTable::Str, 4> AVR::USBSerial::pgmStrings PROGMEM{{
	{STR(_MfrStr)},
	{STR(_ProdStr)},
	{STR(_SerStr)},
	{STR(_ItfStr)},
}};

const StringDescriptorTable AVR::USBSerial::pgmStringTbl PROGMEM{
	LanguageID::English_United_States,
	pgmStrings
};

#define USBSERIAL_ENDPT_DESC(num, endptno, delay) \
const EndpointDescriptor AVR::USBSerial::pgmSerial##num##In PROGMEM{				\
	EndpointDirection::IN,							\
	endptno,										\
	EndpointTransferType::Interrupt,				\
	EndpointSynchronisationType::None,				\
	EndpointUsageType::Data,						\
	delay											\
};													\
const EndpointDescriptor AVR::USBSerial::pgmSerial##num##Out PROGMEM{				\
	EndpointDirection::OUT,							\
	endptno,										\
	EndpointTransferType::Interrupt,				\
	EndpointSynchronisationType::None,				\
	EndpointUsageType::Data,						\
	delay											\
};													\


#define USBSERIAL_ENDPTS(num, itfno) \
const AVR::pgm_array<Endpoint*, 2> AVR::USBSerial::pgmEndpoints##itfno PROGMEM{{	\
	static_cast<EndpointIn*>(&USBSerial##num),								\
	static_cast<EndpointOut*>(&USBSerial##num),								\
}};																			\


#define USBSERIAL_INTF(num, itfno) \
const Interface AVR::USBSerial::pgmInterface##itfno##Alt0 PROGMEM{	\
	InterfaceClass::ApplicationSpecific,		\
	0,											\
	0,											\
	pgmEndpoints##itfno,							\
	itfno,											\
	0,											\
	4											\
};												\
const AVR::pgm_array<Interface, 1> AVR::USBSerial::pgmInterface##itfno PROGMEM{{\
	pgmInterface##itfno##Alt0													\
}};																				\

#define USBSERIAL(num, delay, eptNo, itfNo) \
USBSERIAL_ENDPT_DESC(num, eptNo, delay)			\
USBSERIAL_ENDPTS(num, itfNo)					\
USBSERIAL_INTF(num, itfNo)

#ifndef NUM_USBSERIAL
#define NUM_USBSERIAL 0
#endif

#define USBSERIAL_DELAY 10

#if (NUM_USBSERIAL) > 0
	USBSERIAL(0, USBSERIAL_DELAY, 1, 0)
#endif
#if (NUM_USBSERIAL) > 1
	USBSERIAL(1, USBSERIAL_DELAY, 2, 1)
#endif
#if (NUM_USBSERIAL) > 2
	USBSERIAL(2, USBSERIAL_DELAY, 3, 2)
#endif
#if (NUM_USBSERIAL) > 3
	USBSERIAL(3, USBSERIAL_DELAY, 4, 3)
#endif
#if (NUM_USBSERIAL) > 4
	USBSERIAL(4, USBSERIAL_DELAY, 5, 4)
#endif
#if (NUM_USBSERIAL) > 5
	USBSERIAL(5, USBSERIAL_DELAY, 6, 5)
#endif
#if (NUM_USBSERIAL) > 6
	USBSERIAL(6, USBSERIAL_DELAY, 7, 6)
#endif
#if (NUM_USBSERIAL) > 7
	USBSERIAL(7, USBSERIAL_DELAY, 8, 7)
#endif


const AVR::pgm_array<AVR::pgm_span<Interface>, NUM_USBSERIAL> AVR::USBSerial::pgmInterfaces PROGMEM{{
	#if (NUM_USBSERIAL) > 0
	 pgmInterface0
	#endif
	#if (NUM_USBSERIAL) > 1
	,pgmInterface1
	#endif
	#if (NUM_USBSERIAL) > 2
	,pgmInterface2
	#endif
	#if (NUM_USBSERIAL) > 3
	,pgmInterface3
	#endif
	#if (NUM_USBSERIAL) > 4
	,pgmInterface4
	#endif
	#if (NUM_USBSERIAL) > 5
	,pgmInterface5
	#endif
	#if (NUM_USBSERIAL) > 6
	,pgmInterface6
	#endif
	#if (NUM_USBSERIAL) > 7
	,pgmInterface7
	#endif
}};

const Configuration AVR::USBSerial::pgmConfiguration0 PROGMEM{
	ConfigurationAttributes::NONE,
	pgmInterfaces,
	1
};

const AVR::pgm_array<Configuration, 1> AVR::USBSerial::pgmConfigurations PROGMEM{{
	pgmConfiguration0
}};

const Device AVR::USBSerial::pgmDevice PROGMEM{
	USB_BCD::USB1_1,
	0x16c0,
	0x05dc,
	0x0100,
	DeviceClass::VendorSpecific,
	0x00,
	0x00,
	pgmConfigurations,
	pgmStringTbl,
	1,
	2,
	3,
};
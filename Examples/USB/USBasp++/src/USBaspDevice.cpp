
#include <usb/libusb++.hpp>
#include <usb/descriptor.hpp>
#include <usb/device.hpp>
#include <usb/configuration.hpp>
#include <usb/interface.hpp>

#include "include/USBasp.hpp"

#include <array>

const wchar_t PROGMEM _MfrStr[] = L"www.fischl.de";
const wchar_t PROGMEM _ProdStr[] = L"USBasp";
const wchar_t PROGMEM _SerStr[16] = L"IlMatto_izzy";

#define STR(str) sizeof(str)-2, str

using Str = AVR::USB::StringDescriptorTable::Str;

const AVR::pgm_array<Str, 3> PROGMEM pgmStringsEN_US
{{
	{STR(_MfrStr)},
	{STR(_ProdStr)},
	{STR(_SerStr)},
}};

const AVR::USB::StringDescriptorTable PROGMEM pgmStringTable
{
	AVR::USB::LanguageID::English_United_States,
	pgmStringsEN_US
};

const AVR::pgm_array<AVR::USB::Endpoint*, 0> PROGMEM pgmEndpoints{}; 

const AVR::USB::Interface PROGMEM pgmInterface{
	AVR::USB::InterfaceClass::NONE,
	0x00,
	0x01,
	pgmEndpoints,
	0,
	0,
	2
};

const AVR::pgm_array<AVR::USB::Interface, 1> PROGMEM pgmInterfaceConfiguration{
	pgmInterface
};

const AVR::pgm_span PROGMEM pgmInterfaceConfigurationSpan{
	pgmInterfaceConfiguration
};
const AVR::pgm_array<AVR::pgm_span<AVR::USB::Interface>, 1> PROGMEM pgmInterfaceConfigurations{
	pgmInterfaceConfigurationSpan
};

const AVR::USB::Configuration PROGMEM pgmConfiguration{
	AVR::USB::ConfigurationAttributes::NONE,
	pgmInterfaceConfigurations,
	1
};

const AVR::pgm_array<AVR::USB::Configuration, 1> PROGMEM pgmConfigurations{
	AVR::USB::Configuration{
		AVR::USB::ConfigurationAttributes::NONE,
		pgmInterfaceConfigurations,
		1
	}
	// pgmConfiguration
};

const AVR::USB::Device PROGMEM USBasp::pgmDevice{
	AVR::USB::USB_BCD::USB1_1,
	0x16C0,
	0x05DC,
	0x0104,
	AVR::USB::DeviceClass::VendorSpecific,
	0x00,
	0x00,
	pgmConfigurations,
	pgmStringTable,
	1,
	2,
	3,
};

std::array arr{123};

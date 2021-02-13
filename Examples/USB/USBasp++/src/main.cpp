#include <avr/interrupt/interrupt.hpp>
#include <avr/io/io.hpp>
#include <usb/libusb++.hpp>
#include <avr/io/serial.hpp>

#include "include/USBasp.hpp"
#include "include/endpUSBasp.hpp"


constexpr void (*application_reset)(void) = 0x0000;

USBasp::Endpoint0 USBaspEndpoint0{
	nullptr,
	nullptr
};


void USBasp::BootloaderLeave()
{

	//do we want to check if application is signed correctly?

	//reset vectors and off we go
	AVR::Interrupt::disable();
	AVR::USB::disconnect();

	AVR::Interrupt::setVectors(AVR::Interrupt::Vector::Application);
	application_reset();
}

int main()
{
	DDRB |= 0x03;
	PORTB &= ~0x0F;
	USART0.begin(115200);
	USART0.puts(P("init test"));


	AVR::USB::pDevice = &USBasp::pgmDevice;
	AVR::USB::init(&USBaspEndpoint0);

	while(1);
}
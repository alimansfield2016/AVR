#include <usb/libusb++.hpp>
#include <avr/interrupt/interrupt.hpp>

#include "include/SerialDevice.hpp"
#include "include/SerialEndpoint.hpp"

#include <avr/io/serial.hpp>
#include <multiprocess/process.hpp>

#include "include/terminal.hpp"

using namespace AVR::USBSerial;

#define TERM(num) \
void term##num()		\
{					\
	Terminal t##num{USBSerial##num}; \
	t##num.run();		\
}
TERM(0)
TERM(1)
TERM(2)
TERM(3)

int main()
{ 
	USART0.begin(115200);

	AVR::USB::pDevice = &AVR::USBSerial::pgmDevice;
	AVR::USB::init();

	USBSerial0.echo(false);
	USBSerial1.echo(false);
	USBSerial2.echo(true);
	USBSerial3.echo(true);

	while(!AVR::USB::ready());


	AVR::MultiProcess::init(256);
	AVR::MultiProcess::spawn(term0, 256);
	AVR::MultiProcess::spawn(term1, 256);

	while(1) USBSerial5.puts(P("Hello World!\r\n"));
}
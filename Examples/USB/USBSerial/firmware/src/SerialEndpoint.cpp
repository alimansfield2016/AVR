#include "include/SerialEndpoint.hpp"
#include "include/SerialDevice.hpp"

#include <avr/io/io.hpp>
#include <avr/io/serial.hpp>

using namespace AVR::USBSerial;
using namespace AVR::USB;

#define USBSERIAL_ENDPT(num) \
SerialEndpoint AVR::USBSerial::USBSerial##num{	\
	&pgmSerial##num##In, 							\
	&pgmSerial##num##Out							\
};											\


#if (NUM_USBSERIAL) > 0
USBSERIAL_ENDPT(0)
#endif
#if (NUM_USBSERIAL) > 1
USBSERIAL_ENDPT(1)
#endif
#if (NUM_USBSERIAL) > 2
USBSERIAL_ENDPT(2)
#endif
#if (NUM_USBSERIAL) > 3
USBSERIAL_ENDPT(3)
#endif
#if (NUM_USBSERIAL) > 4
USBSERIAL_ENDPT(4)
#endif
#if (NUM_USBSERIAL) > 5
USBSERIAL_ENDPT(5)
#endif
#if (NUM_USBSERIAL) > 6
USBSERIAL_ENDPT(6)
#endif
#if (NUM_USBSERIAL) > 7
USBSERIAL_ENDPT(7)
#endif

bool SerialEndpoint::out(uint8_t *rxBuf, uint8_t &rxLen, bool)
{
	uint8_t count = rxLen-3;
	// if(count > m_buf_in.space()) return true;
	for(uint8_t i = 1; i < rxLen-2; i++){
		if(!m_buf_in.full())
			m_buf_in.push_back(rxBuf[i]);
	}
	if(m_echo)
		for(uint8_t i = 1; i < rxLen-2; i++){
			m_buf_out.push_back(rxBuf[i]);
		}
	rxLen = 0;
	return true;
}

void SerialEndpoint::in()
{
	int i;
	for(i = 0; i < 8 && !m_buf_out.empty(); i++)
		txBuf[i] = m_buf_out.pop_front();
	if(i){
		genPacket(getDataPID(), i);
	}
}


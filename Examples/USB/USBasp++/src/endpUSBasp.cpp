#include "include/endpUSBasp.hpp"

#include <avr/io/io.hpp>

#include <avr/boot/flash.hpp>
#include <avr/boot/fuse.hpp>
#include <avr/boot/lock.hpp>
#include <avr/boot/signature.hpp>

#include <avr/memory/pgmspace.hpp>
#include <avr/memory/eeprom.hpp>
#include <avr/io/serial.hpp>

#define transmit transmit_m644p

USBasp::Endpoint0::Endpoint0(const AVR::USB::EndpointDescriptor *descIn, const AVR::USB::EndpointDescriptor *descOut) : 
	AVR::USB::Endpoint0{descIn, descOut},
	buf_ptr{nullptr} 
	{}

bool USBasp::Endpoint0::out(uint8_t *rxBuf, uint8_t &rxLen, bool _setup)
{
	if(AVR::USB::Endpoint0::out(rxBuf, rxLen, _setup))
		return true;
	if(_setup)
		return USBasp::Endpoint0::setup(rxBuf, rxLen);

	USART0.Print(pageOffset&0xFF);

	switch (state)
	{
	case State::WriteFlash:
		for(uint8_t i = 1; i < rxLen-2; i++){
			uint8_t b = rxBuf[i];
			AVR::Boot::spmWrite8(b, pageAddr+pageOffset, AVR::Boot::spmOperation::PageBufferWrite);
			pageOffset += 1;
			if(pageOffset >= pageSize){
				//erase, flush
				AVR::Boot::spmOp(pageAddr, AVR::Boot::spmOperation::PageErase);
				AVR::Boot::spmOp(pageAddr, AVR::Boot::spmOperation::PageWrite);
				pageAddr += pageOffset;
				pageOffset = 0;
			}
			if(!--nBytes){
				genPacket(getDataPID(), 0);
				if(blockFlags & BLOCK_LAST){
					//erase, flush
					AVR::Boot::spmOp(pageAddr, AVR::Boot::spmOperation::PageErase);
					AVR::Boot::spmOp(pageAddr, AVR::Boot::spmOperation::PageWrite);
				}
				state = State::Idle;
			}
		}
		break;
	
	default:
		return false;
		break;
	}

	rxLen = 0;
	return true;
}

bool USBasp::Endpoint0::setup(uint8_t *rxBuf, uint8_t &rxLen)
{
	USART0.Print('S');
	USBasp::Request request = static_cast<USBasp::Request>(rxBuf[2]);
	rxLen = 0;
	setDataPID(AVR::USB::PID::DATA1);
	int8_t len = 0;
	switch (request)
	{
		case Request::Connect:		//COMPLETE
			//Send 0-length data
			break;
		case Request::Disconnect:
		//Exit bootloader cleanly
		break;
		case Request::Transmit:		//WIP
		//This one's more complicated
			transmit(rxBuf+3);
			len = 4;
			break;
		case Request::ReadFlash:
			if(state != State::PageAddressSet)
				pageAddr = rxBuf[4]<<8|rxBuf[3];
			pageOffset = rxBuf[6]<<8|rxBuf[5];
			pageSize = rxBuf[8]<<8|rxBuf[7];
			len = -1;
			//set state. We're going to send data!
			USART0.Print('r');
			state = State::ReadFlash;
			break;
		case Request::EnableProg:	//COMPLETE
			//Enable programming
			txBuf[0] = 0;
			len = 1;
			break;
		[[likely]] 
		case Request::WriteFlash:
			if(state != State::PageAddressSet)
				pageAddr = rxBuf[4]<<8|rxBuf[3];
			pageSize = rxBuf[5];
			blockFlags = rxBuf[6]&0x0F;
			pageSize += static_cast<uint16_t>(rxBuf[6]&0xF0)<<4;
			if(blockFlags&BLOCK_FIRST){
				pageOffset = 0;
			}
			// pageOffset = rxBuf[6]<<8|rxBuf[5];
			nBytes = rxBuf[8]<<8|rxBuf[7];
			//set state. We're expecting incoming data!
			state = State::WriteFlash;
			len = -1;
			break;
		case Request::ReadEeprom: break;
		case Request::WriteEeprom: break;
		case Request::SetLongAddress:
			pageAddr = rxBuf[4]<<8|rxBuf[3];
			state = State::PageAddressSet;
			break;
		case Request::SetISPSCK:	//COMPLETE
			//Send 0-length data
			// txBuf[0] = 0;
			// len = 1;
			break;
		case Request::TPI_Connect:
		case Request::TPI_Disconnect:
		case Request::TPI_RawRead:
		case Request::TPI_RawWrite:
		case Request::TPI_ReadBlock:
		case Request::TPI_WriteBlock:
			break;
		case Request::GetCapabilities:	//COMPLETE
			//Send 0-length data
			// txBuf[0] = 0x00;
			// txBuf[1] = 0x00;
			// txBuf[2] = 0x00;
			// txBuf[3] = 0x00;
			// len = 4;
			break;
		default:
			return false;
	}
	if(len>=0)
		genPacket(getDataPID(), len);
	return true;
}


void USBasp::Endpoint0::in()
{
	if(reqType() == AVR::USB::RequestType::Standard)
		return AVR::USB::Endpoint0::in();
	if(txLen()) return;
	switch(state)
	{
		case State::ReadFlash:
			loadFlash();
			break;
		case State::ReadEeprom:
			loadEeprom();
			break;
	}

}

void USBasp::Endpoint0::transmit_m644p(uint8_t *vals)
{
	USART0.Print('t');
	uint8_t val = 0;
	txBuf[0] = 0;
	txBuf[1] = 0;
	txBuf[2] = 0;
	switch(*vals++){
		//Control instructions
		case 0xAC:{
				switch (*vals++)
				{
				case 0x53: //Programming Enable
					//Not possible again!
					[[fallthrough]];
				case 0x80: //Chip Erase
					//Not possible again!
					break;
				case 0xE0: //Write Lock bits
					//This we can do!
					AVR::Boot::setLockByte(vals[1]);
					break;
				case 0xA0: //Write Fuse bits
					//Not possible in Bootloader
					[[fallthrough]];
				case 0xA4: //Write Fuse High bits
					//Not possible in Bootloader
					[[fallthrough]];
				case 0xA8: //Write Extended Fuse bits
					//Not possible in Bootloader
					break;
				}
				break;
			}
		case 0xF0:	//Poll RDY/~BSY
			//I think this just returns true?
			break; //return 0
		//Load instructions
		case 0x4D:	//Load extended Address Byte
		case 0x48:	//Load Progmem memory page High byte
		case 0x40:	//Load Progmem memory page Low byte
		case 0xC1:	//Load EEPROM memory page
		//Read instructions
		case 0x28:	//Progmem High byte
		case 0x20:	//Progmem Low byte
			txBuf[3] = *AVR::pgm_ptr<uint8_t>{reinterpret_cast<uint8_t*>(vals[0]<<8|vals[1])};
			break;
		case 0xA0:	//EEPROM
		case 0x30:	//Signature byte xx
			val = AVR::Boot::signatureRead(vals[1]);
			break;
		case 0x38:	//Read Calibration byte
		case 0x50:	//Fuse bits
			{
				switch (*vals++)
				{
				case 0x00:	//Fuse Low
					val = AVR::Boot::getLowFuse();
					break;
				case 0x08:	//Fuse Extended
					val = AVR::Boot::getExtendedFuse();
					break;
				}
			}
			break;
		case 0x58:	//Fuse/Lock bits
			{
				switch (*vals++)
				{
				case 0x00:	//Lock bits
					val = AVR::Boot::getLockByte();
					break;
				case 0x08:	//Fuse High
					val = AVR::Boot::getHighFuse();
					break;
				}
			}
			break;
		//Write instructions
		case 0x4C:
		case 0xC0:
		case 0xC2:
		break;
	}
	txBuf[3] = val;
}

void USBasp::Endpoint0::loadFlash()
{
	USART0.Print('f');
	uint8_t len = 0;
	buf_ptr = {reinterpret_cast<uint8_t*>(pageAddr+pageOffset)};
	for(; len < 8 && pageSize; len++, pageSize--, pageOffset++)
		txBuf[len] = *buf_ptr++;
	genPacket(getDataPID(), len);
	if(len < 8)
		state = State::Idle;
}

void USBasp::Endpoint0::loadEeprom()
{
	USART0.Print('e');
	uint8_t len = 0;
	for(; len < 8 && pageOffset < pageSize; len++, pageOffset++)
		txBuf[len] = *AVR::eeprom_ptr{reinterpret_cast<uint8_t*>(pageAddr+pageOffset)};
	genPacket(getDataPID(), len);
	if(len < 8)
		state = State::Idle;
}


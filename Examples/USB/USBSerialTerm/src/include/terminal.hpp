#pragma once

#include <cstdlib>
#include <avr/io/serial.hpp>

#define CSI "\033["
#define FEED "\r\n"
#define TAB "\t"

extern const char prompt[];// PROGMEM = "> ";

class Terminal
{
	constexpr static std::size_t s_buf_len = 32;
	AVR::Serial &m_serial;
	char m_buf[s_buf_len];
	uint8_t m_idx;
	bool m_debug;
	bool m_cr;
	bool m_running;
	using ParseFn = bool (*)(Terminal&, void* ctx);
	ParseFn m_parseFn;
	void *m_parseCtx;
	AVR::pgm_string m_prompt;
	static bool commandParser(Terminal &term, void *ctx);
public:
	Terminal(AVR::Serial &serial, ParseFn _parseFn = commandParser, void *ctx = nullptr) : 
		m_serial{serial},
		m_idx{0},
		m_debug{false},
		m_running{true},
		m_parseFn{_parseFn},
		m_parseCtx{ctx},
		m_prompt{P("> ")} {}
	void setParseFn(ParseFn _parseFn) { m_parseFn = _parseFn; };
	ParseFn parseFn() const { return m_parseFn; }
	void setParseCtx(void *ctx) { m_parseCtx = ctx; }
	void *parseCtx() const { return m_parseCtx; }
	AVR::Serial &serial() { return m_serial; }
	void run();

};
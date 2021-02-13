#include "include/terminal.hpp"
#include "include/forth.hpp"
#include <avr/interrupt/watchdog.hpp>

typedef void(*Command)(Terminal &);
struct CommandMap
{
	AVR::pgm_span<std::pair<AVR::pgm_string, Command>> data;
	[[nodiscard]] constexpr Command at(const char *s) const {
		Command c = nullptr;
		for(const auto v : data){
				int i = 0;
				while(s[i] && v.first[i] && s[i]!=' '){
					if(s[i] != v.first[i]) break;
					i++;
				}
				if(v.first[i]) continue;
				if(s[i] != 0x00 && s[i] ) continue;
				return c = v.second;
		}
		return c;
	}
};
extern const char _help[];
void help(Terminal&); 

const char _reset[] PROGMEM = "reset";
/**
 * When using the original IlMatto bootloader
 * this will reset to bootloader mode.
 * If using a patched bootloader, it will not
 * enter bootloader mode
 */
void reset(Terminal&)
{
	AVR::Interrupt::disable();
	AVR::Watchdog::setTimeoutCycles(AVR::Watchdog::TimeoutCycles::_2K);
	AVR::Watchdog::setMode(AVR::Watchdog::Mode::Reset);
}


static constexpr AVR::pgm_array<std::pair<AVR::pgm_string, Command>, 3> commands PROGMEM = {{
	{AVR::pgm_ptr{ _help}, help},
	{AVR::pgm_ptr{_forth}, forth},
	{AVR::pgm_ptr{_reset}, reset}
}};
static constexpr CommandMap commandMap{commands};
const char _help[] PROGMEM = "help";
void help(Terminal &term)
{
	term.serial().puts(P(FEED"Listing all available commands:"));
	for(auto e : commands){
		term.serial().puts(P(FEED TAB));
		term.serial().puts(e.first);
	}
}
bool Terminal::commandParser(Terminal &term, void *ctx)
{
	Command c = commandMap.at(term.m_buf);
	if(!c) return false;
	(*c)(term);
	return true;
}

void Terminal::run()
{

	while(1){
		char c = m_serial.getc();
		switch(c){
			case '\r': m_cr = true; 
			[[fallthrough]];
			case '\n':{
				if(m_cr && c == '\n'){
					m_cr = false;
					break;
				}
				if(m_idx) {
					//parse input
					m_buf[m_idx] = 0;
					if(!(*m_parseFn)(*this, m_parseCtx)){
						m_serial.puts(P(FEED "Error parsing: "));
						m_serial.write(m_buf, m_idx);
					}
				}
				m_serial.puts(P(FEED));
				m_serial.puts(m_prompt);
				m_idx = 0;
				break;
			}
			case '\x0C':{	//form feed
				m_serial.puts(P(CSI"2J" CSI"0;0H"));
				m_serial.puts(m_prompt);
				m_serial.write(m_buf, m_idx);
				break;
			}
			case '\x7F':{	//backspace
				//move left
				//space
				//move left
				//only if i>0
				if(!m_idx) break;
				m_serial.puts(P("\b \b"));
				--m_idx;
				break;
			}
			default:{
				m_cr = false;
				static char arr[] = "0123456789ABCDEF";
				if(m_idx < s_buf_len-1 /* && isprintable*/ ){
					if(m_debug){
						m_serial.putc(arr[c>>4]);
						m_serial.putc(arr[c&0xF]);
					}else{
						m_serial.putc(c);
					}
					m_buf[m_idx] = c;
					++m_idx;
				}
			}
		}
	}
}
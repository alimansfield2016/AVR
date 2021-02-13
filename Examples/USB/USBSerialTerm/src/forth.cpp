#include "include/forth.hpp"


const char _forth[] PROGMEM = "forth";
void forth(Terminal &term)
{
	term.serial().puts(P(FEED "Forth interpreter unimplemented"));
}
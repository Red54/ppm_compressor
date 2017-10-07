#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <deque>

typedef unsigned int uint;
typedef uint Symbol;
typedef std::deque<Symbol> Context;
enum {ESC = 256};
typedef unsigned char uchar;
typedef unsigned char Bit;

#endif
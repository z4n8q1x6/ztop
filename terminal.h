#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

void setup_terminal();
void handle_input();
void reset_terminal();

#endif // TERMINAL_H

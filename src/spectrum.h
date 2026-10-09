#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <stdint.h>

#define SPEC_N 256

// Copy one fresh 256-sample mono window (int32). Returns false if no
// new window is ready yet. Clears the ready flag on success.
bool spectrum_fetch(int32_t* dest);

#endif

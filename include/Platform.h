#pragma once

#include <string>

namespace platform {

// Read a single character without echoing to the terminal.
// Returns '\r' on Enter and '\b' / 127 on Backspace.
char getCharNoEcho();

// Print text one character at a time with a small delay (ms).
void animatedText(const std::string& text, int delayMs);

// Read a password from stdin while masking each character with '*'.
std::string readPassword();

}  // namespace platform

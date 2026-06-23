#include "Platform.h"

#include <chrono>
#include <iostream>
#include <thread>

#if defined(_WIN32)
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace platform {

char getCharNoEcho() {
#if defined(_WIN32)
    return static_cast<char>(_getch());
#else
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (ch == '\n') ch = '\r';  // normalize Enter to '\r' for Windows parity
    return static_cast<char>(ch);
#endif
}

void animatedText(const std::string& text, int delayMs) {
    for (char ch : text) {
        std::cout << ch << std::flush;
        if (delayMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
}

std::string readPassword() {
    std::string password;
    std::cout << "Enter Password: ";
    while (true) {
        char ch = getCharNoEcho();
        if (ch == '\r' || ch == '\n') break;
        if (ch == '\b' || ch == 127) {  // backspace / DEL
            if (!password.empty()) {
                std::cout << "\b \b" << std::flush;
                password.pop_back();
            }
        } else {
            std::cout << '*' << std::flush;
            password.push_back(ch);
        }
    }
    std::cout << "\n";
    return password;
}

}  // namespace platform

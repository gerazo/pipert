
#ifndef TESTLIBRARY_UTILS_H
#define TESTLIBRARY_UTILS_H

#include <termios.h>
#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>

namespace measurements {
    ///keyboard hit generated
    int kbhit(void) {
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }

        return 0;
    }
}

#endif //TESTLIBRARY_UTILS_H

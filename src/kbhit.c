/**
 Linux (POSIX) implementation of _kbhit().
 Morgan McGuire, morgan@cs.brown.edu
 https://www.flipcode.com/archives/_kbhit_for_Linux.shtml
 */

#ifndef KBHIT
#define KBHIT

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
//#include <stropts.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif
/// end ///

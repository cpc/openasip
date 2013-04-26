#include <unistd.h>


namespace std {
    void terminate() {
        _exit(1);
    }
}

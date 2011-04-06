/**
 * Simple led blinker app for smoke testing
 */

void usleep(unsigned int us) {
    unsigned int counter = 1;
    _TCE_RTIMER(us, counter);
    counter = 1;
     while (counter != 0) {
         _TCE_RTIMER(0, counter);
    }      
}

int main() {
    volatile unsigned char pattern = 0x01;
    while (1) {
        _TCE_LEDS(pattern);
        usleep(100000);
        pattern <<= 1;
        if (pattern == 0)
            pattern = 0x01;
    }
    return 0;
}
   

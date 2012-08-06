volatile float f = 0.1f;
volatile float g = 0.2f;
volatile float fg[2];

int main(void) {
    _TCE_STW2(&fg, f,g);
    float ff = fg[0] * 10;
    float gg = fg[1] * 10;
    _TCE_STDOUT(48 + (int)(ff));
    _TCE_STDOUT(48 + (int)(gg));
}
    

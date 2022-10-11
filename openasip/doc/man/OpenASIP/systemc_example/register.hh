#ifndef SC_REGISTER_HH
#define SC_REGISTER_HH

#include <systemc>

SC_MODULE(Register) {
    sc_in<int> input;
    sc_out<int> output;
    sc_in<bool> updateValue;
    sc_in<bool> clock;

    int value;

    void run() {
        if (updateValue) {
            value = input;
        }
        output = value;
    }

    SC_CTOR(Register) {
        SC_METHOD(run);
        sensitive << clock.pos();
        sensitive << input;
        sensitive << updateValue;
        value = 0;
    }
};

#endif

/**
 * OSAL behavior definition file.
 */

#include "OSAL.hh"
#include "OperationGlobals.hh"

#include <cstring>



OPERATION(ID)
TRIGGER

IO(5) = IO(1);
IO(6) = IO(2);
IO(7) = IO(3);
IO(8) = IO(4);
RETURN_READY;

END_TRIGGER;
END_OPERATION(ID)

OPERATION(WRITE_TEST)

TRIGGER

const char* testData = "Test12345";

UIntWord addr = UINT(1);

char* t = (char*)testData;

while (*t) {
    MEMORY.write(addr, 1, t[0]);
    ++addr;
    ++t;
}

END_TRIGGER;

END_OPERATION(WRITE_TEST)




OPERATION(READ_TEST)

TRIGGER
UIntWord addr = UINT(1);

UIntWord c = 1;
while (c) {
    MEMORY.read(addr, 1, c);
    if (c == 0) break;
    OUTPUT_STREAM << (char)c;
    ++addr;
}
OUTPUT_STREAM << std::endl;
END_TRIGGER;

END_OPERATION(READ_TEST);

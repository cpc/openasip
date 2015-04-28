#include <stdint.h>
#define AHB_ADDRESS_SPACE __attribute__((address_space(1)))
#define IO_BASE ((volatile uint32_t AHB_ADDRESS_SPACE *) 0x666)
int main() {
        _TCE_SLEEP(0);
	_TCE_INT(IO_BASE[0]);
}

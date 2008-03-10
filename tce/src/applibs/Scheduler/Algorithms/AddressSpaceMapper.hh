/** 
 * @file AddressSpaceMapper.hh
 *
 * Declaration of AddresSpaceMapper class
 * 
 * Changes address spaces during scheduling
 */

#ifndef TTA_ADDRESS_SPACE_MAPPER_HH
#define TTA_ADDRESS_SPACE_MAPPER_HH

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class Machine;
}

class AddressSpaceMapper {
public:
    static void mapAddressSpace(
        TTAProgram::Program& program, const TTAMachine::Machine& machine);

};

#endif

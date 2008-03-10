/**
 * @file BasicMachineCheckSuite.hh
 *
 * Declaration of BasicMachineCheckSuite class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_MACHINE_CHECK_SUITE_HH
#define TTA_BASIC_MACHINE_CHECK_SUITE_HH

#include "MachineCheckSuite.hh"

/**
 * Basic machine suite tests for basic design errors such as register files
 * without input ports, etc.
 */
class BasicMachineCheckSuite : public MachineCheckSuite {
public:
    BasicMachineCheckSuite();    
    virtual ~BasicMachineCheckSuite();
};

#endif

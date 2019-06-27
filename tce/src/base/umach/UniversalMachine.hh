/*
    Copyright (c) 2002-2011 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file UniversalMachine.hh
 *
 * Declaration of UniversalMachine class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2006,2011 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_MACHINE_HH
#define TTA_UNIVERSAL_MACHINE_HH

#include "Machine.hh"
#include "OperationPool.hh"

class UniversalFunctionUnit;
class UnboundedRegisterFile;

// the size of the data address space in bytes (MAUs) is 4GB.
#define DATA_MEMORY_SIZE (static_cast<unsigned int>(4096)*1024*1024)

/**
 * UniversalMachine is used to mark unassigned/unscheduled resources.
 *
 * In the old versions of TCE, UniversalMachine represented a TTA processor 
 * which used to model an imaginary target for the old "sequential code", 
 * an intermediate format from the old gcc frontend. Previously, one UM 
 * instance per Program was created but from now one, the singleton object 
 * should be used everywhere because UM resources are just used to mark 
 * unscheduled parts of the program.
 */
class UniversalMachine : public TTAMachine::Machine {
public:
    virtual ~UniversalMachine();

    virtual bool isUniversalMachine() const;
    UniversalFunctionUnit& universalFunctionUnit() const;
    TTAMachine::RegisterFile& booleanRegisterFile() const;
    UnboundedRegisterFile& integerRegisterFile() const;
    UnboundedRegisterFile& doubleRegisterFile() const;
    TTAMachine::RegisterFile& specialRegisterFile() const;
    TTAMachine::AddressSpace& instructionAddressSpace() const;
    TTAMachine::AddressSpace& dataAddressSpace() const;
    TTAMachine::Bus& universalBus() const;

    virtual void addBus(TTAMachine::Bus& bus);
    virtual void addSocket(TTAMachine::Socket& socket);
    virtual void addFunctionUnit(TTAMachine::FunctionUnit& unit);
    virtual void addImmediateUnit(TTAMachine::ImmediateUnit& unit);
    virtual void addRegisterFile(TTAMachine::RegisterFile& unit);
    virtual void addAddressSpace(TTAMachine::AddressSpace& as);
    virtual void addBridge(TTAMachine::Bridge& bridge);
    virtual void addInstructionTemplate(TTAMachine::InstructionTemplate& iTemp);
    virtual void setGlobalControl(TTAMachine::ControlUnit& unit);

    virtual void removeBus(TTAMachine::Bus& bus);
    virtual void removeSocket(TTAMachine::Socket& socket);
    virtual void removeFunctionUnit(TTAMachine::FunctionUnit& unit);
    virtual void removeRegisterFile(TTAMachine::RegisterFile& unit);
    virtual void deleteAddressSpace(TTAMachine::AddressSpace& as);
    virtual void unsetGlobalControl();

    virtual void loadState(const ObjectState* state);

    static UniversalMachine& instance();

private:
    UniversalMachine();

    void construct();

    /// Indicates whether the UniversalMachine is built completely.
    bool isBuilt_;
    /// The operation pool instance to use for finding operations.
    OperationPool opPool;
    /// The singleton instance. Use this instance everywhere. Creating
    /// multiple instances of UM is deprecated.
    static UniversalMachine* instance_;
};

/// Machine component names reserved for the universal machine.
#define UM_BUS_NAME "universal_bus"
#define UM_SEGMENT_NAME "universal_segment"
#define UM_INPUT_SOCKET_NAME "universal_input_socket"
#define UM_OUTPUT_SOCKET_NAME "universal_output_socket"
#define UM_BOOLEAN_RF_NAME "universal_boolean_rf"
#define UM_BOOLEAN_RF_WRITE_PORT "universal_boolean_write_port"
#define UM_BOOLEAN_RF_READ_PORT "universal_boolean_read_port"
#define UM_INTEGER_URF_NAME "universal_integer_rf"
#define UM_INTEGER_URF_WRITE_PORT "universal_rf_write_port"
#define UM_INTEGER_URF_READ_PORT "universal_rf_read_port"
#define UM_DOUBLE_URF_NAME "universal_double_rf"
#define UM_DOUBLE_URF_WRITE_PORT "universal_drf_write"
#define UM_DOUBLE_URF_READ_PORT "universal_drf_read"
#define UM_SPECIAL_RF_NAME "universal_special_rf_return_value"
#define UM_SPECIAL_RF_WRITE_PORT "universal_special_rf_write"
#define UM_SPECIAL_RF_READ_PORT "universal_special_rf_read"
#define UM_UNIVERSAL_FU_NAME "universal_fu"
#define UM_IMEM_NAME "universal_instructions"
#define UM_DMEM_NAME "universal_data"
#define UM_GCU_NAME "universal_gcu"

#endif

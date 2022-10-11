/*
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
 * @file RemoteController.hh
 * @author Kalle Raiskila 2014
 *
 * Declaration of the RemoteController class.
 */

#ifndef TTA_REMOTE_CONTROLLER
#define TTA_REMOTE_CONTROLLER

#include "BaseType.hh"
#include "TTASimulationController.hh"
#include "AddressSpace.hh"
#include <sstream>

using TTAMachine::AddressSpace;

/**
 * RemoteController is a "simulation" controller base class for debuggers
 * on hardware implementations.
 * 
 * RemoteController is a base class for the TCE built-in debugger and the 
 * custom debugger classes. It adds virtual interfaces to TTASimulationController,
 * that are needed to integrate an FPGA or ASIC to the TCE debugging tools.
 * RemoteController is a interface class, and cannot be instantiated.
 */
class RemoteController : public TTASimulationController {
public:
    typedef MinimumAddressableUnit MAU;

    RemoteController(
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program);

    /** 
     * Write data to physical memory.
     *
     * @param address TTA's view of the memory address to which to write
     * @data one MAU of data to write. Needs to be masked, if MAU is 
     *       less than maximum MAU size.
     * @space the address space in which to write the data.
     */
    virtual void writeMem(Word address, MAU data, const AddressSpace&) = 0;

    /** 
     * Read data from physical memory.
     *
     * @param address TTA's view of the memory address from which to read
     * @space the address space from which to read the data.
     * @return one MAU of data to write. (TODO: is return value masked or not?)
     */
    virtual MAU readMem(Word address, const AddressSpace&) = 0;
    
    /**
     * Create and load instruction memory image from current program.
     *
     * This function generates a binary image of the instruction
     * memory, and stores it in cache. It is then written with loadInstructions(), 
     * which in turn call the pure virtual writeIMem, that subclasses need 
     * to implement.
     */
    virtual void loadIMemImage();

    /**
     * Write to phyical instruction memory.
     *
     * @param address TTA's view of the memory address to which to write
     * @data buffer of raw data to write to instruction memory
     * @size the size of the data buffer, in bytes
     */
    virtual void writeIMem(const char *data, int size) = 0;

    // virtual members from TTASimulationController
    virtual void step(double count = 1) = 0;

    virtual void next(int count = 1) = 0;

    virtual void run() = 0;

    virtual void runUntil(UIntWord address) = 0;

    virtual void reset() = 0;
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1) = 0;
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1) = 0;
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName) = 0;
    
    virtual InstructionAddress programCounter() const = 0;

protected:
    std::set<InstructionAddress> exitPoints;
    std::ostringstream imemImage;
};

#endif

/* vim: set ts=4 expandtab: */


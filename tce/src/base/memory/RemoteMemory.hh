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
 * @file RemoteMemory.hh
 * 
 * Declaration of RemoteMemory class.
 *
 * @author Kalle Raiskila 2013
 */

#ifndef TTA_REMOTE_MEMORY_H
#define TTA_REMOTE_MEMORY_H

#include "Memory.hh"
#include "RemoteController.hh"
#include "AddressSpace.hh"

using TTAMachine::AddressSpace;

/**
 * RemoteMemory is a proxy class for a physical memory.
 * 
 * This class adapts a physical memory on a FPGA or ASIC, 
 * as seen through a debug interface, to the main TCE system.
 * The RemoteMemory is used only in the Simulator parts of TCE.
 * All accesses to the physical memories RemoteMemory objects do
 * go through the RemoteController that implements the debugging
 * interface.
 */
class RemoteMemory : public Memory {
public:
	// TODO: hoist AddressSpace-based constructor to parent class?
	RemoteMemory(const AddressSpace &space, bool littleEndian) :
        Memory(space.start(), space.end(), space.width(), littleEndian), 
    controller_(NULL), addressspace_(space) {}

	/** 
     * Set SimulationController for memory.
     *
	 * The RemoteMemory accesses physical memories via this controller object.
	 */
	void setController(RemoteController* con) {
		controller_ = con;
	}

	// overload the pure viruals of Memory
	virtual void write(Word address, MAU data);
	virtual Memory::MAU read(Word address);
	
private:
	RemoteController* controller_;
	const AddressSpace& addressspace_;

};

#endif 

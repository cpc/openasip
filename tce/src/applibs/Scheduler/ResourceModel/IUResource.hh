/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file IUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the IUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IURESOURCE_HH
#define TTA_IURESOURCE_HH

#include <string>
#include <vector>
#include <memory>

#include "SchedulingResource.hh"

namespace TTAProgram {
    class Terminal;
    class TerminalImmediate;
}

namespace TTAMachine {
    class Machine;
}

/**
 * An interface for scheduling resources of Resource Model
 * The derived class IUResource
 */

class IUResource : public SchedulingResource {
public:
    virtual ~IUResource();
    IUResource(
        const TTAMachine::Machine& mach,
        const std::string& name,
        const int registers,
        const int width,
        const int latency,
        const bool signExtension,
        unsigned int initiationInterval = 0);

    virtual bool isInUse(const int cycle) const override;
    virtual bool isAvailable(const int cycle, int immRegIndex) const;
    virtual bool isAvailable(const int cycle) const override;
    virtual bool canAssign(const int, const MoveNode&)const override;
    virtual bool canAssign(
        const int defCycle,
        const int useCycle,
        const MoveNode& node,
	int immRegIndex) const;
    virtual void assign(const int cycle, MoveNode& node) override;
    virtual void assign(
        const int defCycle, const int useCycle, MoveNode& node, int& index);
    virtual void unassign(const int cycle, MoveNode& node);
    virtual bool isIUResource() const;

    int registerCount() const;
    std::shared_ptr<TTAProgram::TerminalImmediate>
    immediateValue(const MoveNode& node) const;
    int immediateWriteCycle(const MoveNode& node) const;
    int width() const;

    void clearOldResources();
    void clear();
    
protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();
    void setRegisterCount(const int registers);

private:
    // Stores first and last cycle register is marked for use
    // also the actual value of constant to be stored in register
    struct ResourceRecordType{
        ResourceRecordType(int definition, int use, TTAProgram::TerminalImmediate* val);
        int         definition_;
        int         use_;
        std::shared_ptr<TTAProgram::TerminalImmediate> immediateValue_;
    };
    // For each register, there are several def-use combinations
    // non overlapping
    typedef std::vector<ResourceRecordType*> ResourceRecordVectorType;
    // Copying forbidden
    IUResource(const IUResource&);
    // Assignment forbidden
    IUResource& operator=(const IUResource&);
    int findAvailable(
        const int defCycle, const int useCycle, int immRegIndex) const;

    std::vector<ResourceRecordVectorType> resourceRecord_;
    // Number of registers in given IU
    int registerCount_;
    // The bit width of registers in IU
    int width_;
    // The latency of IU
    int latency_;
    //Extention of IU, true == sign extends, false == zero extends
    bool signExtension_;

    const TTAMachine::Machine& machine_;
};
#endif

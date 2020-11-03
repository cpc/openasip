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
 * @file NullInstructionTemplate.hh
 *
 * Declaration of NullInstructionTemplate class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_INSTRUCTION_TEMPLATE_HH
#define TTA_NULL_INSTRUCTION_TEMPLATE_HH

#include "InstructionTemplate.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null InstructionTemplate.
 *
 * All the methods abort the program.
 */
class NullInstructionTemplate : public InstructionTemplate {
public:
    static NullInstructionTemplate& instance();

    virtual TCEString name() const;
    virtual void setName(const std::string& name);

    virtual void addSlot(
        const std::string& slotName, int width, ImmediateUnit& dstUnit);
    virtual void removeSlot(const std::string& slotName);
    virtual void removeSlots(const ImmediateUnit& dstUnit);

    virtual int slotCount() const;
    virtual TemplateSlot* slot(int index) const;

    virtual bool usesSlot(const std::string& slotName) const;
    virtual bool destinationUsesSlot(
        const std::string& slotName,
        const ImmediateUnit& dstUnit) const;
    virtual int numberOfDestinations() const;
    virtual bool isOneOfDestinations(const ImmediateUnit& dstUnit) const;
    virtual ImmediateUnit* destinationOfSlot(const std::string& slotName) const;

    virtual int numberOfSlots(const ImmediateUnit& dstUnit) const;
    virtual std::string slotOfDestination(
        const ImmediateUnit& dstUnit, int index) const;

    virtual int supportedWidth() const;
    virtual int supportedWidth(const ImmediateUnit& dstUnit) const;
    virtual int supportedWidth(const std::string& slotName) const;

    virtual bool isEmpty() const;

    virtual void setMachine(Machine& machine);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const;
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

private:
    NullInstructionTemplate();
    virtual ~NullInstructionTemplate();

    /// The only instance of NullInstructionTemplate.
    static NullInstructionTemplate instance_;
    /// Machine to which the null instruction template is registered.
    static Machine machine_;
};
}

#endif

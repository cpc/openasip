/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file NullInstructionTemplate.hh
 *
 * Declaration of NullInstructionTemplate class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual void addSlot(
        const std::string& slotName,
        int width,
        ImmediateUnit& dstUnit)
        throw (InstanceNotFound, ComponentAlreadyExists, OutOfRange);
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
    virtual ImmediateUnit* destinationOfSlot(
        const std::string& slotName) const
        throw (InstanceNotFound);

    virtual int numberOfSlots(const ImmediateUnit& dstUnit) const;
    virtual std::string slotOfDestination(
        const ImmediateUnit& dstUnit, 
        int index) const
        throw (OutOfRange);

    virtual int supportedWidth() const;
    virtual int supportedWidth(const ImmediateUnit& dstUnit) const;
    virtual int supportedWidth(const std::string& slotName) const;

    virtual bool isEmpty() const;

    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

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

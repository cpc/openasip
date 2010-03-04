/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file Guard.hh
 *
 * Declaration of Guard class and its derived classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_GUARD_HH
#define TTA_GUARD_HH

#include "MachinePart.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class FUPort;
class Bus;
class RegisterFile;

/////////////////////////////////////////////////////////////////////////////
// Guard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard expression representing an execution predicate.
 *
 * Evaluates to true (execute) or false (don't execute). This is a
 * base class for real Guards.
 */
class Guard : public SubComponent {
public:
    virtual ~Guard();

    virtual Bus* parentBus() const;
    virtual void setParentBus(Bus& parentBus) {
        parent_ = &parentBus;
    }
    virtual bool isEqual(const Guard& guard) const = 0;
    virtual bool isInverted() const;
    virtual bool isMoreRestrictive(const Guard& guard) const;
    virtual bool isLessRestrictive(const Guard& guard) const;
    virtual bool isOpposite(const Guard& guard) const = 0;
    virtual bool isDisjoint(const Guard& guard) const;
    virtual void copyTo(Bus& parentBus) const = 0;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for guard.
    static const std::string OSNAME_GUARD;
    /// ObjectState attribute key for inverted feature.
    static const std::string OSKEY_INVERTED;

protected:
    Guard(bool inverted, Bus& parentBus);
    Guard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);

private:
    /// Indicated whether the condition term is inverted.
    bool inverted_;
    /// The parent bus of the guard.
    Bus* parent_;
};


/////////////////////////////////////////////////////////////////////////////
// PortGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard where the condition term is taken from the value of an
 * output port of a FunctionUnit.
 */
class PortGuard : public Guard {
public:
    PortGuard(
        bool inverted, 
        FUPort& port, 
        Bus& parentBus)
        throw (IllegalRegistration, ComponentAlreadyExists);
    PortGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~PortGuard();

    bool isEqual(const Guard& guard) const;
    bool isOpposite(const Guard& guard) const;

    FUPort* port() const;
    virtual void copyTo(Bus& parentBus) const {
        new PortGuard(
            isInverted(), *port_, parentBus);
    }

    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for PortGuard ObjectState.
    static const std::string OSNAME_PORT_GUARD;
    /// ObjectState attribute key for function unit name.
    static const std::string OSKEY_FU;
    /// ObjectState attribute key for port name.
    static const std::string OSKEY_PORT;

private:
    /// Port from which the condition term is taken.
    FUPort* port_;
};


/////////////////////////////////////////////////////////////////////////////
// RegisterGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard where the condition term is taken from the value of a
 * register (from a RegisterFile).
 */
class RegisterGuard : public Guard {
public:
    RegisterGuard(
        bool inverted,
        RegisterFile& regFile,
        int registerIndex,
        Bus& parentBus)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange,
               InvalidData);
    RegisterGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~RegisterGuard();

    bool isOpposite(const Guard& guard) const;
    bool isEqual(const Guard& guard) const;
    RegisterFile* registerFile() const;
    int registerIndex() const;
    virtual void copyTo(Bus& parentBus) const {
        new RegisterGuard(
            isInverted(), *regFile_, registerIndex_,
            parentBus);
    }

    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for RegisterGuard.
    static const std::string OSNAME_REGISTER_GUARD;
    /// ObjectState attribute key for register file name.
    static const std::string OSKEY_REGFILE;
    /// ObjectState attribute key for register index.
    static const std::string OSKEY_INDEX;

private:
    /// RegisterFile from which the condition term is taken.
    RegisterFile* regFile_;
    /// Index of the register from which the condition term is taken.
    int registerIndex_;
};


/////////////////////////////////////////////////////////////////////////////
// UnconditionalGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Always true guard term. Always false if inverted.
 */
class UnconditionalGuard : public Guard {
public:
    UnconditionalGuard(bool inverted, Bus& parentBus)
        throw (ComponentAlreadyExists);
    UnconditionalGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~UnconditionalGuard();

    bool isOpposite(const Guard& /*guard*/) const { return false; }
    bool isEqual(const Guard& guard) const;
    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual void copyTo(Bus& parentBus) const {
        new UnconditionalGuard(isInverted(), parentBus);
    }

    /// ObjectState name for UnconditionalGuard.
    static const std::string OSNAME_UNCONDITIONAL_GUARD;
};
}

#include "Guard.icc"

#endif

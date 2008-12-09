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
 * @file FUPort.hh
 *
 * Declaration of FUPort class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#ifndef TTA_FU_PORT_HH
#define TTA_FU_PORT_HH

#include <string>

#include "BaseFUPort.hh"

namespace TTAMachine {

/**
 * Represens an operand, trigger or result port of a function unit.
 */
class FUPort : public BaseFUPort {
public:
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);
    FUPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~FUPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    void setTriggering(bool triggers);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    std::string bindingString() const;
    void updateBindingString() const;

    bool isArchitectureEqual(FUPort* port);

    /// ObjectState name for FUPort.
    static const std::string OSNAME_FUPORT;
    /// ObjectState attribute key for triggering feature.
    static const std::string OSKEY_TRIGGERING;
    /// ObjectState attribute key for operand code setting feature.
    static const std::string OSKEY_OPCODE_SETTING;

protected:
    // this is for UniversalFUPort class
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode,
        bool dummy)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);

private:
    void cleanupGuards() const;
    void cleanupOperandBindings() const;
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Specifies whether this is a triggering port.
    bool triggers_;
    /// Specifies whether this is an operation selecting port.
    bool setsOpcode_;
    /// Binding string describes the operation bindings of 
    /// of the port to allow fast binding comparison.
    mutable std::string bindingString_;
};
}

#endif

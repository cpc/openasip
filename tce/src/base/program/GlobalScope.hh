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
 * @file GlobalScope.hh
 *
 * Declaration of GlobalScope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GLOBAL_SCOPE_HH
#define TTA_GLOBAL_SCOPE_HH

#include "Scope.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

    class Program;
    class Label;

class GlobalScope : public Scope {
public:
    GlobalScope();
    virtual ~GlobalScope();

    virtual bool isGlobal() const;

    int globalCodeLabelCount(Address address) const;

    const CodeLabel& globalCodeLabel(Address address, int index) const;

    int globalCodeLabelCount() const;

    const CodeLabel& globalCodeLabel(int index) const;

    int globalDataLabelCount(Address address) const;

    const DataLabel& globalDataLabel(Address address, int index) const;

    int globalDataLabelCount() const;

    const DataLabel& globalDataLabel(int index) const;

    virtual void removeCodeLabels(InstructionAddress address);

    virtual Scope* copy() const;

    virtual Scope* copyAndRelocate(const TTAProgram::Program& program) const;

    virtual void setDataLabelAddressSpace(
        const TTAMachine::AddressSpace& space);

protected:
    virtual void addGlobalCodeLabel(
        const CodeLabel& codeLabel, const Scope& owner);
    virtual void addGlobalDataLabel(
        const DataLabel& codeLabel, const Scope& owner);

private:

    /**
     * Connects a label and its owner for bookkeeping.
     */
    class LabelOwner {
    public:
        LabelOwner(const Label& label, const Scope& owner);
        ~LabelOwner();
        const Label& label() const;
        const Scope& scope() const;
    private:
        /// The label.
        const Label* label_;
        /// The owner scope of the label.
        const Scope* owner_;
    };

    /// Copying not allowed.
    GlobalScope(const GlobalScope&);
    /// Assignment not allowed.
    GlobalScope& operator=(const GlobalScope&);

    /// List for LabelOwners.
    typedef std::vector<LabelOwner*> LabelOwnerList;

    /// Container for all code labels contained in the global scope and its
    /// child scopes.
    LabelOwnerList allCodeLabels_;
    /// Container for all data labels.
    LabelOwnerList allDataLabels_;
};

}

#endif

/**
 * @file GlobalScope.hh
 *
 * Declaration of GlobalScope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_GLOBAL_SCOPE_HH
#define TTA_GLOBAL_SCOPE_HH

#include "Label.hh"
#include "Scope.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

    class Program;

class GlobalScope : public Scope {
public:
    GlobalScope();
    virtual ~GlobalScope();

    virtual bool isGlobal() const;

    int globalCodeLabelCount(Address address) const;

    const CodeLabel& globalCodeLabel(Address address, int index) const
        throw (KeyNotFound);

    int globalCodeLabelCount() const;

    const CodeLabel& globalCodeLabel(int index) const
        throw (KeyNotFound);

    int globalDataLabelCount(Address address) const;

    const DataLabel& globalDataLabel(Address address, int index) const
        throw (KeyNotFound);

    int globalDataLabelCount() const;

    const DataLabel& globalDataLabel(int index) const
        throw (KeyNotFound);

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

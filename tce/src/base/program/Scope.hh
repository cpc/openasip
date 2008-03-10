/**
 * @file Scope.hh
 *
 * Declaration of Scope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCOPE_HH
#define TTA_SCOPE_HH

#include <string>
#include <vector>

#include "Address.hh"
#include "Exception.hh"

namespace TTAProgram {

class CodeLabel;
class DataLabel;

/**
 * Scopes provide containers for different visibility level labels.
 *
 * @todo Implement rest of the Scopes (only GlobalScope is supported 
 * currently).
 */
class Scope {
public:
    Scope();
    virtual ~Scope();

    virtual bool isGlobal() const;
    virtual bool isUnit() const;
    virtual bool isProcedure() const;
    virtual bool isLocal() const;

    bool containsCodeLabel(const std::string& name) const;
    bool containsDataLabel(const std::string& name) const;

    const CodeLabel& codeLabel(const std::string& name) const
        throw (KeyNotFound);
    const DataLabel& dataLabel(const std::string& name) const
        throw (KeyNotFound);

    int codeLabelCount(Address address) const;
    const CodeLabel& codeLabel(Address address, int index) const
        throw (OutOfRange, KeyNotFound);

    int dataLabelCount(Address address) const;
    const DataLabel& dataLabel(Address address, int index) const
        throw (OutOfRange, KeyNotFound);

    virtual void addCodeLabel(const CodeLabel* codeLabel)
        throw (KeyAlreadyExists);
    virtual void addDataLabel(const DataLabel* dataLabel)
        throw (KeyAlreadyExists);
    virtual void removeCodeLabels(InstructionAddress address);

    virtual Scope* copy() const = 0;

protected:
    /// List for child scopes.
    typedef std::vector<const Scope*> ScopeList;
    /// Child scopes.
    ScopeList children_;

    /// List of data labels.
    typedef std::vector<const DataLabel*> DataLabelList;
    /// List of code labels.
    typedef std::vector<const CodeLabel*> CodeLabelList;

    /// Data labels contained by this scope.
    DataLabelList dataLabels_;
    /// Code labels contained by this scope.
    CodeLabelList codeLabels_;

    /// Adds a code label and its owner to the global label bookkeeping.
    ///
    /// @param dataLabel The code label to be added.
    /// @param owner The owner scope of the label.
    virtual void addGlobalCodeLabel(
        const CodeLabel& codeLabel, const Scope& owner) = 0;

    /// Adds a data label and its owner to the global label bookkeeping.
    ///
    /// @param dataLabel The data label to be added.
    /// @param owner The owner scope of the label.
    virtual void addGlobalDataLabel(
        const DataLabel& dataLabel, const Scope& owner) = 0;

    Scope& parent() const throw (WrongSubclass, IllegalRegistration);
    void setParent(Scope& scope) throw (WrongSubclass);

    int childCount() const;
    void addChild(const Scope& scope) throw (ObjectAlreadyExists);
    const Scope& child(int index) const throw (OutOfRange);

private:
    /// Copying not allowed.
    Scope(const Scope&);
    /// Assignment not allowed.
    Scope& operator=(const Scope&);

    /// The smallest outer scope that contains this scope.
    Scope* parent_;
};

}

#endif

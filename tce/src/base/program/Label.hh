/**
 * @file Label.hh
 *
 * Declaration of Label class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_LABEL_HH
#define TTA_LABEL_HH

#include <string>
#include "Address.hh"

namespace TTAProgram {

class Scope;

/**
 * Marks locations of code or data area of the program.
 *
 * Labels are owned by scopes. Scopes do not provide methods to add
 * new labels, because the labels are automatically registered to a
 * scope by their constructor: a label cannot exist without its scope.
 *
 * A label must have a unique name within the owning scope. It does
 * not matter if there are other labels with the same name in outer
 * scopes, because the inner scope has precedence. As a result, all
 * labels in global scope must have a unique name.
 */
class Label {
public:
    virtual ~Label();

    std::string name() const;
    virtual Address address() const;
    const Scope& scope() const;

    void setAddress(Address address);

protected:
    Label();
    Label(const std::string& name, Address address, const Scope& scope);
    void setName(const std::string& name);
    void setScope(const Scope& scope);

private:
    /// Name of the label.
    std::string name_;
    /// Address of the location corresponding to this label.
    Address address_;
    /// Owning scope of the label.
    const Scope* scope_;
};

}

#endif

/**
 * @file OSEdOptions.hh
 *
 * Declaration of OSEdOptions class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_HH
#define TTA_OSED_OPTIONS_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

/**
 * Class that represents the OSEd options.
 *
 * Options are stored in XML file.
 */
class OSEdOptions : public Serializable {
public:
    OSEdOptions();
    virtual ~OSEdOptions();

    std::string editor() const;
    void setEditor(const std::string& editor);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* saveState() const;
private:
    /// Copying not allowed.
    OSEdOptions(const OSEdOptions&);
    /// Assignment not allowed.
    OSEdOptions& operator=(const OSEdOptions&);

    /// Editor used to edit operation behavior files.
    std::string editor_;
};

#endif

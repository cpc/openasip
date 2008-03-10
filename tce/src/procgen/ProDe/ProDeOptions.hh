/**
 * @file ProDeOptions.hh
 *
 * Declaration of class ProDeOptions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_OPTIONS_HH
#define TTA_PRODE_OPTIONS_HH

#include <string>
#include <vector>

#include "GUIOptions.hh"

/**
 * Represents the options of the editor.
 */
class ProDeOptions : public GUIOptions {
public:
    ProDeOptions();
    ProDeOptions(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ProDeOptions(const ProDeOptions& old);
    virtual ~ProDeOptions();

    int undoStackSize() const;
    void setUndoStackSize(int size);

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    ObjectState* saveState() const;

    /// ObjectState name for ProDeOptions.
    static const std::string CONFIGURATION_NAME;
    /// ObjectState attribute key for the size of the undo stack.
    static const std::string OSKEY_UNDO_STACK_SIZE;

private:
    /// Undo stack size.
    int undoStackSize_;
};

#endif

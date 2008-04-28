/**
 * @file ToolbarButton.hh
 *
 * Declaration of ToolbarButton class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TOOLBAR_BUTTON_HH
#define TTA_TOOLBAR_BUTTON_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

/**
 * This class repsesents a toolbar button. It contains information
 * which action is performed by the button and where the button is
 * placed in the toolbar. This class implements the Serializable
 * interface because toolbar button configurations are serialized in the
 * configuration file.
 */  
class ToolbarButton : public Serializable {
public:
    /// ObjectState name for ToolbarButton.
    static const std::string OSNAME_TOOLBAR_BUTTON;
    /// ObjectState attribute key for slot position.
    static const std::string OSKEY_SLOT;
    /// ObjectState attribute key for action name.
    static const std::string OSKEY_ACTION;

    ToolbarButton(int slot, const std::string& action);
    ToolbarButton(const ObjectState* state);
    ToolbarButton(const ToolbarButton& old);
    virtual ~ToolbarButton();

    std::string action() const;
    int slot() const;

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* saveState() const;

private:
    /// Number of the slot in which this toolbar button is.
    int slot_;
    /// Name of the action performed by this toolbar button.
    std::string action_;
};

#endif

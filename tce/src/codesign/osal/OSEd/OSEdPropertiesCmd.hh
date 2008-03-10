/**
 * @file OSEdPropertiesCmd.hh
 *
 * Declaration of OSEdPropertiesCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_PROPERTIES_CMD_HH
#define TTA_OSED_PROPERTIES_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Executes the command when operation is modified.
 */
class OSEdPropertiesCmd : public GUICommand {
public:
    OSEdPropertiesCmd();
    virtual ~OSEdPropertiesCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdPropertiesCmd(const OSEdPropertiesCmd&);
    /// Assignment not allowed.
    OSEdPropertiesCmd& operator=(const OSEdPropertiesCmd&);
};

#endif

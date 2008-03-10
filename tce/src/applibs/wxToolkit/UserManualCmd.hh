/**
 * @file UserManualCmd.hh
 *
 * Declaration of UserManualCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_USER_MANUAL_CMD_HH
#define TTA_USER_MANUAL_CMD_HH

#include "GUICommand.hh"

/**
 * wxCommand for displaying the TCE manual in the system's default PDF viewer.
 */
class UserManualCmd : public GUICommand {
public:
    UserManualCmd();
    virtual ~UserManualCmd();
    virtual bool Do();
    virtual int id() const;
    virtual UserManualCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();

    /// Command name string.
    static const std::string COMMAND_NAME;
    /// Short version of the command name string.
    static const std::string COMMAND_SHORT_NAME;
    /// Command icon file name.
    static const std::string COMMAND_ICON;
    /// Command ID.
    static const int COMMAND_ID;

private:
    bool askFromUser();

    /// Tells if the singleton mime types manager has already been initialized.
    static bool mimeTypesManagerInitialized_;
};

#endif

/**
 * @file CommandRegistry.hh
 *
 * Declaration of CommandRegistry class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#ifndef TTA_COMMAND_REGISTRY_HH
#define TTA_COMMAND_REGISTRY_HH

#include <vector>
#include <string>
#include "Exception.hh"

class GUICommand;

/**
 * List of editor commands which can be added to the toolbar or executed
 * using a keyboard shortcut.
 */
class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();
    void addCommand(GUICommand* command);
    GUICommand* createCommand(const int id);
    GUICommand* createCommand(const std::string name);
    GUICommand* firstCommand();
    GUICommand* nextCommand();

    std::string commandName(int id) const
        throw (InstanceNotFound);
    std::string commandShortName(const std::string name) const
        throw(InstanceNotFound);
    int commandId(const std::string name) const;
    std::string commandIcon(const std::string name) const
        throw(InstanceNotFound);
    bool isEnabled(const std::string command)
        throw(InstanceNotFound);

private:
    /// Commands in the registry.
    std::vector<GUICommand*> commands_;
    /// The position of the iteration. Used by the firstCommand() and
    /// nextCommand().
    std::vector<GUICommand*>::iterator iterator_;
};

#include "CommandRegistry.icc"

#endif

/**
 * @file QuitCmd.hh
 *
 * Declaration of QuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_QUIT_CMD_HH
#define TTA_QUIT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for quiting the editor.
 */
class QuitCmd : public EditorCommand {
public:
    QuitCmd();
    virtual ~QuitCmd();
    virtual bool Do();
    virtual int id() const;
    virtual QuitCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

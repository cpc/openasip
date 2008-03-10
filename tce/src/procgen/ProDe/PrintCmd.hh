/**
 * @file PrintCmd.hh
 *
 * Declaration of PrintCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_PRINT_CMD_HH
#define TTA_PRINT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for printing documents.
 */
class PrintCmd : public EditorCommand {
public:
    PrintCmd();
    virtual ~PrintCmd();
    virtual bool Do();
    virtual int id() const;
    virtual PrintCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};
#endif

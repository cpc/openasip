/**
 * @file CloseDocumentCmd.hh
 *
 * Declaration of CloseDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_CLOSE_DOCUMENT_CMD_HH
#define TTA_CLOSE_DOCUMENT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for closing documents.
 */
class CloseDocumentCmd : public EditorCommand {
public:
    CloseDocumentCmd();
    virtual ~CloseDocumentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual CloseDocumentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

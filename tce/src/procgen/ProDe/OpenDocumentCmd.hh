/**
 * @file OpenDocumentCmd.hh
 *
 * Declaration of OpenDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_OPEN_DOCUMENT_CMD_HH
#define TTA_OPEN_DOCUMENT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for opening documents.
 */
class OpenDocumentCmd : public EditorCommand {
public:
    OpenDocumentCmd();
    virtual ~OpenDocumentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual OpenDocumentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

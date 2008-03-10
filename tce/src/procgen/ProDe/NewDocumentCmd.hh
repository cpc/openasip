/**
 * @file NewDocumentCmd.hh
 *
 * Declaration of NewDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_NEW_DOCUMENT_CMD_HH
#define TTA_NEW_DOCUMENT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for opening documents.
 */
class NewDocumentCmd : public EditorCommand {
public:
    NewDocumentCmd();
    virtual ~NewDocumentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual NewDocumentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

/**
 * @file SaveDocumentCmd.hh
 *
 * Declaration of SaveDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_SAVE_DOCUMENT_CMD_HH
#define TTA_SAVE_DOCUMENT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for opening documents.
 */
class SaveDocumentCmd : public EditorCommand {
public:
    SaveDocumentCmd();
    virtual ~SaveDocumentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SaveDocumentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

/**
 * @file SaveDocumentAsCmd.hh
 *
 * Declaration of SaveDocumentAsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_SAVE_DOCUMENT_AS_CMD_HH
#define TTA_SAVE_DOCUMENT_AS_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for saving documents with a new name.
 */
class SaveDocumentAsCmd : public EditorCommand {
public:
    SaveDocumentAsCmd();
    virtual ~SaveDocumentAsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SaveDocumentAsCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif

/**
 * @file PrintPreviewCmd.hh
 *
 * Declaration of PrintPreviewCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_PRINT_PREVIEW_CMD_HH
#define TTA_PRINT_PEWVIEW_CMD_HH

#include <string>

#include "EditorCommand.hh"

/**
 * EditorCommand for previewing print of documents.
 */
class PrintPreviewCmd : public EditorCommand {
public:
    PrintPreviewCmd();
    virtual ~PrintPreviewCmd();
    virtual bool Do();
    virtual int id() const;
    virtual PrintPreviewCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};
#endif

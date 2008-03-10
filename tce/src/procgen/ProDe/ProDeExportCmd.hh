/**
 * @file ProDeExportCmd.hh
 *
 * Declaration of ProDeExportCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_EXPORT_CMD_HH
#define TTA_PRODE_EXPORT_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for exporting machine visualization to an encapsulated
 * postscript file or a bitmap.
 */
class ProDeExportCmd : public EditorCommand {
public:
    ProDeExportCmd();
    virtual ~ProDeExportCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ProDeExportCmd* create() const;
    virtual bool isEnabled();
};

#endif

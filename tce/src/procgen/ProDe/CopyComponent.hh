/**
 * @file CopyComponent.hh
 *
 * Declaration of CopyComponent class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_COPY_COMPONENT_HH
#define TTA_COPY_COMPONENT_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for copying components to the clipboard.
 */
class CopyComponent: public ComponentCommand {
public:
    CopyComponent(EditPart* editPart);
    ~CopyComponent();

    bool Do();
    bool Undo();

private:
    /// Component to copy.
    EditPart* editPart_;
};
#endif

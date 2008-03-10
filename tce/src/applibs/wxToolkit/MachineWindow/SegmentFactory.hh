/**
 * @file SegmentFactory.hh
 *
 * Declaration of SegmentFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_SEGMENT_FACTORY_HH
#define TTA_SEGMENT_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating segment EditParts corresponding to a given
 * Machine Segment object.
 */
class SegmentFactory : public EditPartFactory {
public:
    SegmentFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~SegmentFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    SegmentFactory& operator=(SegmentFactory& old);
    /// Copying not allowed.
    SegmentFactory(SegmentFactory& old);
};

#endif

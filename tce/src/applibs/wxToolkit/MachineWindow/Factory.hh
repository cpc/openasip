/**
 * @file Factory.hh
 *
 * Declaration of Factory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_FACTORY_HH
#define TTA_FACTORY_HH

class EditPart;

namespace TTAMachine {
    class MachinePart;
}

/**
 * Interface that will be implemented by different EditPart factories.
 */
class Factory {
public:
    virtual ~Factory();
    /// Returns an EditPart that corresponds to a Machine component.
    virtual EditPart* createEditPart(TTAMachine::MachinePart* component) = 0;
    
protected:
    Factory();

private:
    /// Assignment not allowed.
    Factory& operator=(Factory& old);
    /// Copying not allowed.
    Factory(Factory& old);
};

#endif

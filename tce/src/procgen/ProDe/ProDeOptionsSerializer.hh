/**
 * @file ProDeOptionsSerializer.hh
 *
 * Declaration of ProDeOptionsSerializer class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef PRODE_OPTIONS_SERIALIZER_HH
#define PRODE_OPTIONS_SERIALIZER_HH

#include <string>

#include "GUIOptionsSerializer.hh"
#include "ProDeOptions.hh"

/**
 * Reads/writes ProDe options from/to an XML file.
 */
class ProDeOptionsSerializer : public GUIOptionsSerializer {
public:
    ProDeOptionsSerializer();
    virtual ~ProDeOptionsSerializer();
private:
    ObjectState* convertToConfigFileFormat(const ObjectState* options) const;
    ObjectState* convertToOptionsObjectFormat(const ObjectState* root) const;

    /// Copying not allowed.
    ProDeOptionsSerializer(const ProDeOptionsSerializer&);
    /// Assignment not allowed.
    ProDeOptionsSerializer& operator=(const ProDeOptionsSerializer&);
};

#endif

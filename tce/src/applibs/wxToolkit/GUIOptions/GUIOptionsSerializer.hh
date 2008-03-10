/**
 * @file GUIOptionsSerializer.hh
 *
 * Declaration of GUIOptionsSerializer class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef GUI_OPTIONS_SERIALIZER_HH
#define GUI_OPTIONS_SERIALIZER_HH

#include <string>

#include "XMLSerializer.hh"
#include "Exception.hh"
#include "GUIOptions.hh"

/**
 * Reads and writes options of a graphical user interface from an XML file.
 *
 * This is a base class which provides storing of options common to all
 * GUIs, such as window size and position, toolbar contents and list of
 * keyboard shortcuts. If a GUI needs application specific options,
 * a custom options serializer can be derived from this class. See design
 * documentation for details.
 */
class GUIOptionsSerializer : public XMLSerializer {
public:
    GUIOptionsSerializer(std::string configurationName);
    virtual ~GUIOptionsSerializer();

    void writeState(const ObjectState* optionsState)
        throw (SerializerException);
    ObjectState* readState()
        throw (SerializerException);
    void writeOptions(const GUIOptions& options)
        throw (SerializerException);
    GUIOptions* readOptions()
        throw (SerializerException, ObjectStateLoadingException);

protected:
    virtual ObjectState* convertToConfigFileFormat(
        const ObjectState* options) const;
    virtual ObjectState* convertToOptionsObjectFormat(
        const ObjectState* root) const;

private:
    void setWindowProperties(
        const ObjectState* windowElem,
        ObjectState* options) const;
    void setToolbarProperties(
        const ObjectState* toolbarElem,
        ObjectState* options) const;
    void addKeyboardShortcut(
        const ObjectState* ksElem,
        ObjectState* options) const;

    /// Copying not allowed.
    GUIOptionsSerializer(const GUIOptionsSerializer&);
    /// Assignment not allowed.
    GUIOptionsSerializer& operator=(const GUIOptionsSerializer&);
    /// Name of the configuration to read/write.
    std::string configurationName_;
};

#endif

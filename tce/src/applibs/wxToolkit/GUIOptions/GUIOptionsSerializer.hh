/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file GUIOptionsSerializer.hh
 *
 * Declaration of GUIOptionsSerializer class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
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

/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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

class GUIOptions;

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

    void writeState(const ObjectState* optionsState);
    ObjectState* readState();
    void writeOptions(const GUIOptions& options);
    GUIOptions* readOptions();

    GUIOptionsSerializer(const GUIOptionsSerializer&) = delete;
    GUIOptionsSerializer& operator=(const GUIOptionsSerializer&) = delete;

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

    /// Name of the configuration to read/write.
    std::string configurationName_;
};

#endif

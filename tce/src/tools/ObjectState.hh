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
 * @file ObjectState.hh
 *
 * Declaration of ObjectState class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 8 Jun 2004 by tr, jm, am, ll
 * @note rating: red
 */

#ifndef TTA_OBJECT_STATE_HH
#define TTA_OBJECT_STATE_HH

#include <string>
#include <vector>

#include <xercesc/dom/DOMNode.hpp>

#include "Exception.hh"

/**
 * Class ObjectState represents state of an object.
 *
 * It may contain attribute-value pairs of the object and a main
 * value. It may be used as a node in tree structure or alone. If a
 * class is going to be serialized to a file, it must be able to
 * create an ObjectState object of itself. It must be able to create
 * itself from an ObjectState object too. That is, it must implement
 * the Serializable interface.  ObjectState tree is a general
 * structure which can be handled by many different kind of
 * serializers, for example XMLSerializer. Serializers can read a file
 * create an ObjectState tree according to it and they can write a
 * file of a certain format according to a ObjectState
 * tree. ObjectState enables an easy way to serialize an object model
 * to different kinds of file formats.
 */
class ObjectState {
public:
    /// Struct for describing an attribute of the XML element.
    struct Attribute {
        std::string name;  ///< Name of the attribute.
        std::string value; ///< Value of the attribute.
    };

    ObjectState(const std::string& name);
    ObjectState(const ObjectState& old);
    ~ObjectState();

    ObjectState* parent() const;
    std::string name() const;
    void setName(const std::string& name);

    void setValue(const std::string& value);
    void setValue(int value);
    void setValue(double value);
    void setValue(bool value);

    std::string stringValue() const;
    int intValue() const
        throw (NumberFormatException);
    double doubleValue() const
        throw (NumberFormatException);
    bool boolValue() const
        throw (TypeMismatch);

    void setAttribute(const std::string& name, const std::string& value);
    void setAttribute(const std::string& name, int value);
    void setAttribute(const std::string& name, unsigned int value);
    void setAttribute(const std::string& name, double value);
    void setAttribute(const std::string& name, bool value);

    int attributeCount() const;
    Attribute* attribute(int index) const
        throw (OutOfRange);

    bool hasAttribute(const std::string& name) const;
    std::string stringAttribute(const std::string& name) const
        throw (KeyNotFound);
    int intAttribute(const std::string& name) const
        throw (KeyNotFound, NumberFormatException);
    unsigned int unsignedIntAttribute(const std::string& name) const
        throw (KeyNotFound, NumberFormatException);
    double doubleAttribute(const std::string& name) const
        throw (KeyNotFound, NumberFormatException);
    bool boolAttribute(const std::string& name) const
        throw (KeyNotFound, TypeMismatch);

    bool hasChild(const std::string& name) const;
    void addChild(ObjectState* child);
    void removeChild(ObjectState* child)
        throw (InstanceNotFound);
    void replaceChild(ObjectState* old, ObjectState* newChild)
        throw (InvalidData);

    int childCount() const;
    ObjectState* childByName(const std::string& name) const
        throw (InstanceNotFound);
    ObjectState* child(int index) const
        throw (OutOfRange);

    bool operator!=(const ObjectState& object);

private:
    /// Table of child ObjectState.
    typedef std::vector<ObjectState*> ChildTable;
    /// Table of attributes.
    typedef std::vector<Attribute> AttributeTable;

    /// Assingment forbidden.
    ObjectState& operator=(const ObjectState& old);
    std::string commonErrorMessage() const;

    /// Name of the element.
    std::string name_;

    /// The value of a leaf element.
    std::string value_;

    /// The parent element.
    ObjectState* parent_;

    /// The child elements.
    ChildTable children_;

    /// Contains all the attributes of the element.
    AttributeTable attributes_;
};

#include "ObjectState.icc"

#endif

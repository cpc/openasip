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
 * @file XMLSerializer.hh
 *
 * Declaration of XMLSerializer class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 8 Jun 2004 by tr, jm, am, ll
 * @note rating: red
 */

#ifndef TTA_XML_SERIALIZER_HH
#define TTA_XML_SERIALIZER_HH

#include <string>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMBuilder.hpp>

#include "Serializable.hh"
#include "ObjectState.hh"
#include "Serializer.hh"
#include "Exception.hh"

#if _XERCES_VERSION >= 20200
XERCES_CPP_NAMESPACE_USE
#endif


/**
 * This class is used to read and write XML. This is a base class for
 * different kind of XML serializers.
 */
class XMLSerializer : public Serializer {
public:
    XMLSerializer();
    virtual ~XMLSerializer();

    void setSourceFile(const std::string& fileName);
    void setSourceString(const std::string& source);

    void setDestinationFile(const std::string& fileName);
    void setDestinationString(std::string& destination);

    void setSchemaFile(const std::string& fileName);
    void setUseSchema(bool useSchema);

    virtual ObjectState* readState()
        throw (SerializerException);

    virtual void writeState(const ObjectState* rootState)
        throw (SerializerException);

protected:
    std::string sourceFile() const;

private:
    /// Copying forbidden.
    XMLSerializer(const XMLSerializer&);
    /// Assignment forbidden.
    XMLSerializer& operator=(const XMLSerializer&);

    void initializeParser();

    virtual ObjectState* readFile(const std::string& fileName)
        throw (SerializerException);

    virtual ObjectState* readString(const std::string& source)
        throw (SerializerException);

    virtual void writeFile(
        const std::string& fileName, const ObjectState* rootState)
        throw (SerializerException);

    virtual void writeString(
        std::string& target, const ObjectState* rootState)
        throw (SerializerException);

    DOMDocument* createDOMDocument(const ObjectState* state) const;
    void ensureValidStream(const std::string& fileName) const
        throw (UnreachableStream);

    DOMElement* createDOM(
        ObjectState* state,
        DOMDocument* doc) const;
    ObjectState* createState(const DOMNode* node) const;
    bool hasChildElementNodes(const DOMNode* node) const;

    /// Source file path.
    std::string sourceFile_;
    /// Destination file path.
    std::string destinationFile_;
    /// Schema file path.
    std::string schemaFile_;
    /// Indicates if xml file is validated using schema.
    bool useSchema_;
    /// The parser that checks the XML file for errors with the Schema.
    DOMBuilder* parser_;
    /// Implementation of the DOM.
    DOMImplementation* domImplementation_;

    /// Source string to read.
    const std::string* sourceString_;
    /// Destination string to write.
    std::string* destinationString_;
};

#endif


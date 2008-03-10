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


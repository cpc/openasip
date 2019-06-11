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
 * @file XMLSerializer.cc
 *
 * Implementation of XMLSerializer class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 8 Jun 2004 by tr, jm, am, ll
 * @note rating: red
 */

#include <fstream>

#include <xercesc/util/XercesVersion.hpp>

#if XERCES_VERSION_MAJOR >= 3
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#else
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#endif

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#include "XMLSerializer.hh"
#include "DOMBuilderErrorHandler.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;
using std::ifstream;

/**
 * Constructor.
 */
XMLSerializer::XMLSerializer() :
    Serializer(), sourceFile_(""), destinationFile_(""), schemaFile_(""),
    useSchema_(false), parser_(NULL), domImplementation_(NULL),
    sourceString_(NULL), destinationString_(NULL), nsUri_("") {

    XMLPlatformUtils::Initialize();

    const XMLCh gLS[] = {chLatin_L, chLatin_S, chNull};
    domImplementation_ =
        DOMImplementationRegistry::getDOMImplementation(gLS);

#if XERCES_VERSION_MAJOR >= 3
    parser_ = domImplementation_->createLSParser(
            DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#else
     parser_ = domImplementation_->createDOMBuilder(
         DOMImplementationLS::MODE_SYNCHRONOUS, 0);
#endif
}

/**
 * Destructor.
 */
XMLSerializer::~XMLSerializer() {
    delete parser_;
    XMLPlatformUtils::Terminate();
}

/**
 * Sets the source file used when readState is called.
 *
 * Previously set source file or string is unset.
 *
 * @param fileName Relative or absolute path of the source file, e.g.
 *                 /home/tce/file.xml or ./file.xml.
 */
void
XMLSerializer::setSourceFile(const std::string& fileName) {
    sourceFile_ = fileName;
    sourceString_ = NULL;
}

/**
 * Sets the source string used when readState is called.
 *
 * Previously set source file or string is unset.
 *
 * @param source Source string to read.
 */
void
XMLSerializer::setSourceString(const std::string& source) {
    sourceString_ = &source;
    sourceFile_ = "";
}

/**
 * Sets the destination file used when writeState is called.
 *
 * Previously set destination file or string is unset.
 *
 * @param fileName Relative or absolute path of the destination file,
 *                 e.g. /home/tce/file.xml or ./file.xml.
 */
void
XMLSerializer::setDestinationFile(const std::string& fileName) {
    destinationFile_ = fileName;
    destinationString_ = NULL;
}


/**
 * Sets the destination string used when writeState is called.
 *
 * Previously set destination file or string is unset.
 *
 * @param target Target string to write.
 */
void
XMLSerializer::setDestinationString(std::string& target) {
    destinationString_ = &target;
    destinationFile_ = "";
}

/**
 * Sets the schema file used to validate xml files.
 *
 * @param fileName Relative or absolute path of the schema file, e.g.
 *                 /home/tce/schema.xsd or ./schema.xsd.
 */
void
XMLSerializer::setSchemaFile(const std::string& fileName) {
    schemaFile_ = fileName;
}

/**
 * Sets/unsets validation of xml files using xml schema.
 *
 * @param useSchema True sets and false unsets validation. Default value is
 *                  false.
 */
void
XMLSerializer::setUseSchema(bool useSchema) {
    useSchema_ = useSchema;
}

/**
 * Sets the XML namespace URI to be used when creating a DOM document
 *
 */
void
XMLSerializer::setXMLNamespace(std::string nsUri) {
    nsUri_ = nsUri;
}

/**
 * Reads object state from a file or string.
 *
 * File is read if a source file is set with setSourceFile().
 * String is read if a source string is set with setSourceString().
 * Only the last file/string set will be read.
 */
ObjectState*
XMLSerializer::readState()
    throw (SerializerException) {

    if (sourceFile_ != "") {
        return readFile(sourceFile_);
    } else if (sourceString_ != NULL) {
        return readString(*sourceString_);
    } else {
        string errorMsg = "No Source file or string set.";
        throw SerializerException(__FILE__, __LINE__, __func__, errorMsg);
    }
}

/**
 * Writes the given object state to a file or string.
 *
 * File is written if a destiantion file is set with setDestinationFile().
 * String is written if a destination string is set with
 * setDestinationString(). Only the last file/string set will be written.
 */
void
XMLSerializer::writeState(const ObjectState* state)
    throw (SerializerException) {

    if (destinationFile_ != "") {
        writeFile(destinationFile_, state);
    } else if (destinationString_ != NULL) {
        writeString(*destinationString_, state);
    } else {
        string errorMsg = "No destination file or string set.";
        throw SerializerException(__FILE__, __LINE__, __func__, errorMsg);
    }
}


/**
 * Initializes the XML parser.
 */
void
XMLSerializer::initializeParser() {

    if (useSchema_ && schemaFile_ == "") {
        string errorMsg = "No schema file set.";
        throw SerializerException(__FILE__, __LINE__, __func__, errorMsg);
    }

    if (useSchema_) {

        try {
            ensureValidStream(schemaFile_);
        } catch (UnreachableStream& exception) {
            SerializerException error(__FILE__, __LINE__, __func__,
                exception.errorMessage());
            
            error.setCause(exception);            
            throw error;
        }

        string absoluteSchemaFile = schemaFile_;

        // convert the given schemaFile path to absolute path if it isn't
        if (FileSystem::isRelativePath(schemaFile_)) {
            absoluteSchemaFile = FileSystem::currentWorkingDir() +
                FileSystem::DIRECTORY_SEPARATOR + schemaFile_;
        }

        XMLCh* filePath = Conversion::toXMLCh(absoluteSchemaFile);
#if XERCES_VERSION_MAJOR >= 3
        parser_->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
        parser_->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
        parser_->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
        parser_->getDomConfig()->setParameter(
            XMLUni::fgXercesSchemaFullChecking, true);
        parser_->getDomConfig()->setParameter(
            XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
            filePath);
#else
        parser_->setFeature(XMLUni::fgXercesSchema, true);
        parser_->setFeature(XMLUni::fgDOMValidation, true);
        parser_->setFeature(XMLUni::fgDOMNamespaces, true);
        parser_->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
        parser_->setProperty(
            XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
            filePath);
#endif

        XMLString::release(&filePath);
    }
#if XERCES_VERSION_MAJOR >= 3
    parser_->getDomConfig()->setParameter(
        XMLUni::fgDOMElementContentWhitespace, false);
    parser_->getDomConfig()->setParameter(XMLUni::fgDOMComments, false);
#else
    parser_->setFeature(XMLUni::fgDOMWhitespaceInElementContent, false);
    parser_->setFeature(XMLUni::fgDOMComments, false);
#endif    
}

/**
 * Reads object state from an xml string.
 *
 * @param XML to read as an string containing the entire xml document.
 * @return Root node of the created ObjectState tree.
 * @exception SerializerException If an error occurs while reading.
 */
ObjectState*
XMLSerializer::readString(const std::string& source)
    throw (SerializerException) {

    initializeParser();
    DOMDocument* dom = NULL;
    DOMBuilderErrorHandler* errHandler = new DOMBuilderErrorHandler();
#if XERCES_VERSION_MAJOR >= 3
    parser_->getDomConfig()->setParameter(
        XMLUni::fgDOMErrorHandler, errHandler);
#else    
    parser_->setErrorHandler(errHandler);
#endif

    // build the DOM object
    try {
        parser_->resetDocumentPool();

        MemBufInputSource* buf = new MemBufInputSource(
            (const XMLByte*)source.c_str(),
            source.length(),
            "sourceXML",
            false);

        Wrapper4InputSource* domBuf = new Wrapper4InputSource(buf);
#if XERCES_VERSION_MAJOR >= 3
        dom = parser_->parse(domBuf);
#else        
        dom = parser_->parse(*domBuf);
#endif        
        delete domBuf;
    } catch(...) {
        string errorLog = errHandler->errorLog();
        delete errHandler;
        throw SerializerException(__FILE__, __LINE__, __func__, errorLog);
    }

    int errors(errHandler->errorCount());

    if (errors > 0) {
        // if there were such errors in xml file or schema file that
        // exception was not thrown when parseURI was called
        string errorLog = errHandler->errorLog();
        delete errHandler;
        throw SerializerException(__FILE__, __LINE__, __func__, errorLog);
    }

    if (dom == NULL || dom->getDocumentElement() == NULL) {
        delete errHandler;
        throw SerializerException(
	        __FILE__, __LINE__, __func__, "Illegal file: " + source);
    }
    ObjectState* rootState = createState(dom->getDocumentElement());
    delete errHandler;
    errHandler = NULL;

    return rootState;
}


/**
 * Reads current XML file set and creates an ObjectState tree according to
 * it.
 *
 * The XML file is validated with the current schema file if the setting
 * is on.
 *
 * @return Root node of the created ObjectState tree.
 * @exception SerializerException If an error occurs while reading.
 */
ObjectState*
XMLSerializer::readFile(const std::string& sourceFile)
    throw (SerializerException) {

    initializeParser();
    DOMDocument* dom = NULL;

    if (sourceFile == "") {
        string errorMsg = "No source file set.";
        throw SerializerException(__FILE__, __LINE__, __func__, errorMsg);
    }

    // check validity of file stream
    try {
        ensureValidStream(sourceFile);
    } catch (UnreachableStream& exception) {
        throw SerializerException(__FILE__, __LINE__, __func__,
            exception.errorMessage());
    }

    DOMBuilderErrorHandler* errHandler = new DOMBuilderErrorHandler();
#if XERCES_VERSION_MAJOR >= 3
    parser_->getDomConfig()->setParameter(
        XMLUni::fgDOMErrorHandler, errHandler);
#else    
    parser_->setErrorHandler(errHandler);
#endif

    try {
        parser_->resetDocumentPool();
        dom = parser_->parseURI(sourceFile.c_str());
    } catch (const ErrorInExternalFile& exception) {

        string errorLog = errHandler->errorLog();
        delete errHandler;

        throw SerializerException(__FILE__, __LINE__, __func__, errorLog);
    }

    if (dom == NULL || dom->getDocumentElement() == NULL) {
        delete errHandler;
        throw SerializerException(
	        __FILE__, __LINE__, __func__, "Illegal file: " + sourceFile);

    }


    int errors(errHandler->errorCount());

    if (errors > 0) {
        // if there were such errors in xml file or schema file that
        // exception was not thrown when parseURI was called
        string errorLog = errHandler->errorLog();
        delete errHandler;
        throw SerializerException(__FILE__, __LINE__, __func__, errorLog);
    }

    ObjectState* rootState = createState(dom->getDocumentElement());

    delete errHandler;
    return rootState;
}


/**
 * Writes the given ObjectState tree into the current XML file set.
 *
 * @param rootState The root object of the ObjectState tree.
 * @exception SerializerException If the destination file cannot be written.
 */
void
XMLSerializer::writeFile(
    const std::string& destinationFile, const ObjectState* rootState)
    throw (SerializerException) {
    
#if XERCES_VERSION_MAJOR >= 3
    DOMLSSerializer* domWriter = domImplementation_->createLSSerializer();
    domWriter->getDomConfig()->setParameter(
        XMLUni::fgDOMWRTFormatPrettyPrint, true);
    DOMLSOutput* lsOutput = domImplementation_->createLSOutput();
#else
    DOMWriter* domWriter = domImplementation_->createDOMWriter();
    domWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
#endif
    DOMDocument* document = createDOMDocument(rootState);

    try {
	if (!FileSystem::fileIsCreatable(destinationFile) &&
	    !FileSystem::fileIsWritable(destinationFile)) {
	      throw "";
        }
#if XERCES_VERSION_MAJOR >= 3
        LocalFileFormatTarget targetFile(destinationFile.c_str());
        lsOutput->setByteStream(&targetFile);
        domWriter->write(document, lsOutput);
#else        
        LocalFileFormatTarget targetFile(destinationFile.c_str());
        domWriter->writeNode(&targetFile, *document);
#endif        
        domWriter->release();
        delete document;
    } catch (...) {
        domWriter->release();
        delete document;
        string errorMessage = "Cannot write to " + destinationFile;
        throw SerializerException(__FILE__, __LINE__, __func__,
            errorMessage);
    }
}

/**
 * Writes the given ObjectState tree to a target string.
 *
 * @param rootState The root object of the ObjectState tree.
 * @exception SerializerException If the destination file cannot be written.
 */
void
XMLSerializer::writeString(
    std::string& target, const ObjectState* rootState)
    throw (SerializerException) {
    
#if XERCES_VERSION_MAJOR >= 3
    DOMLSSerializer* domWriter = domImplementation_->createLSSerializer();
    domWriter->getDomConfig()->setParameter(
        XMLUni::fgDOMWRTFormatPrettyPrint, true);
    DOMLSOutput* lsOutput = domImplementation_->createLSOutput();
#else
    DOMWriter* domWriter = domImplementation_->createDOMWriter();
    domWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
#endif
    DOMDocument* document = createDOMDocument(rootState);

    try {
        target.clear();
#if XERCES_VERSION_MAJOR >= 3
        MemBufFormatTarget* buf = new MemBufFormatTarget();
        lsOutput->setByteStream(buf);
        domWriter->write(document, lsOutput);
#else        
        MemBufFormatTarget* buf = new MemBufFormatTarget();
        domWriter->writeNode(buf, *document);
#endif        
        target.append((char*)(buf->getRawBuffer()));
        domWriter->release();
        delete buf;
        delete document;
    } catch (...) {
        domWriter->release();
        delete document;
        string errorMessage = "Error writing xml to a string.";
        throw SerializerException(__FILE__, __LINE__, __func__,
            errorMessage);
    }
}

/**
 * Returns the source file that is set.
 *
 * @return The source file.
 */
std::string
XMLSerializer::sourceFile() const {
    return sourceFile_;
}

/**
 * Creates a DOM tree according to the given ObjectState tree.
 *
 * @param state Root node of the ObjectState tree.
 * @return The created DOM tree.
 */
DOMDocument*
XMLSerializer::createDOMDocument(const ObjectState* state) const {

    XMLCh* nsUri = NULL;
    if (!nsUri_.empty()) {
        nsUri = Conversion::toXMLCh(nsUri_);
    }
    XMLCh* docName = Conversion::toXMLCh(state->name());
    DOMDocument* doc = domImplementation_->createDocument(nsUri, docName, 0);
    XMLString::release(&docName);
    if (nsUri != NULL) {
        XMLString::release(&nsUri);
        nsUri = NULL;
    }
    DOMElement* rootElem = doc->getDocumentElement();

    for (int i = 0; i < state->attributeCount(); i++) {
        ObjectState::Attribute* attribute = state->attribute(i);
        XMLCh* attribName = Conversion::toXMLCh(attribute->name);
        XMLCh* attribValue = Conversion::toXMLCh(attribute->value);
        rootElem->setAttribute(attribName, attribValue);
        XMLString::release(&attribName);
        XMLString::release(&attribValue);
    }

    string value = state->stringValue();
    if (value != "") {
        XMLCh* nodeValue = Conversion::toXMLCh(value);
        DOMText* valueNode = doc->createTextNode(nodeValue);
        rootElem->appendChild(valueNode);
        XMLString::release(&nodeValue);
    }

    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* childState = state->child(i);
        DOMElement* childElement = createDOM(childState, doc);
        rootElem->appendChild(childElement);
    }

    return doc;
}



/**
 * Checks that the file is OK for reading.
 *
 * @param fileName Name of the XML file.
 * @exception UnreachableStream If the given file cannot be read.
 */
void
XMLSerializer::ensureValidStream(const std::string& fileName) const
    throw (UnreachableStream) {

    ifstream filestream;
    filestream.open(fileName.c_str());
    if(!filestream.is_open() || !filestream.good()) {
        filestream.close();
        string procName = "XMLSerializer::ensureValidStream";
        string errorMessage = "Cannot open file \'";
        errorMessage += fileName;
        errorMessage += "\'.";
        throw UnreachableStream(__FILE__, __LINE__, procName, errorMessage);
    }
    filestream.close();
}


/**
 * Creates DOM tree from the given ObjectState tree.
 *
 * @param state Root node of the ObjectState tree.
 * @param doc The DOMDocument to which the created DOM tree
 *            belongs.
 */
DOMElement*
XMLSerializer::createDOM(
    ObjectState* state,
    DOMDocument* doc) const {

    XMLCh* elemName = Conversion::toXMLCh(state->name());
    DOMElement* rootElem =
        doc->createElement(elemName);
    XMLString::release(&elemName);

    for (int i = 0; i < state->attributeCount(); i++) {
        ObjectState::Attribute* attribute = state->attribute(i);
        XMLCh* attribName = Conversion::toXMLCh(attribute->name);
        XMLCh* attribValue = Conversion::toXMLCh(attribute->value);
        rootElem->setAttribute(attribName, attribValue);
        XMLString::release(&attribName);
        XMLString::release(&attribValue);
    }

    if (state->childCount() == 0) {
        if (state->stringValue() != "") {
            XMLCh* text = Conversion::toXMLCh(state->stringValue());
            DOMText* value = doc->createTextNode(text);
            rootElem->appendChild(value);
            XMLString::release(&text);
        }
    }

    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* childState = state->child(i);
        rootElem->appendChild(createDOM(childState, doc));
    }

    return rootElem;
}


/**
 * Creates a one-to-one ObjectState object (tree) according to the given
 * DOMNode.
 *
 * @param node DOMNode from which the ObjectState is created.
 * @return The created ObjectState instance (tree).
 */
ObjectState*
XMLSerializer::createState(const DOMNode* node) const {
    ObjectState* state =
        new ObjectState(Conversion::XMLChToString(node->getNodeName()));
    DOMNamedNodeMap* attributes = node->getAttributes();

    // set the attributes
    if (attributes != NULL) {
        for (unsigned int i = 0; i < attributes->getLength(); i++) {
            DOMNode* attribute = attributes->item(i);
            state->setAttribute(
                Conversion::XMLChToString(attribute->getNodeName()),
                Conversion::XMLChToString(attribute->getNodeValue()));
        }
    }

    DOMNode* child = node->getFirstChild();
    while (child != NULL) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            state->addChild(createState(child));
        }
        child = child->getNextSibling();
    }

    // if node has no child elements, read its value
    if (!hasChildElementNodes(node)) {
        if (node->getFirstChild() != NULL) {
            state->setValue(Conversion::XMLChToString(node->getFirstChild()->
                                              getNodeValue()));
        }
    }

    return state;
}


/**
 * Returns true if the given node has child elements.
 *
 * @param node Node.
 * @return True if the node has child elements, otherwise false.
 */
bool
XMLSerializer::hasChildElementNodes(const DOMNode* node) const {
    DOMNode* child = node->getFirstChild();
    while (child != NULL) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            return true;
        }
        child = child->getNextSibling();
    }
    return false;
}

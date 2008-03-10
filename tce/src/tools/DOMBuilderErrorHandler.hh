/**
 * @file DOMBuilderErrorHandler.hh
 *
 * Declaration of DOMBuilderErrorHandler class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_DOM_BUILDER_ERROR_HANDLER_HH
#define TTA_DOM_BUILDER_ERROR_HANDLER_HH

#include <string>

#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>

#if _XERCES_VERSION >= 20200
XERCES_CPP_NAMESPACE_USE
#endif

/**
 * Error handler used when validating XML files by XMLSerializer.
 */
class DOMBuilderErrorHandler : public DOMErrorHandler {
public:
    DOMBuilderErrorHandler();
    virtual ~DOMBuilderErrorHandler();

    bool handleError(const DOMError& domError);
    int errorCount() const;
    std::string errorLog() const;

private:
    /// Number of errors handled.
    int errorCount_;
    /// Error log.
    std::string errorLog_;
};

#endif

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
 * @file DOMBuilderErrorHandler.hh
 *
 * Declaration of DOMBuilderErrorHandler class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
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

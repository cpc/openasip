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
 * @file MDFDocument.hh
 *
 * Declaration of MDFDocument class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MDF_DOCUMENT_HH
#define TTA_MDF_DOCUMENT_HH

#include "wx/docview.h"
#include "wx/wx.h"
#include "Model.hh"
#include "ModelObserver.hh"

/**
 * An instance of MDFDocument represents one machine object model for
 * the editor.
 *
 * Part of the wxWindows document/view framework. Opening and saving
 * of documents is passed through this class to  the xml reader/writer
 * components. When a new document is created or opened, this class
 * initializes a new model. Document's model can be accessed with the
 * getModel() method.
 */
class MDFDocument : public wxDocument, ModelObserver {
    DECLARE_DYNAMIC_CLASS(MDFDocument)
public:
    MDFDocument();
    virtual ~MDFDocument();
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnNewDocument();
    Model* getModel();
    virtual void update();

private:
    bool openCFG(const std::string& filename);
    bool openADF(const std::string& filename);

    /// Machine Object Model which the document represents.
    Model* model_;
};
#endif

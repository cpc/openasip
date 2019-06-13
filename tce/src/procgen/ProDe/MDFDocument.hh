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
 * @file MDFDocument.hh
 *
 * Declaration of MDFDocument class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
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

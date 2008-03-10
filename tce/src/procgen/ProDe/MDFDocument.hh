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

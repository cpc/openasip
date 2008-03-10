/**
 * @file Figure.hh
 *
 * Declaration of Figure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_FIGURE_HH
#define TTA_FIGURE_HH

#include <wx/wx.h>
#include <vector>

class MachineCanvasOptions;

/**
 * Abstract base class for classes that represent the visuals of a
 * machine block.
 *
 * Can draw itself on the given device context. May contain other
 * Figures as children, and in that case, knows also how to layout
 * them.
 */
class Figure {
public:
    Figure();
    virtual ~Figure();

    wxPoint location() const;
    void setLocation(wxPoint point);
    void setX(int x);
    void setY(int y);
    void setPreferredX(int x);
    bool xSet() const;
    void clearXSetFlag();
    virtual wxRect bounds() const;
    virtual wxRect virtualBounds() const;
    void setBounds(wxSize bounds);
    void setWidth(int width);
    void setHeight(int height);
    virtual void addChild(Figure* child);
    int childCount() const;
    Figure* child(int index) const;
    virtual void layout(wxDC* dc);
    virtual void draw(wxDC* dc);
    void highlight(const wxColour& colour);
    void clearHighlight();
    void setOptions(MachineCanvasOptions* options);
    MachineCanvasOptions* options();

protected:
    virtual void drawSelf(wxDC* dc);
    void drawChildren(wxDC* dc);
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);

    /// Top-left location of the Figure's bounding rectangle.
    wxPoint location_;
    /// wxSize of the Figure's bounding rectangle.
    wxSize size_;
    /// Figure's minimum size.
    wxSize minSize_;
    /// Figure's children.
    std::vector<Figure*> children_;

    /// Tells if x-coordinate has been fixed.
    bool xSet_;
    /// Tells whether the Figure and its children have been laid out or not.
    bool laidOut_;
    /// Tells whether the Figure and its children have been drawn or not.
    bool drawn_;
    /// Highlight colour.
    wxColour highlight_;
    /// Tells if the figure is highlighted.
    bool highlighted_;

    /// Options which are used for customizing figures.
    MachineCanvasOptions* options_;

private:
    /// Assignment not allowed.
    Figure& operator=(Figure& old);
    /// Copying not allowed.
    Figure(Figure& old);
};

#include "Figure.icc"

#endif

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
 * @file EPSDC.hh
 *
 * Declaration of EPSDC class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EPS_DC_HH
#define TTA_EPS_DC_HH

#include <wx/wx.h>
#include "EPSGenerator.hh"

/**
 * Encapsulated postscript device context.
 *
 * EPSDC is a wxWidgets device context for drawing graphics and text to
 * an eps file.
 */
class EPSDC : public wxDC {
public:
    EPSDC();
    virtual ~EPSDC();

    virtual bool Ok() const;
    virtual void BeginDrawing();
    virtual void EndDrawing();

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    virtual void DoCrossHair(wxCoord x, wxCoord y);
    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawLines(
        int n,
        wxPoint points[],
        wxCoord xoffset,
        wxCoord yoffset);

    virtual void DoDrawArc(
        wxCoord x1,
        wxCoord y1,
        wxCoord x2,
        wxCoord y2,
        wxCoord xc,
        wxCoord yc);

    virtual void DoDrawEllipticArc(
        wxCoord x,
        wxCoord y,
        wxCoord width,
        wxCoord height,
        double start,
        double end);

    virtual bool DoFloodFill(
        wxCoord x,
        wxCoord y,
        const wxColour& colour,
        int style);

    virtual void DoDrawBitmap(
        const wxBitmap& bitmap,
        wxCoord x,
        wxCoord y,
        bool transparent);

    virtual void DoDrawPolygon(
        int n,
        wxPoint points[],
        wxCoord xoffset = 0,
        wxCoord yoffset = 0,
        int fillStyle = wxODDEVEN_RULE);

    virtual void DoDrawRectangle(
        wxCoord x,
        wxCoord y,
        wxCoord width,
        wxCoord height);

    virtual void DoDrawRoundedRectangle(
        wxCoord x,
        wxCoord y,
        wxCoord width,
        wxCoord height,
        double radius = 20);

    virtual void DoDrawEllipse(
        wxCoord x,
        wxCoord y,
        wxCoord width,
        wxCoord height);

    virtual void DoDrawRotatedText(
        const wxString& text,
        wxCoord x,
        wxCoord y,
        double angle);

    virtual bool DoBlit(
        wxCoord xdest,
        wxCoord ydest,
        wxCoord width,
        wxCoord height,
        wxDC* source,
        wxCoord xsrc,
        wxCoord ysrc,
        int logicalFunc = wxCOPY,
        bool useMask = false,
        wxCoord xsrcMask = -1,
        wxCoord ysrcMask = -1);

    virtual void DoGetSize(wxCoord* width, wxCoord* height) const;

    virtual void DoGetTextExtent(
        const wxString& text,
        wxCoord* w,
        wxCoord* h,
        wxCoord* descent = NULL,
        wxCoord* externalLeading = NULL,
        wxFont* font = NULL) const;

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour* colour) const;

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;

    virtual void Clear();

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);
    virtual void SetLogicalFunction(int function);

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual int GetDepth() const;

    virtual bool StartDoc(const wxString& message);
    virtual void EndDoc();
    virtual void StartPage();
    virtual void EndPage();

    void setTitle(const std::string& title);
    void setCreator(const std::string& creator);

    void writeToStream(std::ostream& stream);

private:

    void setLineColour(const wxColour& colour);
    void setFillColour(const wxColour& colour);
    void drawCircle(int x, int y, unsigned radius);

    /// EPSGenerator generating the postscript code.
    EPSGenerator eps_;
    /// True, if background brush is set to fill the shapes.
    bool fill_;
    /// Current font size.
    unsigned fontSize_;
};

#endif

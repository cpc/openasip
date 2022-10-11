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
 * @file EPSDC.hh
 *
 * Declaration of EPSDC class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EPS_DC_HH
#define TTA_EPS_DC_HH

#include <wx/wx.h>
#include "EPSGenerator.hh"

// TODO: this module is commented out for wxwidgets version 3
// and needs to be rewritten
#if !wxCHECK_VERSION(3, 0, 0)
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
#endif

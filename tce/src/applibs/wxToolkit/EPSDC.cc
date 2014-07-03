/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file EPSDC.cc
 *
 * Implementation of EPSDC class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "EPSDC.hh"
#include "WxConversion.hh"
#include "VertexList.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 */
EPSDC::EPSDC() : wxDC(), fill_(false), fontSize_(0) {
}

/**
 * The Destructor.
 */
EPSDC::~EPSDC() {
}


/**
 * Returns true, if the DC is ok to use.
 *
 * @return always true
 */
bool
EPSDC::Ok() const {
   return true;
}

/**
 * Draws a line on the dc.
 *
 * @param x1 x-coordinate of the first end of the line.
 * @param y1 y-coordinate of the first end of the line.
 * @param x2 x-coordinate of the second end of the line.
 * @param y2 y-coordinate of the second end of the line.
 */
void
EPSDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
    eps_.drawLine(x1, -1 * y1, x2, -1 * y2);
}

/**
 * Draws a polygon on the dc.
 *
 * @param n Number of vertices.
 * @param vertices Vertices.
 * @param xOffset Offset for x-coordinates.
 * @param yOffset Offset for y-coordinates.
 */
void
EPSDC::DoDrawPolygon(
    int n,
    wxPoint  vertices[],
    wxCoord xOffset,
    wxCoord yOffset,
    int /* fillStyle */) {

    // Create VertexList of the vertices.
    VertexList list;
    for (int i = 0; i < n; i++) {
        list.addVertex(vertices[i].x + xOffset, -1 * (vertices[i].y + yOffset));
    }

    if (fill_) {
        eps_.drawFilledPolygon(list);
    } else {
        eps_.drawPolygon(list);
    }

}

/**
 * Draws a rectangle on the dc.
 *
 * @param x Lower left corner x-coordinate of the canvas.
 * @param y Lower left corner y-coordinate of the canvas.
 */
void
EPSDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) {
    if (fill_) {
        eps_.drawFilledRectangle(x, (-1 * y - height), width, height);
    } else {
        eps_.drawRectangle(x, (-1 * y - height), width, height);
    }
}

/**
 * Draws text on the dc.
 *
 * @param text Text to draw.
 * @param x x-coordinate of the lower left corner of the text.
 * @param y y-coordinate of the lower left corner of the text.
 */
void
EPSDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y) {
    std::string textStr = WxConversion::toString(text);
    eps_.drawText(x, -1 * y - 12, textStr);
}

/**
 * Clears all drawings on the canvas.
 */
void
EPSDC::Clear() {
    eps_.clearBuffer();
}

/**
 * Sets the dc font.
 *
 * Only font size is currently set.
 *
 * @param font Font to set.
 */
void
EPSDC::SetFont(const wxFont& font) {
    fontSize_ = font.GetPointSize();
    eps_.setFont(fontSize_);
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::SetPen(const wxPen& pen) {
    wxColour lineColour = pen.GetColour();
    eps_.setLineWidth(pen.GetWidth());
    setLineColour(lineColour);
}

/**
 * Sets the brush used for filling shapes with colour.
 *
 * Only solid and transparent brushes are supported. Everything else
 * is interpreted as transparent brush.
 */
void
EPSDC::SetBrush(const wxBrush& brush) {
    if (brush.GetStyle() == wxSOLID) {
        fill_ = true;
        wxColour fillColour = brush.GetColour();
        setFillColour(fillColour);
    } else {
        fill_ = false;
    }
}



/**
 * Sets the eps-file title.
 *
 * @param title String which will be set as the eps file title.
 */
void
EPSDC::setTitle(const std::string& title) {
    eps_.setTitle(title);
}

/**
 * Sets the eps-file creator string.
 *
 * @param creator String describin the eps file creator.
 */
void
EPSDC::setCreator(const std::string& creator) {
    eps_.setCreator(creator);
}

/**
 * Writes the generated eps file to an output stream.
 *
 * @param stream Output stream where the eps file contents will be written.
 */
void
EPSDC::writeToStream(std::ostream& stream) {
    eps_.writeEPS(stream);
}


/**
 * Bitmap drawing not implemented.
 *
 * @return Always false.
 */
bool
EPSDC::CanDrawBitmap() const {
    return false;
}


/**
 * Called by the device context client before the drawing is started.
 *
 * @return Always true.
 */
bool
EPSDC::StartDoc(const wxString& /* message */) {
    return true;
}


/**
 * NOT IMPLEMENTED.
 */
void
EPSDC::DoDrawRotatedText(
    const wxString& /* text */, wxCoord, wxCoord, double) {
}

/**
 * NOT IMPLEMENTED.
 */
void
EPSDC::DoDrawPoint(wxCoord, wxCoord) {
}

/**
 * NOT IMPLEMENTED
 */
bool
EPSDC::DoFloodFill(wxCoord, wxCoord, const wxColour&, int) {
    return false;
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoGetSize(wxCoord*, wxCoord*) const {
}

/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoDrawArc(wxCoord, wxCoord, wxCoord, wxCoord, wxCoord, wxCoord) {
}

/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoDrawIcon(const wxIcon&, wxCoord, wxCoord) {
}

/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoCrossHair(wxCoord, wxCoord) {
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoDrawBitmap(const wxBitmap&, wxCoord, wxCoord, bool) {
}

/**
 * Called before the drawing ends.
 */
void
EPSDC::BeginDrawing() {
    // Do nothing.
}

/**
 * Called when the drawing ends.
 */
void
EPSDC::EndDrawing() {
    // Do nothing.
}



/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoDrawLines(
        int,
        wxPoint[] /* points[] */,
        wxCoord,
        wxCoord) {
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoDrawEllipticArc(
        wxCoord,
        wxCoord,
        wxCoord,
        wxCoord,
        double,
        double) {
}

/**
 * Not implemented, falls back to a rectangle.
 */
void
EPSDC::DoDrawRoundedRectangle(
        wxCoord x,
        wxCoord y,
        wxCoord width,
        wxCoord height,
        double) {
    DoDrawRectangle(x, y, width, height);
}

/**
 * Draws an ellipse on the dc.
 *
 * @param x Ellipse bounding box lower left corner x-coordinate.
 * @param y Ellipse bounding box lower left corner y-coordinate.
 * @param width Ellipse width.
 * @param height Ellipse height.
 */
void
EPSDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) {

    y = -1 * y -height;

    // Check if the ellipse is actually circle, which will generated more
    // simple postscript-code.
    if (width == height) {
        x += width / 2;
        y += width / 2;
        drawCircle(x, y, width / 2);
        return;
    }

    if (fill_) {
        eps_.drawFilledEllipse(x, y, width, height);
    } else {
        eps_.drawEllipse(x, y, width, height);
    }
}


/**
 * NOT IMPLEMENTED
 */
bool
EPSDC::DoBlit(
    wxCoord,
    wxCoord,
    wxCoord,
    wxCoord,
    wxDC*,
    wxCoord,
    wxCoord,
    int,
    bool,
    wxCoord,
    wxCoord) {

    return false;
}

/**
 * Sets the line drawing colour of the eps generator.
 *
 * @param colour New line drawing colour.
 */
void
EPSDC::setLineColour(const wxColour& colour) {
    double redC = (double)colour.Red() / 255;
    double greenC = (double)colour.Green() / 255;
    double blueC = (double)colour.Blue() / 255;
    eps_.setLineColour(redC, greenC, blueC);
}

/**
 * Sets the filling colour of the eps generator.
 *
 * @param colour New filing colour.
 */
void
EPSDC::setFillColour(const wxColour& colour) {
    double redC = (double)colour.Red() / 255;
    double greenC = (double)colour.Green() / 255;
    double blueC = (double)colour.Blue() / 255;
    eps_.setFillColour(redC, greenC, blueC);
}


/**
 * Draws a circle.
 *
 * @param x X-coordinate of the circle centre.
 * @param y Y-coordinate of the circle centre.
 * @param radius Radius of the circle.
 */
void
EPSDC::drawCircle(int x, int y, unsigned radius) {

    if (fill_) {
        eps_.drawFilledCircle(x, y, radius);
    } else {
        eps_.drawCircle(x, y, radius);
    }
}

/**
 * Returns crude approximation of the width and height of a text on the dc.
 *
 * @param w Approximation of width is set as the value of w.
 * @param h Approximation of height is set as the value of h.
 */
void
EPSDC::DoGetTextExtent(
        const wxString& text,
        wxCoord* w,
        wxCoord* h,
        wxCoord*,
        wxCoord*,
        wxFont*) const {

    // TODO: find a better way to approximate text extent.
    *h = fontSize_;
    *w = Conversion::toInt((fontSize_ * 72.0 / 120) * text.Length());
}

/**
 * NOT IMPLEMENTED
 *
 * @return Always false.
 */
bool
EPSDC::DoGetPixel(wxCoord, wxCoord, wxColour*) const {
    return false;
}

/**
 * NOT IMPLEMENTED
 */
void
EPSDC::DoSetClippingRegionAsRegion(const wxRegion&) {
}


/**
 * NOT IMPLEMENTED
 *
 * @return Always false.
 */
bool
EPSDC::CanGetTextExtent() const {
    return false;
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::SetBackground(const wxBrush&) {
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::SetBackgroundMode(int) {
}


/**
 * NOT IMPLEMENTED
 */
void
EPSDC::SetPalette(const wxPalette&) {
}

/**
 * NOT IMPLEMENTED
 */
void
EPSDC::SetLogicalFunction(int) {
}


/**
 * NOT IMPLEMENTED
 *
 * @return Always zero.
 */
wxCoord
EPSDC::GetCharHeight() const {
    return 0;
}

/**
 * NOT IMPLEMENTED
 *
 * @return Always zero.
 */
wxCoord
EPSDC::GetCharWidth() const {
    return 0;
}

/**
 * NOT IMPLEMENTED
 *
 * @return Always zero.
 */
int
EPSDC::GetDepth() const {
    return 0;
}

/**
 * Starts a new page in the document.
 */
void
EPSDC::StartPage() {
    // Do nothing.
}

/**
 * Ends current page.
 */
void
EPSDC::EndPage() {
    // Do nothing.
}

/**
 * Ends the document that is being drawn on.
 */
void
EPSDC::EndDoc() {
    // Do nothing.
}

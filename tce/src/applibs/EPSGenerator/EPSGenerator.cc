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
 * @file EPSGenerator.cc
 *
 * Implementation of EPSGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include "EPSGenerator.hh"
#include "Application.hh"
#include "Conversion.hh"
#include "VertexList.hh"

using boost::format;
using std::endl;

const unsigned EPSGenerator::DEFAULT_MARGIN = 20;

const std::string EPSGenerator::FMT_RLINETO = " %1% %2% rlineto";
const std::string EPSGenerator::FMT_LINETO = " %1% %2% lineto";
const std::string EPSGenerator::FMT_MOVETO = " %1% %2% moveto";

/**
 * The Constructor.
 */
EPSGenerator::EPSGenerator() :
    hasTitle_(false), title_(""), creator_("TCE EPS Generator"),
    lineWidth_(1), minX_(0), minY_(0), maxX_(0), maxY_(0),
    scale_(1) ,boundsSet_(false), xMargin_(DEFAULT_MARGIN),
    yMargin_(DEFAULT_MARGIN) {

    time_t currentTime;
    time(&currentTime);
    creationDate_ = ctime(&currentTime);

    fillColour_.r = 0;
    fillColour_.g = 0;
    fillColour_.b = 0;
    lineColour_.r = 0;
    lineColour_.g = 0;
    lineColour_.b = 0;
}


/**
 * The Destructor.
 */
EPSGenerator::~EPSGenerator() {
    clearBuffer();
}

/**
 * Stretches eps bounds so that the given point is inside the bounds.
 */
void
EPSGenerator::appendToBounds(int x, int y) {

    if (boundsSet_) {
        minX_ = (x < minX_) ? x : minX_;
        maxX_ = (x > maxX_) ? x : maxX_;
        minY_ = (y < minY_) ? y : minY_;
        maxY_ = (y > maxY_) ? y : maxY_;
    } else {
        boundsSet_ = true;
        minX_ = x;
        maxX_ = x;
        minY_ = y;
        maxY_ = y;
    }
}

/**
 * Draws a rectangle.
 *
 * @param x Lower left corner x-coordinate of the rectangle.
 * @param y Lower left corner y-coordinate of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void
EPSGenerator::drawRectangle(int x, int y, unsigned width, unsigned height) {
    drawRectanglePath(x, y, width, height);
    buffer_.push("stroke");
}

/**
 * Draws a rectangle fileld with the fill colour set.
 *
 * @param x Lower left corner x-coordinate of the rectangle.
 * @param y Lower left corner y-coordinate of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void
EPSGenerator::drawFilledRectangle(
    int x, int y, unsigned width, unsigned height) {

    drawRectanglePath(x, y, width, height);
    buffer_.push(" gsave");
    useFillColour();
    buffer_.push(" fill");
    buffer_.push(" grestore");
    useLineColour();
    buffer_.push("stroke");
}

/**
 * Writes path of a rectangle outline to the eps buffer.
 *
 * @param x Lower left corner x-coordinate of the rectangle.
 * @param y Lower left corner y-coordinate of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void
EPSGenerator::drawRectanglePath(
    int x, int y, unsigned width, unsigned  height) {

    appendToBounds(x, y);
    appendToBounds(x + width, y + height);

    format move = format(FMT_MOVETO) % x % y;
    format line1 = format(FMT_RLINETO) % width % 0;
    format line2 = format(FMT_RLINETO) % 0 % height;
    format line3 = format(FMT_RLINETO) % (-1 * (int)width) % 0;

    buffer_.push("");
    buffer_.push("newpath");
    buffer_.push(move.str());
    buffer_.push(line1.str());
    buffer_.push(line2.str());
    buffer_.push(line3.str());
    buffer_.push(" closepath");
}


/**
 * Draws a line.
 *
 * @param x1 X-coordinate of the first end point of the line.
 * @param y1 Y-coordinate of the first end point of the line.
 * @param x2 X-coordinate of the second end point of the line.
 * @param y2 Y-coordinate of the second end point of the line.
 */
void
EPSGenerator::drawLine(int x1, int y1, int x2, int y2) {

    appendToBounds(x1, y1);
    appendToBounds(x2, y2);

    format move = format(FMT_MOVETO) % x1 % y1;
    format line = format(FMT_LINETO) % x2 % y2;
    buffer_.push("");
    buffer_.push("newpath");
    buffer_.push(move.str());
    buffer_.push(line.str() );
    buffer_.push("stroke");
}

/**
 * Draws a polygon outline.
 *
 * @param vertices Vector of at least three coordinate pairs which are the
 *                 polygon vertices.
 */
void
EPSGenerator::drawPolygon(const VertexList& vertices) {
    assert(vertices.size() > 2);
    drawPolygonPath(vertices);
    buffer_.push("stroke");
}


/**
 * Draws a filled polygon.
 *
 * @param vertices Vector of at least three coordinate pairs which are the
 *                 polygon vertices.
 */
void
EPSGenerator::drawFilledPolygon(const VertexList& vertices) {

    assert(vertices.size() > 2);
    drawPolygonPath(vertices);
    buffer_.push(" gsave");
    useFillColour();
    buffer_.push(" fill");
    buffer_.push(" grestore");
    useLineColour();
    buffer_.push("stroke");
}

/**
 * Writes path of a polygon outline to the eps buffer.
 *
 * @param vertices Vector of at least three coordinate pairs which are the
 *                 polygon vertices.
 */
void
EPSGenerator::drawPolygonPath(const VertexList& vertices) {

    assert(vertices.size() > 2);
    
    format move(FMT_MOVETO);

    int x = vertices.vertexX(0);
    int y = vertices.vertexY(0);
    appendToBounds(x, y);
    move % x % y;

    buffer_.push("");
    buffer_.push("newpath");
    buffer_.push(move.str());

    for (unsigned i = 1; i < vertices.size(); i++) {

        x = vertices.vertexX(i);
        y = vertices.vertexY(i);
        appendToBounds(x, y);
        format lineto(FMT_LINETO);
        lineto % x % y;
        buffer_.push(lineto.str());
    }

    buffer_.push(" closepath");
}


/**
 * Draws a circle.
 *
 * @param x X-coordinate of the circle centre.
 * @param y Y-coordinate of the circle centre.
 * @param radius Radius of the circle.
 */
void
EPSGenerator::drawCircle(int x, int y, unsigned radius) {
    drawCirclePath(x, y, radius);
    buffer_.push("stroke");
}

/**
 * Draws a circle filled with the fill colour.
 *
 * @param x X-coordinate of the circle centre.
 * @param y Y-coordinate of the circle centre.
 * @param radius Radius of the circle.
 */
void
EPSGenerator::drawFilledCircle(int x, int y, unsigned radius) {
    drawCirclePath(x, y, radius);
    buffer_.push(" gsave");
    useFillColour();
    buffer_.push(" fill");
    buffer_.push(" grestore");
    useLineColour();
    buffer_.push("stroke");
}


/**
 * Writes path of a circle outline to the eps buffer.
 *
 * @param x X-coordinate of the circle centre.
 * @param y Y-coordinate of the circle centre.
 * @param radius Radius of the circle.
 */
void
EPSGenerator::drawCirclePath(int x, int y, unsigned radius) {

    appendToBounds(x + radius, y + radius);
    appendToBounds(x - radius, y - radius);

    buffer_.push("");

    std::string fmtCircle = " %1% %2% %3% 0 360 arc";

    boost::format circle = format(fmtCircle) % x % y % radius;
    buffer_.push("newpath");
    buffer_.push(circle.str());
}


/**
 * Draws outline of an ellipse.
 *
 * @param x X-coordinate of the ellipse bounding box lower left corner.
 * @param y Y-coordinate of the ellipse bounding box lower left corner.
 * @param width Width of the ellipse.
 * @param height Height of the ellipse.
 */
void
EPSGenerator::drawEllipse(int x, int y, unsigned width, unsigned height) {
    doDrawEllipse(x, y, width, height, false);
}


/**
 * Draws a filled ellipse.
 *
 * @param x X-coordinate of the ellipse bounding box lower left corner.
 * @param y Y-coordinate of the ellipse bounding box lower left corner.
 * @param width Width of the ellipse.
 * @param height Height of the ellipse.
 */
void
EPSGenerator::drawFilledEllipse(
    int x, int y, unsigned width, unsigned height) {

    doDrawEllipse(x, y, width, height, true);
}

/**
 * Writes path of an ellipse outline to the eps buffer.
 *
 * @param x X-coordinate of the ellipse bounding box lower left corner.
 * @param y Y-coordinate of the ellipse bounding box lower left corner.
 * @param width Width of the ellipse.
 * @param height Height of the ellipse.
 * @param fill True, if the ellipse shoud be filled.
 */
void
EPSGenerator::doDrawEllipse(
    int x, int y, unsigned width, unsigned height, bool fill) {

    appendToBounds(x + width, y + height);
    appendToBounds(x, y);

    buffer_.push("");

    format fmtScale = format("1 %1% %2% div scale");
    format fmtTranslate = format("%1% %2% translate");
    fmtScale % width % height;
    fmtTranslate % (x + width / 2) % (y + height / 2);

    buffer_.push("gsave");
    buffer_.push(fmtTranslate.str());
    buffer_.push(fmtScale.str());


    std::string fmtEllipse = " 0 0 %4% 2 div 0 360 arc";

    boost::format ellipse = format(fmtEllipse) % x % y % width % height;
    buffer_.push("newpath");
    buffer_.push(ellipse.str());

    if (fill) {
        buffer_.push(" gsave");
        useFillColour();
        buffer_.push(" fill");
        buffer_.push(" grestore");
        useLineColour();
    }

    buffer_.push("stroke");
    buffer_.push("grestore");
}

/**
 * Draws text to the eps with the current font set with setFont method.
 *
 * @param x X-coordinate of the text bounds lower left corner.
 * @param y Y-coordinate of the text bounds lower left corner.
 * @param text Text to draw.
 */
void
EPSGenerator::drawText(int x, int y, std::string text) {

    format fmtMoveTo("%1% %2% moveto");
    format fmtShowText(" (%1%) show");
    
    fmtMoveTo % x % y;
    fmtShowText % text;

    buffer_.push("");
    buffer_.push(fmtMoveTo.str());
    buffer_.push(fmtShowText.str());
    
}

/**
 * Sets the creator string of the image.
 *
 * The creator string will be set as the Creator comment of the generated
 * eps file.
 *
 * @param creator String describing the creator of the eps-file.
 */
void
EPSGenerator::setCreator(std::string creator) {
    creator_ = creator;
}

/**
 * Sets the title of the eps file.
 *
 * Title string will be set as the title comment of the generated eps file.
 * If the title string is empty, the title comment will be omitted.
 *
 * @param title Title of the eps file.
 */
void
EPSGenerator::setTitle(std::string title) {

    if (title.length() > 0) {
        hasTitle_ = true;
    } else {
        hasTitle_ = false;
    }
    title_ = title;
}

/**
 * Sets the line width of the shapes that are drawn after this function call.
 *
 * @param width Width of the lines drawn to the eps.
 */
void
EPSGenerator::setLineWidth(unsigned width) {
    lineWidth_ = width;
    buffer_.push("");
    std::string setlinewidth = "%1% setlinewidth";
    buffer_.push(str(format(setlinewidth) % width));
}


/**
 * Sets the font face and size.
 *
 * @param size Font height in pixels.
 * @param fontName Name of the font. The name must be valid post script font
 *        name.
 */
void
EPSGenerator::setFont(unsigned size, std::string fontName) {
    format setFont = format("/%2% findfont %1% scalefont setfont");
    setFont % size % fontName;
    buffer_.push("");
    buffer_.push(setFont.str());
}

/**
 * Sets the line drawing colour.
 *
 * @param r Red component scaled between 0 and 1.
 * @param g Green component scaled between 0 and 1.
 * @param b Blue component scaled between 0 and 1.
 */
void
EPSGenerator::setLineColour(double r, double g, double b)
    throw (OutOfRange) {

    if (r < 0 || g < 0 || b < 0 || r > 1 || g > 1 || b > 1) {
        std::string error =
            "Colour component values must be between 0 and 1.";
        std::string proc = "EPSGenerator::setLineColour";
        OutOfRange e(__FILE__, __LINE__, proc, error);
        throw e;
    }

    lineColour_.r = r;
    lineColour_.g = g;
    lineColour_.b = b;

    useLineColour();
}

/**
 * Sets the shape filling colour.
 *
 * @param r Red component scaled between 0 and 1.
 * @param g Green component scaled between 0 and 1.
 * @param b Blue component scaled between 0 and 1.
 */
void
EPSGenerator::setFillColour(double r, double g, double b)
    throw (OutOfRange) {

    if (r < 0 || g < 0 || b < 0 || r > 1 || g > 1 || b > 1) {
        std::string error =
            "Colour component values must be between 0 and 1.";
        std::string proc = "EPSGenerator::setFillColour";
        OutOfRange e(__FILE__, __LINE__, proc, error);
        throw e;
    }

    fillColour_.r = r;
    fillColour_.g = g;
    fillColour_.b = b;
}


/**
 * Clears the eps code buffer.
 */
void
EPSGenerator::clearBuffer() {
    while(!buffer_.empty()) {
        buffer_.pop();
    }
}

/**
 * Generates an eps file which contains the graphics client drew.
 *
 * @param ostream Output stream to write the eps file contents to.
 */
void
EPSGenerator::writeEPS(std::ostream& ostream) {

    int w = (int)(scale_*(maxX_ - minX_ + 2 * xMargin_));
    int h = (int)(scale_*(maxY_ - minY_ + 2 * yMargin_));

    // Write eps headers.
    ostream << "%!PS-Adobe-3.0 EPSF-3.0" << endl;
    ostream << "%%BoundingBox: 0 0 " << w << " " << h << " " << endl;
    if (hasTitle_) {
        ostream << "%%Title: (" << title_ << ")" << endl;
    }
    ostream << "%%Creator: " << creator_ << endl;
    ostream << "%%CreationDate: " << creationDate_ << endl;
    ostream << "%%EndComments" << endl;


    // Set scaling factor & translation. The coordinate system is translated
    // so that the minimum x and y coordinates are zero.
    ostream << scale_ << " " << scale_ << " scale" << endl;
    ostream << minX_ * -1 + (int)xMargin_ << " "
            << minY_ * -1 + (int)yMargin_ << " translate" << endl;


    // Write the postscript drawing code from the code buffer to the
    // output stream.
    while (!buffer_.empty()) {
        std::string line = buffer_.front();
        buffer_.pop();
        ostream << line << endl;
    }
    ostream << endl << "showpage" << endl << endl;

    // Write end of file comment.
    ostream << "%%EOF" << endl;
}


/**
 * Sets the coordinate system scaling factors for the coordinate axels.
 *
 * This function sets only the final scaling factor of the image.
 * Only the last scaling factor set before calling writeEPS has effect on
 * the final eps file.
 *
 * @param scale Scaling factor.
 */
void
EPSGenerator::setScale(double scale)
    throw (OutOfRange) {

    if (scale < 0) {
        std::string error = "Scaling factor must be greater than zero.";
        std::string proc = "EPSGenerator::setScale";
        OutOfRange e(__FILE__, __LINE__, proc, error);
        throw e;
    }

    scale_ = scale;
}


/**
 * Sets the eps margins.
 *
 * @param x Left and right side margin.
 * @param y Top and bottom margin.
 */
void
EPSGenerator::setMargins(unsigned x, unsigned y) {
    xMargin_ = x;
    yMargin_ = y;
}


/**
 * Sets the eps drawing colour to the current line drawing colour.
 */
void
EPSGenerator::useLineColour() {
    format setColour("%1% %2% %3% setrgbcolor");
    setColour % lineColour_.r % lineColour_.g % lineColour_.b;
    buffer_.push(setColour.str());
}

/**
 * Sets the eps drawing colour to the current filling colour.
 */
void
EPSGenerator::useFillColour() {
    format setColour("%1% %2% %3% setrgbcolor");
    setColour % fillColour_.r % fillColour_.g % fillColour_.b;
    buffer_.push(setColour.str());
}

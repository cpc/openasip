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
 * @file EPSGenerator.hh
 *
 * Declaration of EPSGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EPS_GENERATOR_HH
#define TTA_EPS_GENERATOR_HH

#include <string>
#include <iostream>
#include <queue>
#include "Exception.hh"

class VertexList;

/**
 * EPSGenerator is a tool for drawing vector graphics to an eps file.
 *
 * The drawing is done into a buffer using various drawing methods.
 * Once the drawing is done, the buffer which contains a valid .eps file
 * can be written to an output stream.
 *
 * The coordinate system used by EPSGenerator corresponds to the
 * Postscript coordinate system. Origo is in the lower left corner of the
 * image and one pixel corresponds to one 72th of an inch. 
 */
class EPSGenerator {
public:
    EPSGenerator();
    ~EPSGenerator();

    void setCreator(std::string creator);
    void setTitle(std::string title);

    void setLineWidth(unsigned width);
    void setFont(unsigned size, std::string fontName = "Courier-Bold");
    void setLineColour(double r, double g, double b)
        throw (OutOfRange);
    void setFillColour(double r, double g, double b)
        throw (OutOfRange);

    void setScale(double scale) throw (OutOfRange);
    void setMargins(unsigned x, unsigned y);

    void drawRectangle(int x, int y, unsigned width, unsigned height);
    void drawFilledRectangle(int x, int y, unsigned width, unsigned height);

    void drawCircle(int x, int y, unsigned radius);
    void drawFilledCircle(int x, int y, unsigned radius);

    void drawEllipse(int x, int y, unsigned width, unsigned height);
    void drawFilledEllipse(int x, int y, unsigned width, unsigned height);

    void drawPolygon(const VertexList& vertices);
    void drawFilledPolygon(const VertexList& vertices);

    void drawText(int x, int y, std::string text);

    void drawLine(int llx, int lly, int urx, int ury);

    void clearBuffer();
    void writeEPS(std::ostream& stream);

private:
    /// Copying forbidden.
    EPSGenerator(const EPSGenerator&);
    /// Assignment forbidden.
    EPSGenerator& operator=(const EPSGenerator&);

    void useLineColour();
    void useFillColour();

    void appendToBounds(int x, int y);

    void doDrawEllipse(
        int x,
        int y,
        unsigned width,
        unsigned height,
        bool fill);

    void drawRectanglePath(int x, int y, unsigned width, unsigned height);
    void drawCirclePath(int x, int y, unsigned radius);
    void drawPolygonPath(const VertexList& vertices);

    struct colour {
        double r;
        double g;
        double b;
    };

    /// True, if the EPS file has a title.
    bool hasTitle_;
    /// Title of the EPS file.
    std::string title_;

    /// String describing the creator of the document.
    std::string creator_;
    /// String describing the creation date of the EPS file.
    std::string creationDate_;
    /// Buffer for the .eps code to be written.
    std::queue<std::string> buffer_;

    /// Current width of the lines drawn.
    unsigned lineWidth_;

    /// Minimum x-coordinate used before scaling & translation.
    int minX_;
    /// Minimum y-coordinate used before scaling & translation.
    int minY_;
    /// Maximum x-coordinate used before scaling & translation.
    int maxX_;
    /// Maximum y-coordinate used before scaling & translation.
    int maxY_;

    /// Final scaling factor for the eps file.
    double scale_;

    /// True, if a point has been added to the bounds.
    bool boundsSet_;

    /// Margin to add on the left and right side of the figure in pixels.
    unsigned xMargin_;
    /// Margin to add on the top and bottom side of the figure in pixels.
    unsigned yMargin_;

    /// Current drawing colour for lines.
    colour lineColour_;
    /// Current colour for filling shape backgrounds.
    colour fillColour_;

    /// Default margin width.
    static const unsigned DEFAULT_MARGIN;

    /// Format string for postscript moveto command.
    static const std::string FMT_MOVETO;
    /// Format string for postscript lineto command.
    static const std::string FMT_LINETO;
    /// Format string for postscript rlineto command.
    static const std::string FMT_RLINETO;

};

#endif

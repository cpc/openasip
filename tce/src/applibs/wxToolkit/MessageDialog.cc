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
 * @file MessageDialog.cc
 *
 * Implementation of MessageDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi) 2005
 * @note rating: red
 */


#include <wx/tokenzr.h>
#include "MessageDialog.hh"
#include "WxConversion.hh"


// Maximum number of charactes per line.
const unsigned int MAX_LINE_WIDTH = 100;
// Maximum number of lines.
const int MAX_LINES = 20;


/**
 * Constructor.
 */
MessageDialog::MessageDialog(
    wxWindow* parent,
    const wxString& title,
    const wxString& message,
    long style):
    wxMessageDialog(
	parent,
	suppressLines(wordWrap(message, MAX_LINE_WIDTH), MAX_LINES),
	title, style) {

}



/**
 * Destructor.
 */
MessageDialog::~MessageDialog() {
}


/**
 * Wraps lines longer than given width to multiple lines. Existing line
 * breaks aren't modified, but are taken to account when determining line
 * lengths. Lines are wrapped at word boundaries only, unless a word is
 * longer than the given line width.
 *
 * @param string String to wrap.
 * @param lineWidth Maximum line width.
 * @return Word wrapped string.
 */
wxString
MessageDialog::wordWrap(const wxString& string, unsigned int lineWidth) {

    wxString wrapped;
    wxStringTokenizer lines(string, _T("\n"), wxTOKEN_RET_EMPTY);

    while (lines.HasMoreTokens()) {
	// Handle each existing line of the string separately.
	wxString line = lines.GetNextToken();
	wxStringTokenizer words(line, _T(" \t"), wxTOKEN_RET_DELIMS);
	wxString wrappedLine;
	int length = 0;
	while (words.HasMoreTokens()) {
	    // Handle lines word by word.
	    wxString word = words.GetNextToken();
	    if (length + word.length() < lineWidth) {
		// Word fits to the current line.
		wrappedLine.Append(word);
		length = length + word.length();
	    } else if (word.length() < lineWidth) {
		// Word doesn't fit to the current line, but isn't longer
		// than the maximum line width.
		wrappedLine.Append(_(" \n"));
		wrappedLine.Append(word);
		length = word.length();
	    } else {
		// Word is longer than the maximum line width.
		unsigned int i = 0;
		for (; i < word.length(); i = i + (lineWidth - 1)) {
		    wrappedLine.Append(_T(" \n"));
		    wrappedLine.Append(word.Mid(i, lineWidth - 1));
		    length = word.length() - i;
		}
	    }
	}
	wrappedLine.Append(_T("\n"));
	wrapped.Append(wrappedLine);
    }

    return wrapped;
}

/**
 * Truncates a string to given number of lines. If lines are suppressed,
 * text '[n lines suppressed.]' is appended to the truncated
 * string, where n is the number of lines suppressd.
 *
 * @param string String to truncate.
 * @param maxLines Maximum number of lines.
 * @return Truncated string.
 */
wxString
MessageDialog::suppressLines(const wxString& string, int maxLines) {

    int lineCount = 0;
    wxString truncated;

    wxStringTokenizer lines(
	string, _T("\n"), wxTOKEN_RET_EMPTY|wxTOKEN_RET_DELIMS);

    while (lineCount < maxLines && lines.HasMoreTokens()) {
	// Get maximum number of lines to the result string.
	lineCount++;
	truncated.Append(lines.GetNextToken());
    }

    if (!lines.HasMoreTokens()) {
	// No lines suppressed.
	return truncated;
    }

    int suppressed = 0;
    while (lines.HasMoreTokens()) {
	// Count suppressed lines.
	lines.GetNextToken();
	suppressed++;
    }

    truncated.Append(_T("\n [ "));
    truncated.Append(WxConversion::toWxString(suppressed));
    truncated.Append(_T(" lines suppressed.]"));
    return truncated;
}

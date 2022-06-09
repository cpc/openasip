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
 * @file EditLineReader.hh
 *
 * Declaration of EditLineReader class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: yellow
 */

#ifndef TTA_EDIT_LINE_READER_HH
#define TTA_EDIT_LINE_READER_HH

extern "C" {
#include <histedit.h>
}
#include <string>
#include <map>

#include "LineReader.hh"

/**
 * LineReader implementation using libedit library.
 *
 * libedit offers functionality to edit a line as well as command history
 * browsing functionality. In other words, user can edit the line he is 
 * writing as well as browse history of commands.
 */
class EditLineReader : public LineReader {
public:
    explicit EditLineReader(std::string program = "");
    virtual ~EditLineReader();

    virtual void initialize(
        std::string defPrompt = "",
        FILE* in = stdin, 
        FILE* out = stdout, 
        FILE* err = stderr);
    virtual std::string readLine(std::string prompt = "");
    virtual char charQuestion(
        std::string question, std::string allowedChars,
        bool caseSensitive = false, char defaultAnswer = '\0');

private:
    /// value_type for map.
    typedef std::map<EditLine*, EditLineReader*>::value_type ValType;
    /// Iterator for map.
    typedef std::map<EditLine*, EditLineReader*>::iterator MapIt;
    
    /// Copying not allowed.
    EditLineReader(const EditLineReader&);
    /// Assignment not allowed.
    EditLineReader& operator=(const EditLineReader&);

    static char* wrapperToCallPrompt(EditLine* edit);
    char* prompt();
    void updateHistory(const char* c);

    /// Line reader prompt.
    char* prompt_;
    /// The name of the invocating program.
    std::string program_;
    /// EditLine instance.
    EditLine* editLine_;
    /// History instance.
    History* history_;
    /// Map containing all (EditLine*, EditLineReader*) pairs to make prompt
    /// printing possible. This map offers to callback function
    /// 'wrapperToCallPrompt' a right instance of EditLineReader. This
    /// instance then returns the right prompt.
    static std::map<EditLine*, EditLineReader*> lineReaders_;

    /// Input stream is saved for end-of-file checking.
    FILE* in_;
};

#endif

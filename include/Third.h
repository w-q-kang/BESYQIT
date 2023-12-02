/*
Copyright 2022-23 Wolfgang Hermsen

This file is part of the BESYQIT Interpreter.

    The BESYQIT Interpreter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BESYQIT Interpreter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the BESYQIT Interpreter.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef THIRD_H
#define THIRD_H
#include "Log.h"
#include "Sentence.h"
#include "Paragraph.h"
#include "Basics.h"
#include <string>
#include <vector>

using namespace std;

class Third  : public Basics
{
    public:
        Third();
        virtual ~Third();
        int thirdParse(vector<Sentence> v);
        string getContentString();
        vector<Paragraph> getParagraphs();
        int calculateJump(int index, vector<Sentence> v);
        int calculateJumpLabel(int index, vector<Sentence> &v);
        int calculateJumpEnd(int index, vector<Sentence> &v);
        int calculateJumpInside(int index, vector<Sentence> &v);
        bool checkParagraphs();
    protected:

    private:
        // list of paragraphs, par. 0 is the start paragraph
        vector<Paragraph> paragraphs;
        Log log;
};

#endif // THIRD_H

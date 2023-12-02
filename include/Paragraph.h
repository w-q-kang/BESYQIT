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

#ifndef PARAGRAPH_H
#define PARAGRAPH_H
#include "Log.h"
#include "Sentence.h"
#include <string>
#include <vector>

using namespace std;

class Paragraph
{
    public:
        Paragraph();
        virtual ~Paragraph();
        void addSentence(Sentence sentence);
        Sentence getSentence(int index);
        void setName(string s);
        string getName();
        string getContentString();
        void setSaved(int index);
        int getLength();
        bool isHeader();
    protected:

    private:
        string name;
        vector<Sentence> sentences;
        bool header;
        Log log;
};

#endif // PARAGRAPH_H

/*
Copyright 2023 Wolfgang Hermsen

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
    Current
*/

#include "Paragraph.h"

Paragraph::Paragraph()
{
    //ctor
}

Paragraph::~Paragraph()
{
    //dtor
}

 void Paragraph::addSentence(Sentence sentence) {
    if (sentences.size() == 0) {
        header = sentence.isHeader();
        name = sentence.getName();
    }
    sentences.push_back(sentence);
 };

 Sentence Paragraph::getSentence(int index) {
     return sentences[index];
 };

int Paragraph::getLength() {
    return sentences.size();
};

void Paragraph::setName(string s) {
    name = s;
};

string Paragraph::getName() {
    return name;
};

bool Paragraph::isHeader() {
   return header;
}

void Paragraph::setSaved(int index) {
    Sentence s = sentences[index];
    s.setSaved();
    sentences[index] = s;
}

string Paragraph::getContentString() {
    int len = sentences.size();
    string result = "";
    if (header) {
        result += log.getMessage(NAMEISHEADER) + "\n";
    } else {
        result += log.getMessage1(NAMEIS, name) + "\n";
    }
    for (int i = 0; i < len; ++i) {
        result += sentences[i].getContentString(true, true) + "\n";
    }
    return result;
};

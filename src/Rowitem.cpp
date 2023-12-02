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
*/

#include "Rowitem.h"

Rowitem::Rowitem(string text)
{
    index = 0;
    plain = "";
    Numtest nt;
    typ = 0;
    if (text == "#") {
        typ = 1;
    } else if (text == "#sip") {
        typ = 7;
    } else if (text == "#chan") {
        typ = 8;
    } else if (text == "#mesec") {
        typ = 9;
    // to avoid misinterpretation of #neel as #n<eel>
    // #<n> must be tested first
    } else if (text.substr(0,1) == "#") {
        nt = testType(text.substr(1));
        if ((nt == NUMINT) || (nt == JUUINT)) {
            typ = 2;
            index = xtoll(text.substr(1));
        } else if (text.substr(0,2) == "#e") {
            nt = testType(text.substr(2));
            if ((nt == NUMINT) || (nt == JUUINT)) {
                typ = 3;
                index = xtoll(text.substr(2));
            }
        } else if (text.substr(0,2) == "#x") {
            nt = testType(text.substr(2));
            if ((nt == NUMINT) || (nt == JUUINT)) {
                typ = 4;
                index = xtoll(text.substr(2));
            }
        } else if (text.substr(0,2) == "#n") {
            nt = testType(text.substr(2));
            if ((nt == NUMINT) || (nt == JUUINT)) {
                typ = 5;
                index = xtoll(text.substr(2));
            }
        } else if (text.substr(0,2) == "#m") {
            nt = testType(text.substr(2));
            if ((nt == NUMINT) || (nt == JUUINT)) {
                typ = 6;
                index = xtoll(text.substr(2));
            }
        }
    }

    if (typ == 0) {plain = text;};
}

Rowitem::~Rowitem()
{
    //dtor
}

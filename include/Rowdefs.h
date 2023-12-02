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

#ifndef ROWDEFS_H
#define ROWDEFS_H


struct Rowdefs
{
    int offset;
    int rowlength;
    bool hasDefjan;
    bool hasDefet;
    bool hasDefxi;
    bool hasDefnyr;
    bool hasDefres;
    string defjan;
    string defnyr;
    string defet;
    string defxi;
    string defres;
    vector<string> inits;
};

#endif // ROWDEFS_H

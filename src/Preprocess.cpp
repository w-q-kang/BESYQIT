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

#include "Preprocess.h"
#include "globals.h"

using namespace std;

Preprocess::Preprocess()
{
    //ctor
}

Preprocess::~Preprocess()
{
    //dtor
}

void Preprocess::setFile(string filename) {
    log.logmsg(INFO, PREPROCESSING, filename, "");
    firstfile = filename;
    currDirectory = getDir(filename);
    if (last(currDirectory) != '\\') {
        currDirectory.push_back('\\');
    }
}

void Preprocess::process(){
    header.clear();
    bodies.clear();
    vector<string> v;
    vector<string> incl;
    int len = 0;

    try {
        v = readFile(firstfile, false, false, false);
        if (debug_L) {log.logdbg3(FIRSTFILELENGTH, to_string(v.size()), "", "");};
        len = getHeaderLength(v);
        for (int i = 0; i <= len; ++i) {header.push_back(v[i]);}
        if (debug_L) {log.logdbg3(HEADERLENGTH, to_string(header.size()), "", "");};
        incl =  getIncludes(header);
        if (debug_L) {log.logdbg3(INCLUDENUMBER, to_string(incl.size()), "", "");};
    } catch (exception& e) {
        log.logmsg(FATAL, WHENPREPROCESSHEAD, cstos(e.what()), "");
    }

    if (debug_L) {log.logdbg3(HEADERLINECOUNT, to_string(header.size()), "", "");};
    try {
        for (int i = len+1; i < (int) v.size(); ++i) {
            bodies.push_back(v[i]);
        }
        bodies.push_back("");
        if (debug_L) {log.logdbg3(FIRSTBODYLINECOUNT, to_string(bodies.size()), "", "");};
        if (incl.size() > 0) {
            for (int f = 0; f < (int) incl.size(); ++f) {
                if (debug_L) {log.logdbg3(READINGINCLUDEFILE, incl[f], "", "");};

                v = readFile(incl[f], false, false, false);
                int hlen = getHeaderLength(v);
                for (int i = hlen+1; i < (int) v.size(); ++i) {
                    bodies.push_back(v[i]);
                }
                bodies.push_back("");
            }
        }
    } catch (exception& e) {
        log.logmsg(FATAL, WHENPREPROCESSBODY, cstos(e.what()), "");
    }

    if (debug_L) {log.logdbg3(BODIESLINECOUNT, to_string(bodies.size()), "", "");};
}

int Preprocess::getHeaderLength(vector<string> content) {
    vector<string> result;
    int i = 0;
    bool foundfirst = false;
    while (i< (int) content.size()) {
        string text = trimLeft(content[i]);
        if ((text.size() > 0) && !foundfirst){
            foundfirst = true;
        } else if ((text.size() == 0) && foundfirst) {
            break;
        }
        i++;
    }
    return i;
}

vector<string> Preprocess::getIncludes(vector<string> content) {
   vector<string> result;
   for (int i = 0; i < (int) content.size(); ++i) {
        string text = content[i];
        if (text.length()>0) {text = trimLeft(text);}
        if (text.length()>0) {text = trimRight(text);}
        if (text.length()>0) {
            vector<string> parts = split(text, ' ');
            int last = parts.size() - 1;
            if (last>=0) {
                if (parts[last] == "diigana.") {
                   for (int j = 0; j < last; ++j) {
                        if (parts[j] != "jan") {
                            result.push_back(currDirectory + parts[j]);
                        }
                   }
                }
            }
        }
   }
    return result;
}

vector<string> Preprocess::getHeader() {
    return header;
}

vector<string> Preprocess::getBodies() {
    return bodies;
}

vector<string> Preprocess::getComplete() {
    vector<string> result;
    addToVector(result, header);
    result.push_back("");
    addToVector(result, bodies);
    return result;
}

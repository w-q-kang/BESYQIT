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

#include "Lexer.h"
#include <algorithm>

using namespace std;

    Lexer::Lexer() {
    //ctor
}

    Lexer::~Lexer() {
    //dtor
}
    // set the lines and process them
    void Lexer::setContent(vector<string> v) {
        content.clear();
        symbols.clear();
        string line = "";

        // set the lines
        int len = v.size();
        for (int i = 0; i < len; ++i) {
            line = v[i];
            line = trimLeft(line);
            if (line.size() <=1) {
                content.push_back(v[i]);
            // discard comment lines
            } else if ((line[0] != '/') || (line[1] != '/')) {
                content.push_back(v[i]);
            }
        }

        // process the lines
        len = content.size();
        for (int i = 0; i < len; ++i) {
            vector<string> liste = splitContent(i);
            int jlen = liste.size();
            for (int j = 0; j < jlen; ++j) {
                // discard rest of line comments (but keep line break!)
                string s = liste[j];
                if (s.size() > 1) {
                  if ((s[0] == '/') && (s[1] == '/')) {
                    symbols.push_back("<EOL>");
                    break;
                  }
                }
                symbols.push_back(s);
            }
        }
    }

    // get the list of lines set so far
    vector<string> Lexer::getContent() {
        return content;
    }

    // get the count of lines set so far
    int Lexer::getContentLength() {
        return content.size();
    }

   // get the list of symbols set so far
    vector<string> Lexer::getSymbols() {
        return symbols;
    }

     // get the count of symbols set so far
    int Lexer::getSymbolsLength() {
        return symbols.size();
    }


    // return the lexer result as a list joined by ';'
    string Lexer::getSymbolsString() {
        string s;
        s = "";

        int len = symbols.size();
        for (int i = 0; i < len; ++i) {
            s.append(symbols[i]);
            s.append(";");
        }
        return s;
    }

    // make a lexical analysis of line no. index
    vector<string> Lexer::splitContent(int index) {
        string s = content[index];
        vector<string> liste;
        string word = "";
        bool inside = false;
        int len = s.size();
        for (int i = 0; i < len; ++i) {
            char ch = s[i];
            if (inside) {
                word += ch;
            } else if (findInString(ch, " \t")) {
                if (word.size() > 0) {
                    liste.push_back(word);
                    word = "";
                }
                if (findInString(ch, ",")) {
                    word = ch;
                    liste.push_back(word);
                    word = "";
                }
            } else {
                word += ch;
            }

            if (ch == '"') {inside = !inside;}
        }

        if (word.size() > 0) {liste.push_back(word);}

        liste.push_back("<EOL>");
        return liste;
    }

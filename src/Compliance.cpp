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

#include "Compliance.h"
#include <algorithm>
#include <iostream>

using namespace std;

Compliance::Compliance()
{
    //ctor
}

Compliance::~Compliance()
{
    //dtor
}

tuple<LogMsg, string> Compliance::test(string word) {
    string current = "";
    int phase = 1;
    char lastType = 'X';
    char currType = '?';
    LogMsg pos = NOMSG;
    string msg = "";
    string myword = expand(word);
    for (int i = 0; i < (int) myword.size(); ++i) {
        char c = myword[i];
        currType = getType(phase, c);
        if (currType != lastType) {
            if (i>0) {
                pos = check(phase,current);
                phase = 2;
                if (pos != NOMSG) {break;}
            }
            current = "";
        }
        current += c;
        lastType = currType;
    }

    if ((current.size() > 0) && (pos == NOMSG)) {
      pos = check(3,current);
    }
    return make_tuple(pos, current);
}

string Compliance::expand(string word) {
    string result = "";
    for (int i = 0; i < (int) word.size(); ++i) {
       char c = word[i];
       auto f = find(digits.begin(), digits.end(), c);
       if (f == digits.end()) {
            result += c;
       } else {
            result += juubes[f-digits.begin()];
       }
    }
    return result;
}
char Compliance::getType(int phase, char c) {
    if (isVowel(phase, c)) {
        return 'V';
    } else if (isConsonant(phase, c)) {
        return 'C';
    }
    return 'D';
}

bool Compliance::isVowel(int phase, char c) {
    if ((c == 'y') &&(phase != 1)) {return true;}
    return find(vowels.begin(), vowels.end(), c) != vowels.end();
}

bool Compliance::isConsonant(int phase, char c) {
    if ((c == 'y') &&(phase == 1)) {return true;}
    return find(consonants.begin(), consonants.end(), c) != consonants.end();
}

LogMsg Compliance::check(int phase, string cluster) {
    LogMsg pos = NOMSG;
    char typ;
    string cl;

    if (cluster.size() > 0) {
        typ = getType(phase, cluster[0]);
        if (typ == 'V') {
            if (cluster.size() == 3) {
                if ((cluster[0] == cluster[1]) && (cluster[1] == cluster[2])) {
                    pos = NAMENOTCOMPLIES;
                }
            } else if (cluster.size() > 3) {
                pos = NAMENOTCOMPLIES;
            }
        } else if (typ == 'C') {
            if (cluster.size() == 2) {
                if (phase == 1) {
                    if (find(pre.begin(), pre.end(), cluster) == pre.end()) {
                        pos = INITNOTCOMPLIES;
                    };
                } else if (phase==3) {
                    if (find(post.begin(), post.end(), cluster) == post.end()) {
                         pos = ENDNOTCOMPLIES;
                    };
                }
            } else if (cluster.size() == 3) {
                if (phase != 2) {
                   pos = INITENDNOTCOMPLIES;
                } else {
                     cl = cluster.substr(0,2);
                     bool test1 = find(post.begin(), post.end(), cl) != post.end();
                     cl = cluster.substr(1,2);
                     test1 = test1 || (find(pre.begin(), pre.end(), cl) != pre.end());
                     if (!test1) {pos = NAMENOTCOMPLIES;}
                }
             } else if (cluster.size() > 3) {
                pos = NAMENOTCOMPLIES;
             }
        }
    }

     return pos;
}

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

#include "Basics.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include "globals.h"

using namespace std;

Basics::Basics()
{
}

Basics::~Basics()
{
}

// ==================== string and vector ====================

 bool Basics::findInString(char c, string s) {
    return s.find(c) != string::npos;
}

bool Basics::contains(string s, vector<string> v) {
    return find(v.begin(), v.end(), s) != v.end();
}

int Basics::getIndex(string s, vector<string> v) {
    int idx = -1;
    auto f = find(v.begin(), v.end(), s);
    if (f != v.end()) {
        idx = f - v.begin();
    }
    return idx;
}

vector<string> Basics::splitStart(string s, vector<string> v) {

    int i = 0;
    while (i < (int) v.size()) {
        int h = v[i].size();
        if (s == v[i]) {
            return {v[i], ""};
        } else if (s.substr(0, h) == v[i]) {
            return {v[i], s.substr(h, string::npos)};
        }
        i++;
    }
    return {s};
}

vector<string> Basics::splitStartInt(string s, vector<string> v) {

    int i = 0;
    while (i < (int) v.size()) {
        int h = v[i].size();
        if (s.substr(0, h) == v[i]) {
            if ((int) s.size()==h) {
                return {to_string(i), ""};
            } else {
                return {to_string(i), s.substr(h, string::npos)};
            }
        }
        i++;
    }
    return {s};
}

vector<string> Basics::splitEnding(string s, vector<string> v) {

    int i = 0;
    int l = s.size();
    while (i < (int) v.size()) {
        int h = v[i].size();
        if (l>=h) {
            if (s.substr(l-h, string::npos) == v[i]) {
                if (l==h) {
                    return {v[i], ""};
                } else {
                    return {v[i], s.substr(0,l-h)};
                }
            }
        }
        i++;
    }
    return {s};
}

 vector<string> Basics::split(string name, char c) {
    vector<string> liste;
    string word = "";
    int len = name.size();

    for (int i = 0; i < len; ++i) {
        char ch = name[i];
        if (ch == c) {
            if (word.size() > 0) {
                liste.push_back(word);
                word = "";
            }
        } else {
            word += ch;
        }
    }

    if (word.size() > 0) {liste.push_back(word);}

    return liste;
}

vector<string> Basics::split2(string name, char c1, char c2) {
    vector<string> liste;
    string word = "";
    int len = name.size();

    for (int i = 0; i < len; ++i) {
        char ch = name[i];
        if ((ch == c1) || (ch == c2)){
            if (word.size() > 0) {
                liste.push_back(word);
                word = "";
            }
        } else {
            word += ch;
        }
    }

    if (word.size() > 0) {liste.push_back(word);}

    return liste;
}


string Basics::head(string name) {
    int len = name.size();
    return name.substr(0, len - 1);
}

char Basics::last(string name) {
    int len = name.size();
    return name[len - 1];
}

string Basics::format(string text) {
        string r = text;
        string result = "";
        int len = r.size();
        int i = 0;
        bool escaped = false;
        if ((r[0]== '"') && (r[len-1]== '"')) {
            r = r.substr(1,len-2);
        }
        while (i<(int)r.size()) {
            char c = r[i];
            if (escaped) {
                escaped = false;
                if (c == 'b') {
                    result.push_back('"');
                } else if (c=='w') {
                    result.push_back('\n');
                } else if (c=='g') {
                    result.push_back('\t');
                } else {
                    result.push_back(c);
                }
            } else if (c != '\\') {
              result.push_back(c);
            } else {
                escaped = true;
            }
          ++i;
        }
        return result;
}

string Basics::trimLeft(string text) {
    text.erase(0, text.find_first_not_of(" "));
    return text;
}

string Basics::trimRight(string text) {
    if (text.length()>0) {
        text.erase(text.find_last_not_of(" ") + 1);
    }
    return text;
}

string Basics::toUpper(string text) {
    string result = "";
    for (int i = 0; i < (int) text.length(); ++i) {
        char c = text[i];
        if ((int(c) >= 97 ) && (int(c) <= 122)) {
            c = int(c) - 32;
        }
        result += c;
    }
    return result;
}

string Basics::toLower(string text) {
    string result = "";
    for (int i = 0; i < (int) text.length(); ++i) {
        char c = text[i];
        if ((int(c) >= 65 ) && (int(c) <= 90)) {
            c = int(c) + 32;
        }
        result += c;
    }
    return result;
}

// ==================== vector ====================


void Basics::addToVector(vector<string> &v, vector<string> &w) {
     for (int i = 0; i < (int) w.size(); ++i) {
        v.push_back(w[i]);
    }
}

void Basics::addToVectorP(vector<string>* v, vector<string>* w) {
     for (int i = 0; i < (int) w->size(); ++i) {
        v->push_back((*w)[i]);
    }
}

int Basics::countOcc(vector<string> &v, string search) {
    int result = 0;
    for (int i=0;i<(int) v.size(); ++i) {
        if (v[i] == search){++result;}
    }
    return result;
}

string Basics::join(vector<string> &v, string sep) {
    string result = "";
    int len = v.size();
    for (int i = 0; i < len; ++i) {
        if (i>0) {result+=sep;}
        result += v[i];
    }

    return result;
}

string Basics::joinint(vector<int> &v, string sep) {
    string result = "";
    int len = v.size();
    for (int i = 0; i < len; ++i) {
        if (i>0) {result+=sep;}
        result += to_string(v[i]);
    }

    return result;
}

// ==================== test functions ====================

bool Basics::isEmpty(string text) {
    return (text == "") || (text.size() == 0);
}


bool Basics::testInt(string text) {
    bool result = true;
    try {
        for (int i = 0; i < (int) text.length(); ++i) {
            char c = text[i];
            if ((int(c) < 48 ) || (int(c) > 57)) {
                if ((i > 0) || ((c != '+') && (c != '-'))) {
                    result = false;
                    break;
                }
            }
        }
    } catch (exception& e) {
        result = false;
    }

    return result;
}

bool Basics::testNum(string text) {
    bool result = true;
    int dots = 0;
    int imax = text.length() - 1;
    try {
        for (int i = 0; i <= imax; ++i) {
            char c = text[i];
            if ( c== '.') {dots++;}
            if ((int(c) < 48 ) || (int(c) > 57)) {
                if (c == '.') {
                    if ((i==imax) || (dots>1)) {
                        result = false;
                        break;
                    }
                } else if ((i > 0) || ((c != '+') && (c != '-')))  {
                    result = false;
                    break;
                }
            }
        }
    } catch (exception& e) {
        result = false;
    }

    return result;
}

bool Basics::isLong(string text) {
    bool result = testInt(text);
    try {
        if (result) {
            int i __attribute__((unused)) = stoll(text);
        }
    } catch (exception& e) {
        result = false;
    }

    return result;
}

bool Basics::isNumber(string text) {
    bool result = isLong(text);
    if(result) {return true;}
    result = testNum(text);

    try {
        if (result) {
            long double d __attribute__((unused)) = stold(text);
        }
    } catch (exception& e) {
        result = false;
    }

    return result;
}

// ==================== conversion ====================

string Basics::ctos(char c) {
    string result = "";
    result.push_back(c);
    return result;
}

string Basics::cstos(const char* a) {
    string s(a);
    return s;
 }

vector<string> Basics::stov(string s) {
    vector<string> v;
    for (int i = 0; i < (int) s.size(); ++i) {
        v.push_back(ctos(s[i]));
    }
    return v;
}

// ==================== file/dir ====================

vector<string> Basics::readFile(string name, bool keepEmpty, bool keepEol, bool keepEof) {
   fstream fs;
   vector<string> v;
   fs.open(name,ios::in); //open a file to perform read operation using file object
   if (fs.is_open()){   //checking whether the file is open
      string tp;
      while(getline(fs, tp)){ //read data from file object and put it into string.
         if ((tp.size() == 0) && keepEmpty) {
            v.push_back("<wayuq>");
         } else {
            v.push_back(tp);
         }
         if (keepEol) {v.push_back("<wak>");}
      }
      if (keepEof) {v.push_back("<sec>");}
      fs.close(); //close the file object.
   }
   return v;
}

vector<string>* Basics::readFileP(string name) {
   fstream fs;
   vector<string>* v = new vector<string>;
   fs.open(name,ios::in); //open a file to perform read operation using file object
   if (fs.is_open()){   //checking whether the file is open
      string tp;
      while(getline(fs, tp)){ //read data from file object and put it into string.
         v->push_back(tp);
      }
      fs.close(); //close the file object.
   }
   return v;
}

bool Basics::writeFile(string filename, vector<string> &v) {
    fstream fs;
    bool result = false;
    try {
        fs.open(filename,ios::app);
        if (fs.is_open()){
            for (int i = 0; i < (int) v.size(); ++i) {
                fs << v[i] << "\n";
            }
        }
        fs.close(); //close the file object.
         result = true;
    } catch (exception& e) {
        result = false;
    }

    return result;
}

string Basics::getDir(string s) {
    vector<string> result = split2(s, '\\', '/');
    int len = result.size();
    if(result[len-1].size()>0) {result.pop_back();}
    return join(result, "\\");
}

bool Basics::isPath(string s) {
    if (s.find(':')!=string::npos){return true;}
    if (s.find('\\')!=string::npos){return true;}
    if (s.find('/')!=string::npos){return true;}
    return false;
}

bool Basics::existsFile(string filename) {
    bool result = false;
    if (FILE *file = fopen(filename.c_str(), "r")) {
         fclose(file);
         result = true;
    }
    return result;
}

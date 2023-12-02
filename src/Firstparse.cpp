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

#include "Firstparse.h"
#include "globals.h"

using namespace std;

Firstparse::Firstparse()
{
}

Firstparse::~Firstparse()
{
    //dtor
}

// also returns newly defined verbs
int Firstparse::setParse(vector<string> v, vector<string>& verbdefs) {
    int result = 0;
     try {
        parse.clear();
        string entry = "";
        int len = v.size();
        if (debug_P) {log.logdbg3(PARSEVECTOR,join(v, " "), "", "");};
        for (int i = 0; i < len; ++i) {
            entry = v[i];
            if (debug_i) {log.logdbg3(ENTRY, entry, "", "");};
            Item item = Item(entry);
            parse.push_back(item);
            if (item.getTyp() == 'l') {
                verbdefs.push_back(item.getName());
            }
            if (item.hasErrors()) {
                result = -1;
                break;
            }
        }
        // add two extra newlines at file end for easier subsequent analysis
        Item item = Item("<EOL>");
        parse.push_back(item);
        parse.push_back(item);
    } catch(exception &e) {
       string s = e.what();
       log.logmsg(FATAL, FIRSTPARSEFAIL, s, "" );
       result = -1;
    }
    return result;
}

string Firstparse::getParserString() {
        string result = "";
        int len = parse.size();

    try {
        for (int i = 0; i < len; ++i) {
            string name = parse[i].content(true, true);
            result += name + ";";
            if (i < len - 1) {
                if ((parse[i].getTyp() == 'n') && (parse[i+1].getTyp() != 'n')) {result += "\n";}
             }
        }
    } catch(exception &e) {
       string s = e.what();
       log.logmsg(FATAL, FIRSTPARSEFAIL, s, "" );
    }
    return result;
}

vector<Item> Firstparse::getParse() {
    return parse;
}

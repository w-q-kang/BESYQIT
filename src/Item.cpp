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

#include "Item.h"
#include "globals.h"

using namespace std;

//ctor
Item::Item(string pname)
{
    isError = false;
    verbType = 0;
    // set name and typ
    entry = pname;
    name = pname;
    refdistance = 0;
    condLevel = 0;
    getSize = false;
    forget = false;
    vector<string> two;
    int len = pname.size();
    refFrom = 0;
    refTo = -1;

    if ((pname[0] == '|') && (pname.back() == '|') && (pname.size() > 2)) {
        name = pname.substr(1, len-2);
        getSize = true;
    }

    if (name == "sipar") {
        typ = 's';
    } else if ((last(name) == '.') &&(name.size() > 1)) {
        typ = 'v';
        name = head(name);
        two = splitEnding(name, endings);
        if (two.size()==2) {
            ending = two[0];
            restname = two[1];
            name = restname;
        }
    } else if ((last(name) == ':') &&(name.size() > 1)) {
        typ = 'l';
        name = head(name);
        if (last(name) == 'g') {
            isError = true;
            log.logmsg(FATAL, USERDEFNOG, name, "");
        }
    } else if ((last(name) == ')') &&(name.size() > 1)) {
        typ = 'c';
        name = head(name);
        condLevel = countDots(name) + 1;
    } else if (name == "<EOL>") {
        typ = 'n';
    } else if (contains(name, gwords)) {
        typ = 'g';
    } else {
        two = splitStart(name, refs);
        if (two.size()==2) {
            typ = 'r';
            name = two[0];
            restname = two[1];
        } else {
            typ = 'p';
            if (getSize) {
                isError = true;
                log.logmsg(FATAL, BARSONLYFORREF, pname, "");
                getSize = false;
            }
        }
    }
}

Item::~Item()
{
    //dtor
}

void Item::secondRoundRef(vector<string>& snames) {
    // parts
    try {
        determineRefModsAndTyp(snames);
        if (typ == 'r') {
            refdistance = calcDistance(name);
            setRefToFrom();
        }
     } catch(exception &e) {
        string s = e.what();
        isError = true;
        log.logmsg(FATAL, ERRORLEXANLYZEREF, s, entry);
    }
}

void Item::secondRoundVerb(vector<string>& allstems) {
    // parts
    try {
        vector<string> two = splitStart(name, allstems);
        if (two.size()==2) {
            name = two[0];
            restname = two[1];
        } else {
            isError = true;
            log.logmsg(FATAL, MUSTBEVERB, name, "");
        }

        if (contains(name, standard)) {
            verbType = 1;
        } else if (contains(name, other)){
            verbType = 2;
        } else {
            verbType = 3;
        }
        determineVerbMods();
     } catch(exception &e) {
        string s = e.what();
        isError = true;
        log.logmsg(FATAL, ERRORLEXANLYZEREF, s, entry);
    }

    try {
         for (int i=0;i<(int)parts.size(); ++i) {
            string part = parts[i];
            if (part == "khoes") {forget = true;}
            if (!contains(part, vmods)) {
                isError = true;
                log.logmsg(FATAL, VERBMODNOTRECOG, parts[i], "");
            }
        }
     } catch(exception &e) {
        string s = e.what();
        isError = true;
        log.logmsg(FATAL, ERRORLEXANLYZEREF, s, entry);
    }

}

    //split the word
    vector<string> Item::splitByType(string pname, char sep) {
        vector<string> liste;
        string myname = pname;
        int len = pname.size();
        if (sep == '-') {myname = pname.substr(0, len - 1);}

        liste = split(myname, sep);
        return liste;
    }

    string Item::content(bool withType, bool withDiff) {
        string s = "";
        int len = parts.size();
        if (withType) {
           s+= typ;
           s+= "|";
        }
        if (withDiff && (refdistance > 0)) {s+="(" + to_string(refdistance) + ")";}
        s += name;
        if (name == "bes"){s+= "(" + sentenceref + ")";}

        if (len > 0) {
            for (int i = 0; i < len; ++i) {
                s += "+" + parts[i];
                if (parts[i] == "bes"){s+= "(" + sentenceref + ")";}
            }
        }
        if (ending != "") {s+= "-" + ending;};
        if (getSize) {s += "+size";}
        if (typ == 'r') { s += "[" + to_string(refFrom) + ".." + to_string(refTo) + "]";}
        return s;
    }

    char Item::getTyp() {
        return typ;
    }

    string Item::getName() {
        return name;
    }

    string Item::getEnding() {
        return ending;
    }

    int Item::getRefdistance() {
        return refdistance;
    };

    void Item::setRefdistance(int dist) {
        refdistance = dist;
    };

    vector<string> Item::getParts() {
        return parts;
    };

    vector<string> Item::getTrueParts() {
        vector<string> liste;
        for (int i = 0; i < (int) parts.size(); ++i) {
            string p = parts[i];
            if (p[0] != '>') {
                liste.push_back(p);
            }
        }
        return liste;
    };

    int Item::getCondLevel() {
        return condLevel;
    }

    int Item::countDots(string text) {
        int count = 0;
        for (int i = 0; i < (int) text.size(); ++i) {
            if (name[i] == '.') {++count;}
        }
        return count;

    }

    bool Item::isStandardVerb() {
        return verbType == 1;
    };

    bool Item::hasGetSize() {
        return getSize;
    }

     bool Item::hasForget() {
        return forget;
    }

    int Item::getVerbType() {
        return verbType;
    }

    int Item::getRefFrom() {
        return refFrom;
    }

    int Item::getRefTo() {
        return refTo;
    }


    string Item::getSentenceRef() {
        return sentenceref;
    }

    bool Item::hasErrors() {
        return isError;
    }

    void Item::setRefToFrom() {
        if (typ == 'r') {
           for (int i=0;i<(int)parts.size(); ++i) {
                if (parts[i] == "wak") {
                    refFrom = refTo; // last
                } else if (parts[i] == "ci") {
                    refTo -= 1; // head
                } else if (parts[i] == "xo") {
                    refFrom += 1; // tail
                } else {
                    isError = true;
                    log.logmsg(FATAL, REFMODNOTRECOG, parts[i], "");
                }
            }
        }
    }

    void Item::setSipar(int sipar) {
        if (sipar > 0) {
            refFrom += sipar - 1;
            refTo = refFrom;
        }
    }

    int Item::calcDistance(string rmod) {
        if (rmod == "yii") {
            return 1;
        } else if (rmod == "ya") {
            return 2;
        } else if (rmod == "ha") {
            return 3;
        } else if (rmod == "hay") {
            return 4;
        } else if (rmod == "haya") {
            return 5;
        }
        return 0;
    }

    void Item::determineRefModsAndTyp(vector<string>& snames) {
        vector<string> two;
        if (name == "bes") {
            two = splitStart(restname, snames);
            if (two.size()==2) {
                sentenceref = two[0];
                restname = two[1];
            } else {
                isError = true;
                log.logmsg(FATAL, SENTREFNOTFOUND, restname, "");
           }
        }

        if (!isError) {
            while (restname.size() > 0) {
                two = splitStart(restname, rmods);
                if (two.size()==2) {
                    parts.push_back(two[0]);
                    restname = two[1];
                } else {
                    name = entry;
                    typ = 'p';
                    break;
                }
             }
        }
    }


    void Item::determineVerbMods() {
        vector<string> two;
        while (restname.size() > 0) {
            two = splitStart(restname, vmods);
            if (two.size()==2) {
                parts.push_back(two[0]);
                restname = two[1];
                if ((typ == 'v') && (two[0] == "bes")) {
                    sentenceref = restname;
                    restname = "";
                }
            } else if (typ == 'r') {
                name = entry;
                typ = 'p';
                break;
            } else {
                isError = true;
                log.logmsg(FATAL, MODSEQNOTRECOG, restname, "");
                break;
             }
        }
    }


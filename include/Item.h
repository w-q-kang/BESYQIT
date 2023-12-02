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

#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <vector>
#include "Basics.h"
#include "Log.h"

using namespace std;

class Item: public Basics
{
    public:
        Item(string pname);
        virtual ~Item();
        vector<string> splitByType(string pname, char typ);
        string content(bool withType, bool withDiff);
//        bool isRef(string s);
        char getTyp();
        string getName();
        string getEnding();
        int getRefdistance();
        void setRefdistance(int dist);
        vector<string> getParts();
        int countDots(string text);
        int getCondLevel();
        bool isStandardVerb();
        bool hasGetSize();
        bool hasForget();
        int getRefFrom();
        int getRefTo();
        int getVerbType();
        void secondRoundRef(vector<string>& snames);
        void secondRoundVerb(vector<string>& allstems);
        void determineVerbMods();
        void determineRefModsAndTyp(vector<string>& snames);

        vector<string> getSentenceRefnames();
        vector<string> getTrueParts();
        string getSentenceRef();
        bool hasErrors();
        void setSipar(int sipar);
    protected:

    private:
        bool isError;
        int verbType; // 0=no verb,1 =standard, 2 = other, 3 = Paragraph
        bool getSize;
        bool forget;
        char typ; // p=plain, r=ref, v=verb, l=Label, c = condition label, n = newline, s=selector
        string entry; // full name
        string name; // true name without modifiers
        string sentenceref;
        string restname; // modifiers and verb ref.name
        string ending; // verb ending
        int condLevel;
        int refdistance;
        vector<string> parts; //modifiers without name
        int refFrom;
        int refTo;
        void setRefToFrom();
        int calcDistance(string rmod);
        Log log;
};

#endif // ITEM_H

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

#ifndef SENTENCE_H
#define SENTENCE_H

#include "Basics.h"
#include "Item.h"
#include "Log.h"
#include <string>
#include <vector>

using namespace std;

class Sentence : public Basics
{
    public:
        vector<Item> janparams;
        vector<Item> janetparams;
        vector<Item> janxiparams;
        vector<Item> jannyrparams;

        Sentence();
        virtual ~Sentence();
        void addItem(Item item);
        vector<Item> getSentence();
        void setName(string s);
        string getName();
        void setHeader();
        bool isHeader();
        void setSaved();
        bool isSaved();
        vector<int> getThatRefs();
        void setSeq(int s);
        int getSeq();
        string getContentString(bool withType, bool withDiff);
        int getLength();
        Item getItem(int index);
        Item getLastItem();
        void setCondLevel(int s);
        int getCondLevel();
        void setJump(int s);
        int getJump();
        void setLabelType(int lt);
        int getLabeltype();
        void setRefname(string text);
        string getRefname();
        void setParams();
    protected:

    private:
        vector<Item> sentence;
        string name;
        string refname;
        bool header;
        bool saved;
        int seq;
        int condLevel;
        int jump;
        int labelType;
        Log log;
};

#endif // SENTENCE_H

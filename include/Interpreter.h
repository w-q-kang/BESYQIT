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

#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "Log.h"
#include "Sentence.h"
#include "Paragraph.h"
#include "Item.h"
#include "Rowdefs.h"
#include "Opcode.h"
#include "Rowitem.h"
#include "Datetime.h"
#include "Basics.h"
#include "Juubes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

using namespace std;

class Interpreter : public Basics, Juubes
{
    public:
        Interpreter(string wdir, string cdir, bool pLogOutput);
        virtual ~Interpreter();
        void setNames();
        void setParagraphs(vector<Paragraph> v);
        string getNameList();
        vector<string> getTheParam(vector<Item> &items, int sc, vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
           int stackLevel, unordered_map<int, vector<string>> &slists);

        vector<string> getTheReference(Item item, int sc, vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                             unordered_map<int, vector<string>> &slists);

        vector<string> getSlist(unordered_map<int, vector<string>> &slists, int key);
        // exec section
        int execute(vector<string> &pargs);
        vector<string> execParagraph(int index, vector<string> &pthislist, vector<string> &pjanlist, vector<string> &pjanetlist, vector<string> &pjanxilist, vector<string> &pjannyrlist,
                               int stackLevel);

        tuple<int, vector<string>> executeVerb(Item verb, int jump,
                    vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                    int stackLevel);

        tuple<int, vector<string>> executePlainVerb(string name, bool isStandardVerb, int jump, string stype,
                    vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                    int stackLevel);

        tuple<int, vector<string>> executePerRowSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                       vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel);

        tuple<int, vector<string>> executeGainSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                      vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel);

        tuple<int, vector<string>> executeGainabSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                      vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel);

        vector<string> vminus(vector<string> v, vector<string> w);
        void doPrint(vector<string> v, bool withCrlf);
        void doOut(vector<string> v, bool withCrlf);

        vector<string> bulkExecLong(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist);
        vector<string> bulkExecString(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist);
        vector<string> bulkExecFloat(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist);

        long long int getRight(string op, int pre);
        vector<Opcode> toOpcode(vector<string> v);
        int ipower(int b, int e);
        long long int lpower(long long int b, long long int e);
        int getDefLength(string text);
        long long int getRowLength(vector<string> v);
        int newStopExecution(int old, StopTypes stype);
        Rowdefs getPerRowDefs(vector<string>& v);
        vector<Rowitem> getPerRowItems(int start, vector<string>& v);

        vector<bool> getUp(vector<string> therange);
        long long int getRangeIndex(string r, string prev);
        vector<string> getRange(string sfrom, string sto, string sprev, bool up);
        vector<string> fileop(string op, string filename);
        vector<string> timeverb(vector<string> &tv);
        vector<string> freq(vector<string> &dolist,vector<string> &forlist);
        void sortRowsLong(int index, int rows, bool isDesc, vector<string> &v, vector<string> &p);
        void sortRowsString(int index, int rows, bool isDesc, vector<string> &v, vector<string> &p);
//        template <class string> struct gtstoi {
//            bool operator() (const string& x, const string& y) const {return stoi(x)>stoi(y);}
//            typedef string first_argument_type;
//            typedef string second_argument_type;
//            typedef bool result_type;
//        };
//        template <class string> struct ltstoi {
//            bool operator() (const string& x, const string& y) const {return stoi(x)<stoi(y);}
//            typedef string first_argument_type;
//            typedef string second_argument_type;
//            typedef bool result_type;
//        };
        template <class string> struct gtstoll {
            bool operator() (const string& x, const string& y) const {return stoll(x)>stoll(y);}
            //bool operator() (const string& x, const string& y) const {return wxtoll(x)>wxtoll(y);}
            typedef string first_argument_type;
            typedef string second_argument_type;
            typedef bool result_type;
        };
        template <class string> struct ltstoll {
            bool operator() (const string& x, const string& y) const {return stoll(x)<stoll(y);}
            //bool operator() (const string& x, const string& y) const {return wxtoll(x)<wxtoll(y);}
            typedef string first_argument_type;
            typedef string second_argument_type;
            typedef bool result_type;
        };

        int vmemsize(vector<string> v);
        int mmemsize(unordered_map<int, vector<string>> slists);
    int ms = 0;
    protected:

    private:
        vector<Paragraph> paragraphs;
        unordered_map<string, int> names;
        Log logg;
        Level current;
        vector<string> groupNames;
        int stopExecution; //0= dont stop, 1 = jump to paragraph end, 2 = intermediate, 3= stop paragraph and surrounding loop, 9 = stopProgram
        string workDir;
        string currDir;
        vector<string> progargs;
        bool logOutput;
        vector<string> vempty;
        Datetime datetime;
        vector<string> nyrs;
        vector<string> nxis;
        vector<string> nets;
};

#endif // INTERPRETER_H

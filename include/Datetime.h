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

#ifndef DATETIME_H
#define DATETIME_H

#include "Basics.h"
#include "Log.h"

using namespace std;

class Datetime : public Basics
{
    public:
        Datetime();
        virtual ~Datetime();
        void setTimeFormat(string tf);
        void setDateFormat(string df);
        string applyTimeFormat(int ivz, int d, int h, int m, int s);
        string tmtoTime(tm * ltm);
        string applyDateFormat(int d, int m, int y, int wday, int week);
        string tmtoDate(tm * ltm);
        vector<int> splitTime(string t);
        string addTimes(string t1, string t2);
        string addDays(string t1, string t2);
        string subtractDays(string t1, string t2);
        string add(string t1, string t2);
        string subtract(string t1, string t2);
        int stot(string s);
        string ttos(int t);
    protected:

    private:
        string timeformat;
        string dateformat;
        Log log;
};

#endif // DATETIME_H

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


#ifndef JUUBES_H
#define JUUBES_H
#include <string>
#include <vector>
#include "Log.h"

using namespace std;

enum Numtest {STR, NUMINT, NUMFLOAT, JUUINT, JUUFLOAT};

struct {
    bool vz;
    vector<int> digits;
} Intermediate;

class Juubes
{
    public:
        Juubes();
        virtual ~Juubes();

        void setBase(int b);
        int getBase();
        void setMaxdigits(int d);
        int getMaxdigits();

        string ldtos(long double d);

        Numtest testType(string text);
        bool testJuuInt(string text);
        bool testLong(string text);

        // bes
        vector<int> besToVec(string text);
        tuple<int, int> findFirst(string text);
        string vecToBes(vector<int> v);
        string makeLast(string s, int cnt);
        void eliminateTrailing0(int posDot, vector<int>& v);
        // long int
        long long int vecToInt(vector<int>& v);
        vector<int> intToVec(long long int l);

        // double
        vector<int> floatToVec30(long double ld);
        long double vecToFloat(vector<int>& v);

        // mix
        long double besToFloat(string text);
        long long int besToInt(string text);
        string intToBes(long long int l);
        string floatToBes30(long double ld);

        long long int xtoll(string s);
        long double xtold(string s);
    protected:

    private:
        Log logg;
        int basis = 30;
        long double dbasis = 30L;
        int maxdigits = 18;
        vector<string> numerals = {"bu", "te", "da", "sol", "swe", "phoo", "yaak", "serif", "joor", "juban", "neel", "gitaq",
            "hubu", "hute", "huda", "husol", "huswe", "huphoo", "huyaak", "huserif", "hujoor", "hujuban", "huneel", "hugitaq",
            "fybu", "fyte", "fyda", "fysol", "fyswe", "fyphoo", "sak", "-", "."};
};



#endif // JUUBES_H

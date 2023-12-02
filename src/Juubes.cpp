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

    neel
*/


#include "Juubes.h"
#include "globals.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
using namespace std;

Juubes::Juubes()
{
    //ctor
}

Juubes::~Juubes()
{
    //dtor
}

void Juubes::setBase(int b) {
    basis = b;
    dbasis = (long double) basis;
}

int Juubes::getBase() {
    return basis;
}

void Juubes::setMaxdigits(int d) {
    maxdigits = d;
}

int Juubes::getMaxdigits() {
    return maxdigits;
}

// to_string is limited to 6 decimal digits
// therefore must be replaced with a function were we can set maxdigits
// and eliminate trailing zeroes (except for first decimal digit)
string Juubes::ldtos(long double d){
    ostringstream oss;
    oss << setprecision(maxdigits) << noshowpoint << d;
    string s =  oss.str();
    auto pos = s.find('.');
    if (pos == string::npos) {s+=".0";}
    return s;
}

Numtest Juubes::testType(string text){
    if (text.length() == 0) {return STR;}
    bool isStr = false;
    bool hasAlfa = false;
    bool hasDot = false;
    int idxDot = 0;
    bool hasVZ = (text[0] == '+') || (text[0] == '-');
    int istart = (hasVZ) ? 1 : 0;
    int iend = text.length() - 1;
    try {
        for (int i = istart; i <= iend; ++i) {
            char c = text[i];
            if (c == '.') {
                if (hasDot || (i==iend)) {
                    isStr = true;
                    break;
                } else {
                    hasDot = true;
                    idxDot = i;
                }
            } else if ((int(c) < 48 ) || (int(c) > 57)) {
                hasAlfa = true;
            }
        }
    } catch (exception& e) {
        return STR;
    }
    if (isStr) {
        return STR;
    } else if (!hasAlfa) {
        if(hasDot) {
            return NUMFLOAT;
        } else if (testLong(text)) {
            return NUMINT;
        } else {
            return STR;
        }
    } else if (hasDot) {
        int len = idxDot - istart;
        string help = text.substr(istart,len);

        bool res = testJuuInt(help);
        if (!res) {return STR;}
        help = text.substr(idxDot+1);

        res = testJuuInt(help);
        if (!res) {return STR;}
        return JUUFLOAT;
    } else {
        if (testJuuInt(text.substr(istart))) {
            return JUUINT;
        } else {
            return STR;
        }
    }

    return STR;

}
// tests, if a string that seems to be long fits in size
bool Juubes::testLong(string text) {
    bool res = true;
    try {
        long long int i __attribute__((unused)) = stoll(text);
    } catch (exception& e) {
        res = false;
    }

    return res;
}


    // simple test, without dots and signs
    bool Juubes::testJuuInt(string text) {
        vector<int> res;
        if (text.length() > 0) {
            res = besToVec(text);
            return res.size() > 0;
        }
        return false;
    }


// ===== Bes <-> Vector ==================================================
vector<int> Juubes::besToVec(string text) {
    vector<int> result;
    string s = text;
    int i = 0;
    int h = 0;
    int i1 = 0;
    int h1 = 0;
    int last = 0;

    while (s.length() > 0) {
        tie(i,h) = findFirst(s);
        if (i>=0) {
            s= ((int) s.length()==h) ? "" : s.substr(h);
            if (i==30) {
                tie(i1,h1) = findFirst(s);
                s= ((int) s.length()==h1) ? "" : s.substr(h1);
                for (int j = 1; j < i1; ++j) {
                    result.push_back(last);
                }
            } else {
                result.push_back(i);
                last = i;
            }
        } else {
            break;
        }
    }

    if (s.size() > 0) {result.clear();}
    return result;
}

tuple<int, int> Juubes::findFirst(string text) {
    int i = 0;
    while (i < (int) numerals.size()) {
        int h = numerals[i].size();
        if (text.substr(0, h) == numerals[i]) {
            return tuple(i, h);
        }
        i++;
    }

    return make_tuple(-1,0);
}


string Juubes::vecToBes(vector<int> v) {
    string result = "";
    int k = 0;
    int cnt = 0;
    int last = -1;

    auto idxPos = find(v.begin(), v.end(), 32);
    if (idxPos != v.end()) {
        int idx = idxPos - v.begin();
        eliminateTrailing0(idx, v);
    }

    try {
        int iend = (int) v.size() - 1;
        for (int i = 0; i <= iend; ++i) {
            k = v[i];
            if (k==last) {
                cnt++;
            } else  {
                if (last>=0) {result += makeLast(numerals[last], cnt+1);}
                cnt=0;
            }
             last = k;
        }

        if (last>=0) {
           result += makeLast(numerals[last], cnt+1);
        }
     } catch (exception& e) {
       string s = e.what();
       logg.logmsg(ERROR, MSG1, s, "");
    }
    return result;
}

void Juubes::eliminateTrailing0(int posDot, vector<int>& v) {
    int i = v.size() - 1;
    if (posDot >= 0) {
        while (i > posDot+1) {
            if (v[i]== 0) {
                 v.pop_back();
            } else {
                break;
            }
            i--;
        }
    }
}

// ===== Long Long Int <-> Vector ==================================================
vector<int> Juubes::intToVec(long long int l) {
    vector<int> result;
    int d;
    long long int r;
    bool isNeg = l < 0;
    if (l < 0) {
        l = -l;
    }

    if (l==0) {
        result.push_back(0);
    } else {
        while(l > 0) {
            d = l % basis;
            r = (l - d) / basis;
            result.insert(result.begin(), d);
            l = r;
        }
    }

    if (isNeg) {
        result.insert(result.begin(), 31);
    }
    return result;
}


long long int Juubes::vecToInt(vector<int>& v) {
    int i = 0;
    int iend = v.size() - 1;
    long long int l = 0;
    bool isNeg = false;
    while (i <= iend) {
        if (v[i] == 31) {
            isNeg = true;
        } else if (v[i] == 32) {
            // found dec. point
            break;
        } else {
            l = l * basis + v[i];
        }
        i++;
    }

    return (isNeg) ? -l : l;
}


// ===== Long Double <-> Vector ==================================================

vector<int> Juubes::floatToVec30(long double ld) {
    vector<int> v;
    string s = ldtos(ld);
    auto pos = s.find('.');

    if (pos == string::npos) {
       v = intToVec(stoll(s));
    } else {
       int x = stoll(s.substr(0, pos));
       vector<int> w;
       s = s.substr(pos+1);
       int iend = (int) s.size() - 1;

       long long int lb = 3;
       long long int l;

       // build up list
       for (int i = 0; i<=iend; ++i) {
            string c (1, s[i]);
            l = stoll(c) * lb;
            w.push_back(l);
            lb *= 3;
       }

       // sum up
       int carry = 0;
       iend = (int) w.size() - 1;
       for (int i = iend; i>=0; --i) {
           int f = w[i] + carry;
           int m = f % 30;
           int r = f - m;
           carry = r / 30;
           w[i] =  m;
       }

       if (x>=0) {
            x+= carry;
       } else {
            x-= carry;
       }

       v = intToVec(x);
       v.push_back(32);
       v.insert(v.end(), w.begin(), w.end());
    }
    return v;
}

long double Juubes::vecToFloat(vector<int>& v) {
    int i = 0;
    int iend = v.size() - 1;
    long double d = 0;
    bool isNeg = false;
    bool isFrac = false;
    long double db = dbasis;

    while (i <= iend) {
        if (v[i] == 31) {
            isNeg = true;
        } else if (v[i] == 32) {
            isFrac = true;
        } else {
            if (isFrac) {
                d = d + ((long double) v[i]) / db;
                db *= dbasis;
            } else {
                d = d * dbasis + (long double) v[i];
            }
         }
        i++;
    }

    return (isNeg) ? -d : d;
}


// ===== Num <-> Bes ==================================================
long double Juubes::besToFloat(string text) {
    vector<int> v = besToVec(text);
    return vecToFloat(v);
}

long long int Juubes::besToInt(string text) {
    vector<int> v = besToVec(text);
    return vecToInt(v);
}

string Juubes::intToBes(long long int l) {
    vector<int> v = intToVec(l);
    return vecToBes(v);
}

string Juubes::makeLast(string s, int cnt) {
    string result = "";
    try {
        if ((s == ".") || (s == "-") || (cnt <= 2)) {
            for (int i = 1; i <= cnt; ++i) {result += s;}
        } else if (cnt>2) {
            result = s + "sak" + numerals[cnt];
        }
    } catch (exception& e) {
       string s = e.what();
       logg.logmsg(ERROR, MSG1, s, "");
    }

    return result;
}

string Juubes::floatToBes30(long double ld) {
    if (debug_q) {logg.logdbg3(NOWCONVERTING, ldtos(ld), "", "");}
    vector<int> v = floatToVec30(ld);
    return vecToBes(v);
}

long long int Juubes::xtoll(string s) {
    Numtest nt = testType(s);
    if (nt == JUUINT) {
        return besToInt(s);
    } else if (nt == NUMINT) {
        return stoll(s);
    } else {
        return 0;
        logg.logmsg(ERROR, NOTANINT, s, "");
    }
}

long double Juubes::xtold(string s) {
    Numtest nt = testType(s);
    if (nt == JUUINT) {
        return (long double) besToInt(s);
    } else if (nt == NUMINT) {
        return (long double) stoll(s);
    } else if (nt==JUUFLOAT) {
        return besToFloat(s);
    } else if (nt==NUMFLOAT) {
        return stold(s);
    } else {
        return 0;
        logg.logmsg(ERROR, NOTANUMBER, s, "");
    }
}

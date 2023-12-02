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

#include "Datetime.h"
#include <sstream>
#include <iomanip>
#include <chrono>
//#include "Log.h"

using namespace std;

Datetime::Datetime()
{
    //ctor
    timeformat = "p:m:w";
    dateformat = "W.S.j";

}

Datetime::~Datetime()
{
    //dtor
}

string Datetime::add(string t1, string t2) {
    if (findInString('.', t1) || findInString('/', t1)) {
        return addDays(t1, t2);
    } else {
        return addTimes(t1, t2);
    }
}

string Datetime::subtract(string t1, string t2) {
    if (findInString('.', t1) || findInString('/', t1)) {
        return subtractDays(t1, t2);
    } else {
        return addTimes(t1, "-" + t2);
    }
}


// --------------- TIME ---------------------------------------

void Datetime::setTimeFormat(string tf) {
        timeformat = tf;
    }



string Datetime::tmtoTime(tm * ltm) {
        int h = 0+ltm->tm_hour;
        int m = 0 + ltm->tm_min;
        int s = 0 + ltm->tm_sec;
        return applyTimeFormat(1,0,h,m,s);
}

string Datetime::applyTimeFormat(int ivz, int d, int h, int m, int s){
        string result = "";
        string help = "";
        bool zeroes = false;
        string vz = (ivz<0) ? "-" : "";
        for (int i = 0; i < (int) timeformat.size(); ++i) {
            char c = timeformat[i];
            switch (c) {
            case 'c':
                help = to_string(d);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'p':
                help = to_string(h);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'm':
                help = to_string(m);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'w':
                help = to_string(s);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case ':':
                result += ":";
                break;
            case 'f':
                result += " ";
                break;
            case 's':
                zeroes = true;
                break;
            default:
                result += ctos(c);
            }
        }

        return vz+result;
}

string Datetime::addTimes(string t1, string t2) {
    int v = stot(t1) + stot(t2);
    return ttos(v);
}

int Datetime::stot(string s) {
    int vz = (s[0] == '-') ? -1 : 1;
    vector<string> parts = split(s, ':');
    int l = parts.size();
    vector<int> f = {86400, 3600, 60, 1};
    int result = 0;
    for (int i = l-1; i>=0;--i) {
        result =  f.at(i+4-l) * abs(stoi(parts.at(i))) + result;
    }
    return vz*result;
}

string Datetime::ttos(int t) {
    int vz = (t<0) ?  -1 : 1;
    int r = abs(t);
    int d = r / 86400;
    r = r % 86400;
    int h = r / 3600;
    r = r - h * 3600;
    int m = r / 60;
    r = r - m * 60;
    int s = r;
    return applyTimeFormat(vz,d,h,m,s);
}


// ------ DATE -------------------------------------------

void Datetime::setDateFormat(string df) {
        dateformat = df;
    }

string Datetime::tmtoDate(tm * ltm) {
        int d= ltm->tm_mday;
        int m = 1 + ltm->tm_mon;
        int y = ltm->tm_year - 100;
        int w = ltm->tm_wday;
        if (w==0) {w=7;}
        char buffer[5];
        // that damned function doesnt recognize %V though it should..
        int ck = strftime(buffer,5,"%W", ltm);
        string s = "";
        s.push_back(buffer[0]);
        if (ck>1) {s.push_back(buffer[1]);}
        int week = stoi(s);
        return applyDateFormat(d,m,y,w,week);
}

string Datetime::applyDateFormat(int d, int m, int y, int wday, int week){
        string result = "";
        string help = "";
        bool zeroes = false;
        for (int i = 0; i < (int) dateformat.size(); ++i) {
            char c = dateformat[i];
            switch (c) {
            case 'W':
                help = to_string(d);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'S':
                help = to_string(m);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'j':
                result += to_string(y);
                break;
            case 'C':
                result += to_string(y+2000);
                break;
            case 'b':
                help = to_string(week);
                if (zeroes && (help.size()==1)) {help = "0" + help;}
                result += help;
                break;
            case 'g':
                result += to_string(wday);
                break;
            case '.':
                result += ".";
                break;
            case '/':
                result += "/";
                break;
            case 'f':
                result += " ";
                break;
            case 's':
                zeroes = true;
                break;
            default:
                result += ctos(c);
            }
        }

        return result;
}


string Datetime::addDays(string t1, string t2) {
    tm gtm = {};
    int days = stoi(t2);
    istringstream ss(t1);

    if (findInString('.', t1)) {
        ss >> get_time(&gtm, "%d.%m.%Y");
    } else {
        ss >> get_time(&gtm, "%d/%m/%Y");
    }

    time_t tt = mktime(&gtm);
    chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(tt);
    tp += chrono::hours(24*days);
    tt = chrono::system_clock::to_time_t(tp);
    tm * rtm = localtime(&tt);
    return tmtoDate(rtm);
}

string Datetime::subtractDays(string t1, string t2) {
    tm gtm = {};
    int days = stoi(t2);
    istringstream ss(t1);

    if (findInString('.', t1)) {
        ss >> get_time(&gtm, "%d.%m.%Y");
    } else {
        ss >> get_time(&gtm, "%d/%m/%Y");
    }

    time_t tt = mktime(&gtm);
    chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(tt);
    tp -= chrono::hours(24*days);
    tt = chrono::system_clock::to_time_t(tp);
    tm * rtm = localtime(&tt);
    return tmtoDate(rtm);
}


vector<int> Datetime::splitTime(string t) {
    vector<string> parts = split(t, ':');
    vector<int> result;
    for (int i = 0; i < (int) parts.size(); ++i) {
        result.push_back(stoi(parts[i]));
    }
    return result;
}

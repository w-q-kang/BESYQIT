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

#ifndef LOG_H
#define LOG_H
#include <string>
#include "Enums.h"
#include <chrono>
#include <map>
#include <vector>

using namespace std;

class Log
{
    public:
        static bool useconsole;
        static string filename;
        static Level level;
        static int language;
        static bool init;
        static map<LogMsg,vector<string>> messagemap;
        static map<LogDbg,vector<string>> debugmap;
        Log();
        virtual ~Log();
        string getMessage(LogMsg lmsg);
        string getMessage1(LogMsg lmsg, string par);
        string getDebugMsg(LogDbg lmsg);
        void msg(Level lv, string text);
        void logmsg(Level lv, LogMsg lmsg, string par1, string par2);
        void logmsg3(Level lv, LogMsg lmsg, string par1, string par2, string par3);
        void logmsg4(Level lv, LogMsg lmsg, string par1, string par2, string par3, string par4);
        void logdbg3(LogDbg lmsg, string par1, string par2, string par3);
        void plain(string text);
        void debug(string text);
        void debugtime(LogDbg lmsg, string par1, string par2);
        void setFile(string name);
        void setLevel(Level level);
        void setUseConsole(bool uc);
        void setLanguage(int lng);
        string levelString(Level level);
        Level getLevel();
        string datum();
        chrono::high_resolution_clock::time_point tstart = chrono::high_resolution_clock::now();
    protected:

    private:


};

#endif // LOG_H

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
#ifndef COMPLIANCE_H
#define COMPLIANCE_H
#include "Enums.h"
#include <string>
#include <vector>
#include <tuple>

using namespace std;

class Compliance
{
    public:
        Compliance();
        virtual ~Compliance();
        LogMsg check(int phase, string cluster);
        char getType(int phase, char c);
        bool isVowel(int phase, char c);
        bool isConsonant(int phase, char c);
        tuple<LogMsg, string> test(string word);
        string expand(string word);
    protected:

    private:
        vector<char> vowels = {'a','e','i','o','u'};
        vector<char> consonants = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x', 'z'};
        vector<string> pre = {"ph","th","ch","kh","pr","kr","sw","sh","nn","mm","ff"};
        vector<string> post = {"ss", "mm","nn"};
        vector<char> digits = {'0','1','2','3','4','5','6','7','8','9'};
        vector<string> juubes = {"bu", "te", "da", "sol", "swe", "phoo", "yaak", "serif", "joor", "juban"};
};

#endif // COMPLIANCE_H

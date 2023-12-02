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

#include "globals.h"

using namespace std;

 vector<string> refs = {"ikyxi", "innyr", "haya", "iket", "hay", "yii", "bes", "san", "ik", "ha", "ya"}; //must be in this order
 vector<string>  gwords = {"janet", "jannyr", "janxi", "jan"};
        // standard verbs, must not contain: plist, between, nop, stop
 vector<string> standard = {"hek", "phe", "ryy", "beqo", "soon", "faon", "sip", "wah", "dii", "dum", "sil", "kum", "mud", "silav",
            "haanav", "khil", "haan", "vae", "roc", "nna", "cop", "khi", "ria", "voos", "kumoryy", "joq", "ren", "fiiar"};
 vector<string> other = {"bai", "suun", "mu", "sec"};
 vector<string> vmods = {"bool", "pri", "thek", "fee", "khoes", "bes"};
 vector<string> endings = {"gete", "gainab", "gana", "gain", "ge", "ainab", "ete", "ana", "ain", "e"};
 vector<string> rmods = {"ci", "xo", "wak"};

 bool isWeak = false;

bool debug_L = false;
bool debug_P = false;
bool debug_I = false;
bool debug_b = false;
bool debug_c = false;
bool debug_i = false;
bool debug_j = false;
bool debug_q = false;
bool debug_r = false;
bool debug_s = false;
bool debug_t = false;
bool debug_v = false;
bool debug_x = false;
bool debug_y = false;

int maxfracdigits = 15;


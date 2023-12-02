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
   string::npos
*/
//
#include "Interpreter.h"
#include "globals.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <math.h>
#include <filesystem>
#include <map>

// catch
using namespace std;

Interpreter::Interpreter(string wdir, string cdir, bool pLogOutput)
{

    stopExecution = 0;
    logOutput = pLogOutput;
    groupNames = {"jan", "janet", "janxi", "jannyr"};
    nyrs = {"bai", "beqo", "cop", "fiiar", "faon", "khi", "kum", "mud", "phe", "ren", "ris", "sil", "silav", "sip", "soon", "suun"};
    nxis = {"haan", "haanv", "joq", "hek", "khi", "mu", "mud", "nna", "sil", "suun", "vae", "voos", "wah"};
    nets = {"bai", "beqo", "cop", "dum", "fiiar", "faon", "joq", "khil", "kum", "kumoryy", "phe", "ren", "ria", "roc", "ryy",
    "sec", "silav", "sip", "soon", "suun"};
    current = logg.getLevel();
    logg.logmsg(INFO, CURRENTLOGLEVEL, to_string(current), "");
    workDir = wdir;
    if (last(workDir) != '\\') {
        workDir.push_back('\\');
    }
    logg.logmsg(INFO, WORKDIR, workDir, "");
    currDir = cdir;
    if (last(currDir) != '\\') {
        currDir.push_back('\\');
    }
    logg.logmsg(INFO, CURRENTDIR, currDir, "");
}

Interpreter::~Interpreter()
{
}

void Interpreter::setParagraphs(vector<Paragraph> v) {
    paragraphs = v;
    setNames();
};

// should never return FATAL errors any more, since these will be detected when parsing
void Interpreter::setNames() {
    for (int i = 0; i < (int) paragraphs.size(); ++i) {
        Paragraph p = paragraphs[i];
        if (!p.isHeader()) {
            string name = p.getName();
            if (name.size() > 0) {
               if (names.find(name) == names.end()) {
                    names[name] = i;
               } else {
                    logg.logmsg(FATAL, PARAGRAPHMULTIPLE, name, "");
                    current = FATAL;
                    break;
                }
            } else {
                logg.logmsg(FATAL, PARAGRAPHUNNAMED, to_string(i), "");
                current = FATAL;
                break;
            }
        }
    };
}

int Interpreter::execute(vector<string> &pargs) {
    if (current == FATAL ) {return -1;}
    progargs = pargs;
    vector<string> thislist;
    vector<string> janlist;
    vector<string> janetlist;
    vector<string> janxilist;
    vector<string> jannyrlist;
    execParagraph(0, thislist, janlist, janetlist, janxilist, jannyrlist, 0);
    return 0;
}

string Interpreter::getNameList() {
    string result = "";
    for (auto & it : names) {
        result += it.first + " = " + to_string(it.second) + "; ";
    }
    return result;
}

vector<string> Interpreter::execParagraph(int index, vector<string> &pthislist, vector<string> &pjanlist, vector<string> &pjanetlist, vector<string> &pjanxilist, vector<string> &pjannyrlist,
                               int stackLevel) {
    string hint = "begin";
    if (stopExecution > 0) {
        stopExecution = newStopExecution(stopExecution, PARAGRAPH);
        return pthislist;
    }
    int offset=0;
    vector<string> totalrv;
    // needed as starting value for sentence loop
    vector<string> outthislist=pthislist;

    if (debug_c) {logg.logdbg3(PARAGRAPHENTER, to_string(index), to_string(stackLevel), "");};
    Paragraph p = paragraphs[index];
    unordered_map<int, vector<string>> slists; //sentence result lists, will change
    int scmax = p.getLength();
    int sc= 0;

    while (sc < scmax) {
        Sentence sentence = p.getSentence(sc);
        try {
            hint = "preprocess";
            vector<string> janlist;
            vector<string> janetlist;
            vector<string> janxilist;
            vector<string> jannyrlist;
            hint = "jan";
            if (sentence.janparams.size() > 0) {janlist = getTheParam(sentence.janparams, sc,
                outthislist, pjanlist, pjanetlist, pjanxilist, pjannyrlist, stackLevel, slists);}

            hint = "janet";

            if (sentence.janetparams.size() > 0) {janetlist = getTheParam(sentence.janetparams, sc,
                 outthislist, pjanlist, pjanetlist, pjanxilist, pjannyrlist,  stackLevel, slists);}
            hint = "janxi";

            if (sentence.janxiparams.size() > 0) {janxilist = getTheParam(sentence.janxiparams, sc,
                 outthislist, pjanlist, pjanetlist, pjanxilist, pjannyrlist,  stackLevel, slists);}
            hint = "jannyr";

            if (sentence.jannyrparams.size() > 0) {jannyrlist = getTheParam(sentence.jannyrparams, sc,
                 outthislist, pjanlist, pjanetlist, pjanxilist, pjannyrlist,  stackLevel, slists);}

            hint = "execVerb";
            Item item = sentence.getLastItem();

            if (item.hasForget()) {
                //int before = mmemsize(slists) + vmemsize(pdolist) + vmemsize(pforlist) + vmemsize(puselist) + vmemsize(pwithlist) + vmemsize(pthislist) + vmemsize(totalrv);
                slists.clear();
                pjanlist.clear();pjanlist.shrink_to_fit();
                pjanetlist.clear();pjanetlist.shrink_to_fit();
                pjanxilist.clear();pjanxilist.shrink_to_fit();
                pjannyrlist.clear();pjannyrlist.shrink_to_fit();
                //int after = mmemsize(slists) + vmemsize(pdolist) + vmemsize(pforlist) + vmemsize(puselist) + vmemsize(pwithlist) + vmemsize(pthislist) + vmemsize(totalrv);
            }

            tie(offset, outthislist) = executeVerb(item, sentence.getJump(), outthislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);

            hint = "addRV";
            if (item.getName() == "nna") {addToVector(totalrv, outthislist);};

            hint = "isSaved";
            if (sentence.isSaved()) {
                slists[sentence.getSeq()] = outthislist;
            }
        } catch (exception& e) {
            string s(e.what());
            logg.logmsg4(FATAL, ERRINSENTOFPAR, s, to_string(sc), p.getName(), to_string(stackLevel));
            logg.logmsg(FATAL, ERREXECSENTENCE, sentence.getLastItem().getName(), hint);
            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
            stopExecution = 9;
        }

        if ((stopExecution >0) || (offset <= - 1000)) {break;}
        sc+= offset;
    }

    if (debug_c) {logg.logdbg3(PARAGRAPHLEAVE, to_string(index), "", "");};
    stopExecution = newStopExecution(stopExecution, PARAGRAPH);
    return totalrv;
}

vector<string> Interpreter::getTheParam(vector<Item> &items, int sc, vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
           int stackLevel, unordered_map<int, vector<string>> &slists) {
    vector<string> result;
    string hint = "begin";

    try {
        vector<string> help;
        int len = items.size();

        for (int i = 0; i < len; ++i) {
            Item item = items[i];
            char typ = item.getTyp();
            hint = "item " + to_string(i) + " of typ " + ctos(typ);

            if (typ == 'p') {
                result.push_back(item.getName());
            } else if (typ == 'r') {
               help.clear();
               help = getTheReference(item, sc, thislist, janlist, janetlist, janxilist, jannyrlist, slists);
               if (debug_r) {logg.logdbg3(REFEVALTO, item.getName(), join(help, ","), "");};
               addToVector(result, help);
            } else if ((typ == 'l') || (typ == 'g') || (typ == 'n') || (typ == 'l')|| (typ == 'v')|| (typ == 'c')) {
                // ignore

            } else {
                string s = "";
                s.push_back(typ);
                logg.logmsg(WARN, UNKNOWNTYPE, s, "");
            }
        }
    } catch (exception& e) {
        logg.logmsg4(FATAL, ERRINPREP, cstos(e.what()), to_string(sc), to_string(stackLevel), hint);
        if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
        stopExecution = 9;
    }

    return result;
}

vector<string> Interpreter::getTheReference(Item item, int sc, vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                             unordered_map<int, vector<string>> &slists) {
    vector<string> result;
    string name = item.getName();
    //string help = "";
    int refFrom = item.getRefFrom();
    int refTo = item.getRefTo();
    int ihelp = 0;
    try {
        // clumsy but (hopefully) faster
        if (item.hasGetSize()) {
            if (name == "yii") {
                ihelp = thislist.size();
            } else if (name == "ik") {
                ihelp = janlist.size();
            } else if (name == "iket") {
                ihelp = janetlist.size();
            } else if (name == "ikyxi") {
                ihelp = janxilist.size();
            } else if (name == "innyr") {
                ihelp = jannyrlist.size();
            } else if (name == "san") {
                ihelp = progargs.size();
            }else {
                int refsc = item.getRefdistance();
                refsc = sc - refsc;
                ihelp= getSlist(slists, refsc).size();
            }
            ihelp = ihelp + refTo - refFrom + 1;
            result.push_back(intToBes(ihelp));

        // not modified => use original
        } else if ((refFrom == 0) && (refTo == -1)) {
            if (name == "yii") {
                return thislist;
            } else if (name == "ik") {
                return janlist;
            } else if (name == "iket") {
                return janetlist;
            } else if (name == "ikyxi") {
                return janxilist;
            } else if (name == "innyr") {
                return jannyrlist;
            } else if (name == "san") {
                return progargs;
            }else {
                int refsc = item.getRefdistance();
                refsc = sc - refsc;
                return getSlist(slists, refsc);
            }
          // index
        } else if (refFrom == refTo) {
            ihelp = refFrom;

            if (name == "yii") {
                if (ihelp<0) {ihelp = thislist.size() + ihelp;}
                result.push_back(thislist.at(ihelp));
            } else if (name == "ik") {
                if (ihelp<0) {ihelp = janlist.size() + ihelp;}
                result.push_back(janlist.at(ihelp));
            } else if (name == "iket") {
                if (ihelp<0) {ihelp = janetlist.size() + ihelp;}
                result.push_back(janetlist.at(ihelp));
            } else if (name == "ikyxi") {
                if (ihelp<0) {ihelp = janxilist.size() + ihelp;}
                result.push_back(janxilist.at(ihelp));
            } else if (name == "innyr") {
                if (ihelp<0) {ihelp = jannyrlist.size() + ihelp;}
                result.push_back(jannyrlist.at(ihelp));
            } else if (name == "san") {
                if (ihelp<0) {ihelp = progargs.size() + ihelp;}
                result.push_back(progargs.at(ihelp));
            } else {
                int refsc = item.getRefdistance();
                refsc = sc - refsc;
                if (ihelp<0) {
                    ihelp = getSlist(slists, refsc).size() + ihelp;
                }
                result.push_back(getSlist(slists, refsc).at(ihelp));
            }
          // construct range here
        } else {
             if (name == "yii") {
                result = thislist;
            } else if (name == "ik") {
                result = janlist;
            } else if (name == "iket") {
                result = janetlist;
            } else if (name == "ikyxi") {
                result = janxilist;
            } else if (name == "innyr") {
                result = jannyrlist;
            } else if (name == "san") {
                result = progargs;
            } else {
                int refsc = item.getRefdistance();
                refsc = sc - refsc;
                result = getSlist(slists, refsc);
            }
            if (refFrom > 0) {
                result.erase(result.begin(), result.begin() + refFrom);
            }

            if (refTo < -1) {
                for (int i = 1; i < abs(refTo); ++i) {result.pop_back();}
            }
        }
    } catch (exception& e) {
        logg.logmsg4(FATAL, ERRWHENPROCESSING, cstos(e.what()), name, to_string(refFrom), to_string(refTo));
        if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
        stopExecution = 9;
    }

    return result;
};

vector<string> Interpreter::getSlist(unordered_map<int, vector<string>> &slists, int key) {
    if (slists.find(key) != slists.end()) {
        return slists[key];
    } else {
        logg.logmsg(FATAL, SENTENCEREFNOTFOUND, to_string(key), "");
        if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
        stopExecution = 9;
        return {};
    }
};

tuple<int, vector<string>> Interpreter::executeVerb(Item verb, int jump,
                    vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                    int stackLevel) {
    string name = verb.getName();
    if (debug_s) {logg.logdbg3(EXECVERB, name, "", "");};
    int offset;
    vector<string> p;
    vector<string> modifiers = verb.getParts();
    string ending = verb.getEnding();
    string stype = "";
    int verbmode = 0;
    bool echoOut = false;

    for (int i = 0; i < (int) modifiers.size(); ++i) {
        // long
        if (modifiers[i] == "thek")  {
            stype = "thek";
            if ((name != "bai")&& (name != "khi")&& (name != "khil")) {
                logg.logmsg(WARN, MODIFIERIGNORED, "thek", name);
            }
        // float
        } else if (modifiers[i] == "fee") {
            stype = "fee";
            if ((name != "bai")&& (name != "khil")) {
                logg.logmsg(WARN, MODIFIERIGNORED, "fee", name);
            }
        // output before - outputs
        } else if (modifiers[i] == "bool")  {
            if (janetlist.size()>0) {
                doOut(janetlist, true);
                if (logOutput) {logg.plain(join(janetlist, " "));}
            }
            if (janxilist.size()>0) {
                doOut(janxilist, true);
                if (logOutput) {logg.plain(join(janxilist, " "));}
            }
            if (jannyrlist.size()>0) {
                doOut(jannyrlist, true);
                if (logOutput) {logg.plain(join(jannyrlist, " "));}
            }
        // output after
        } else if (modifiers[i] == "pri")  {
            echoOut = true;
        }
    }

    if ((ending == "ge") || (ending == "e")) {
            verbmode = 1;
    } else if ((ending == "gete") || (ending == "ete")) {
            verbmode = 2;
    } else if ((ending == "gain") || (ending == "ain")) {
            verbmode = 3;
    } else if ((ending == "gainab")|| (ending == "ainab")) {
            verbmode = 4;
    }

    if (verbmode == 1) {
        tie(offset, p) = executePerRowSentence(name, verb.isStandardVerb(),jump, stype, ending, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);
    } else if (verbmode == 2) {
        tie(offset, p) = executePerRowSentence(name, verb.isStandardVerb(),jump, stype, ending, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);
    } else if (verbmode == 3) {
        tie(offset, p) = executeGainSentence(name, verb.isStandardVerb(),jump, stype, ending, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);
    } else if (verbmode == 4) {
         tie(offset, p) = executeGainabSentence(name, verb.isStandardVerb(),jump, stype, ending, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);
    } else {
        tie(offset, p) = executePlainVerb(name, verb.isStandardVerb(), jump, stype, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel);
    }

    if (echoOut)  {
        doOut(p, true);
        if (logOutput) {logg.plain(join(p, " "));}
    }

    return make_tuple(offset, p);
};

tuple<int, vector<string>> Interpreter::executePlainVerb(string name, bool isStandardVerb, int jump, string stype,
                    vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist,
                    int stackLevel) {
    vector<string> p;
    p.clear();
    int offset = jump;
    int newse = stopExecution;
    int elen = janetlist.size();
    int xlen = janxilist.size();
    int nlen = jannyrlist.size();
    long long int ihelp = 0;
    string help = "";
    string progress = "begin";


    // no operation
    if (name == "mu") {
        p = thislist;
    // stop
    } else if (name == "sec") {
        try {
            offset = -1000;
            newse = 1;
            if (janetlist.size() > 0) {
                if (janetlist[0] == "qit") {
                    newse = 9;
                } else if (janetlist[0] == "gejuc") {
                    newse = 3;
                }
            }
            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), to_string(newse), "");};
            stopExecution = newse;
            p = thislist;
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "sec", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
           stopExecution = 9;
        }
    // between
    } else if (name == "suun") {
        try {
            if (xlen==0) {
                p = janetlist;
            } else if (elen>0) {
                long long int dist = 1;
                long long int distcount = 0;
                if (nlen>0) {
                    dist = xtoll(jannyrlist[0]);
                }
                for (int i = 0; i < elen - 1; ++i) {
                    p.push_back(janetlist[i]);
                    distcount++;
                    if (dist == distcount) {
                        for (int j = 0; j < xlen; ++j) {
                            p.push_back(janxilist[j]);
                        }
                        distcount=0;
                    }
                }
                p.push_back(janetlist[elen-1]);
            }
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "suun", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
           stopExecution = 9;
        }
    // plist
    } else if (name == "bai") {
        try {
            if (stype == "thek") {
                p = bulkExecLong(MULTI, jannyrlist, janetlist, janxilist, janlist);
            } else if (stype == "fee") {
                p = bulkExecFloat(MULTI, jannyrlist, janetlist, janxilist, janlist);
            }else {
                p = bulkExecString(MULTI, jannyrlist, janetlist, janxilist, janlist);
            }
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "bai", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
           stopExecution = 9;
        }
    // delete
    } else if (name == "phe") {
        try {
            if (nlen < 100) {
                for (int i = 0; i < elen; ++i) {
                    //help = intToJuubes(i+1);
                    help = intToBes(i+1);
                    if(!contains(help, jannyrlist)) {
                        p.push_back(janetlist[i]);
                    }
                }
            } else {
               // if del list is long use more efficient algorithm
                unordered_map<int, int> f;
                f.clear();
                for (int i = 0; i < nlen; ++i) {
                    ihelp = xtoll(jannyrlist[i]);
                    if (f.find(ihelp) == f.end()) {
                         f.insert(pair<int, int>(ihelp, 0));
                    }
                }
                for (int i = 0; i < elen; ++i) {
                    ihelp = i+1;
                    if (f.find(ihelp) == f.end()) {
                        p.push_back(janetlist[i]);
                    }
                }

                f.clear();
            }
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "phe", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(stopExecution), "9", "");};
           stopExecution = 9;
        }
    // expand -------------------------------
    } else if (name == "ryy") {
        //split every char (except space, tab)
        for (int i = 0; i < elen; ++i) {
            string x = janetlist[i];
            for (int j = 0; j < (int) x.size(); ++j) {
                char c = x[j];
                if ((c != ' ') && (c != '\t')) {p.push_back(ctos(x[j]));};
            }
        }
    // cond --------------------------------------
    } else if (name == "hek") {
        if (xlen == 0) {
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "hek", to_string(stopExecution), "9");};
           stopExecution = 9;
        } else {
            help = janxilist[0];
            if (help == "thek") {
                if (debug_v) {logg.logdbg3(CONDITIONIS, "thek", join(janxilist, " "), "");};
                p = bulkExecLong(SINGLE, janxilist, vempty, vempty, vempty);
                if (debug_v) {logg.logdbg3(CONDITIONRESULT, join(p, " "), "", "");};
            } else if (help == "fee") {
                if (debug_v) {logg.logdbg3(CONDITIONIS, "fee", join(janxilist, " "), "");};
                p = bulkExecFloat(SINGLE, janxilist, vempty, vempty, vempty);
                if (debug_v) {logg.logdbg3(CONDITIONRESULT, join(p, " "), "", "");};
            } else {
                if (debug_v) {logg.logdbg3(CONDITIONIS, "<string>", join(janxilist, " "), "");};
                p = bulkExecString(SINGLE, janxilist, vempty, vempty, vempty);
                if (debug_v) {logg.logdbg3(CONDITIONRESULT, join(p, " "), "", "");};
            }
           // condition fulfilled => goto next
           if (p.size() > 0) {
                if (help == "fee") {
                    long double res = xtold(p[0]);
                    if (res > 0) {offset = 1;}
                } else {
                    long long int res = xtoll(p[0]);
                    if (res > 0) {offset = 1;}
                }
           }
       }
    // file --------------------------------------
     } else if (name == "soon") {
        int i = 0;
        int j = 0;
        if ((elen>0) && (nlen>0)) {
            while ((i<nlen) && (j<elen)) {
                string op = jannyrlist[i];
                if (op == "rimoo") {
                    i++;
                    op += " " + jannyrlist[i];
                }
                p = fileop(op, janetlist[j]);
                i++; j++;
                if ((i==nlen) && (j<elen)) {
                    i=0;
                } else if ((i<nlen) && (j==elen)) {
                    j=0;
                }
            }
        }
         // find --------------------------------------
     } else if (name == "beqo") {
        try {
            if (nlen < 100) {
                for (int i = 0; i < elen; ++i) {
                    help = janetlist[i];
                    if(contains(help, jannyrlist)) {
                        p.push_back(intToBes(i+1));
                    }
                }
            } else {
                // if find list is long use more efficient algorithm
               unordered_map<string, int> f;
               f.clear();
               for (int i = 0; i < nlen; ++i) {
                   string key = jannyrlist[i];
                    if (f.find(key) == f.end()) {
                        f.insert(pair<string, int>(key, 0));
                    }
               }
               for (int i = 0; i < elen; ++i) {
                   help = janetlist[i];
                   if (f.find(help) != f.end()) {
                        p.push_back(intToBes(i+1));
                   }
               }
               f.clear();
            }
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "beqo", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "beqo", to_string(stopExecution), "9");};
           stopExecution = 9;
        }
        // freq--------------------------------------
     } else if (name == "faon") {
         p=freq(janetlist, jannyrlist);
    //get--------------------------------------
    } else if (name == "sip") {
        try {
            for (int i = 0; i < nlen; ++i) {
                ihelp = xtoll(jannyrlist[i]) - 1;
                if((ihelp >= 0) && (ihelp < elen)) {
                    p.push_back(janetlist[ihelp]);
                }
            }
        } catch (exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "sip", join(janetlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "sip", to_string(stopExecution), "9");};
           stopExecution = 9;
        }
    // identity--------------------------------------
    } else if (name == "wah") {
        p = janxilist;
    // input --------------------------------------
    } else if (name == "dum") {
       string x = join(janetlist, " ") + "? ";
       cout << x;
       cin >> help;
       p.push_back(help);
    // include--------------------------------------
    } else if (name == "dii") {
        // do nothing, is already handled
    // insert --------------------------------------
    } else if (name == "sil") {
        long long int ix = xtoll(jannyrlist[0]);
        for (int i = 0; i < xlen; ++i) {
            if (ix == i) {
             for (int j = 1; j < nlen; ++j) {
                p.push_back(jannyrlist[j]);
             }
            }
            p.push_back(janxilist[i]);
        }
        if (ix >= xlen) {
             for (int j = 1; j < nlen; ++j) {
                p.push_back(jannyrlist[j]);
             }
        }
    // join --------------------------------------
    } else if (name == "kum") {
        if (elen > 0) {
            if (nlen > 0) {
                help = jannyrlist[0];
				if (help == "#fu") {help = " ";}
				p.push_back(join(janetlist,help));
            } else {
				p.push_back(join(janetlist, ""));
		  	}
        }
    // mask--------------------------------------
    } else if (name == "ren") {
        if ((elen>0)&&(nlen>0)) {
            int ihelp = -1;
            for (int i = 0; i < elen; ++i) {
                ++ihelp;
                if(ihelp >= nlen) {ihelp=0;}
                string shelp = jannyrlist[ihelp];
                long long int jhelp = xtoll(shelp);
 				for (int j=1; j<=jhelp; ++j) {
					p.push_back(janetlist[i]);
                }
            }
        } else if (elen>0) {
            p = janetlist;
        }
    // minus--------------------------------------
    } else if (name == "silav") {
        if ((nlen>0)&&(elen>0)) {
            p = vminus(janetlist,jannyrlist);
        } else if (elen > 0) {
            p = janetlist;
        }
    // output--------------------------------------
     }  else if (name == "haanav") {
        doOut(janxilist, true);
        p = janxilist;
        if (logOutput) {logg.plain(join(janxilist, " "));}
    // pexpr--------------------------------------
     }  else if (name == "khil") {
        if (stype == "thek") {
             p = bulkExecLong(SINGLE, janetlist, vempty, vempty, vempty);
        }else if (stype == "fee") {
             p = bulkExecFloat(SINGLE, janetlist, vempty, vempty, vempty);
        } else {
             p = bulkExecString(SINGLE, janetlist, vempty, vempty, vempty);
        }
    // print --------------------------------------
    } else if (name == "haan") {
        doPrint(janxilist, false);
        p = janxilist;
        if (logOutput) {logg.plain(join(janxilist, " "));}
    // range--------------------------------------
       } else if (name == "vae") {
        try {
          if (xlen >= 2) {
            vector<bool> up = getUp(janxilist);
            vector<string> first = getRange(janxilist[0], janxilist[1], "0", up[0]);
            for (int i=0;i<(int)first.size();i++) {
                if (xlen >= 4) {
                   vector<string> second = getRange(janxilist[2], janxilist[3], first[i], up[1]);
                   for (int j=0;j<(int) second.size();j++) {
                       if (xlen >= 6) {
                            vector<string> third = getRange(janxilist[4], janxilist[5], second[j], up[2]);
                            for (int k=0;k<(int)third.size();k++) {
                                p.push_back(first[i]);
                                p.push_back(second[j]);
                                p.push_back(third[k]);
                            }
                       } else {
                            p.push_back(first[i]);
                            p.push_back(second[j]);
                       }
                   }
                } else {
                   p.push_back(first[i]);
                }
            }
          }
        } catch (exception& e) {
            logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "vae", join(janxilist, " "));
            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "vae", to_string(stopExecution), "9");};
            stopExecution = 9;
        }
    // readLines--------------------------------------
    } else if (name == "mud") {
        bool keepEmpty = false;
        bool keepEol = false;
        bool keepEof = false;
        if (nlen > 0) {
            for (int i=0;i<(int)nlen;i++) {
                if ((jannyrlist[i] == "wayuq") &&(jannyrlist[i+1] == "moqa")) {
                    keepEmpty = true;
                } else if ((jannyrlist[i] == "wak")&&(jannyrlist[i+1] == "moqa")){
                    keepEol = true;
                } else if ((jannyrlist[i] == "sec")&&(jannyrlist[i+1] == "moqa")) {
                    keepEof = true;
                }
            }
        }
        if (xlen == 0) {
            logg.logmsg(FATAL, FILEMISSING, "", "");
            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "mud", to_string(stopExecution), "9");};
            stopExecution = 9;
        } else {
            try {
                string x = janxilist[0];
                if (!isPath(x)) {x=currDir+x;}
                if (debug_x) {logg.logdbg3(READFILEIS, x, "", "");};
                p = readFile(x, keepEmpty, keepEol, keepEof);
                if (debug_x) {logg.logdbg3(RESULTSIZE, to_string(p.size()), "", "");};
           } catch (exception& e) {
                logg.logmsg(FATAL, MUDNOEXEC, cstos(e.what()), janxilist[0]);
                if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "mud", to_string(stopExecution), "9");};
                stopExecution = 9;
            }
        }

    // reverse--------------------------------------
    } else if (name == "roc") {
       for (int i = elen-1; i >= 0; --i) {
            p.push_back(janetlist[i]);
       }
    // returnValue--------------------------------------
    } else if (name == "nna") {
        for (int i = 0; i < xlen; ++i) {
            p.push_back(janxilist[i]);
        }
    // set --------------------------------------
    } else if (name == "cop") {
       try {
           // maybe needs rewrite??
           for (int i = 0; i < nlen; i+=2) {
               long long int ix = xtoll(jannyrlist[i]);
               if ((i+1) < nlen) {
                    help = jannyrlist[i+1];
                    if ((ix>0)&& (ix<=elen)){janetlist[ix - 1] = help;}
               } else {
                    logg.logmsg(WARN, COPMISSINGVAL, "", "");
               }
           }
           addToVector(p, janetlist);
       } catch(exception& e) {
           logg.logmsg3(FATAL, ERRORWHENEXEC, cstos(e.what()), "cop", join(jannyrlist, " "));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "cop", to_string(stopExecution), "9");};
           stopExecution = 9;
       }
    // sort --------------------------------------
    } else if (name == "khi") {
        bool isDesc = false;
        bool hasRows = false;
        int index = 0;
        int rowlen = 1;
        if (nlen > 0) {
           for (int i = 0; i < nlen; ++i) {
                if (jannyrlist[i] == "roc") {
                    isDesc = true;
                } else if (jannyrlist[i]== "tut") {
                    hasRows = true;
                    index = xtoll(jannyrlist[i+1]);
                    rowlen =xtoll(jannyrlist[i+2]);
                }
           }
        }
        if (hasRows) {
            if (stype == "thek") {
                sortRowsLong(index, rowlen, isDesc, janxilist, p);
            } else {
                sortRowsString(index, rowlen, isDesc, janxilist, p);
            }
        } else if (stype == "") {
            try {
                index = 1;
                rowlen = 1;
                sortRowsString(index, rowlen, isDesc, janxilist, p);
            } catch (exception& e) {
                logg.logmsg(FATAL, ERRORWHENEXECPLAIN, cstos(e.what()), "khi");
                if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "khi", to_string(stopExecution), "9");};
               stopExecution = 9;
             }
    // --------------------------------------
        } else if (stype == "thek") {
            try {
                index = 1;
                rowlen = 1;
                sortRowsLong(index, rowlen, isDesc, janxilist, p);
            } catch (exception& e) {
               logg.logmsg(FATAL, ERRORWHENEXECPLAIN, cstos(e.what()), "khithek");
               if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "khithek", to_string(stopExecution), "9");};
               stopExecution = 9;
             }
        }

        if (janxilist.size() != p.size()) {
            logg.logmsg(ERROR, ERROREXECKHI, "", "");
        }
    // split--------------------------------------
    } else if (name == "ria") {
        if(elen>0) {
           if(nlen>0) {
                help = jannyrlist[0];
                if (help == "#fu") {help = " ";}
                char c = help[0];
                for (int i = 0; i < elen; ++i) {
                    vector<string> w = split(janetlist[i], c);
                    addToVector(p, w);
                }
           } else {
                //split every char
                for (int i = 0; i < elen; ++i) {
                    string x = janetlist[i];
                    vector<string> w;
                    for (int j = 0; j < (int) x.size(); ++j) {
                        w.push_back(ctos(x[j]));
                    }
                    addToVector(p, w);
                }
           }
        }
    // time--------------------------------------
     } else if (name == "voos") {
         p = timeverb(janxilist);
    // unique--------------------------------------
    } else if (name == "kumoryy") {
        try {
            progress = "init";
           unordered_map<string, int> f;
           f.clear();
           progress = "set map";
           for (int i = 0; i < elen; ++i) {
               string key = janetlist[i];
                if (f.find(key) == f.end()) {
                    f.insert(pair<string, int>(key, 0));
                }
           }
           progress = "fill p";
           for (auto iter = f.begin(); iter != f.end(); ++iter){
                p.push_back(iter->first);
           }
           progress = "after p";
           f.clear();
        } catch (exception& e) {
           logg.logmsg4(FATAL, ERROREXECJANET, cstos(e.what()),"kumoryy", to_string(elen), progress);
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "kumoryy", to_string(stopExecution), "9");};
           stopExecution = 9;
        }
     // writeLines--------------------------------------
    } else if (name == "joq") {
        try {
            string x = janxilist[0];
            if (!isPath(x)) {x=currDir+x;}
            bool retcode = writeFile(x, janetlist);
            if (retcode) {
                p.push_back("te");
            } else {
                p.push_back("bu");
            }
        } catch (exception& e) {
           logg.logmsg4(FATAL, ERROREXECJANET, cstos(e.what()),"joq", to_string(elen), progress);
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "joq", to_string(stopExecution), "9");};
           stopExecution = 9;
        }
    } else if ((name == "fiiar") && isWeak) {
        try {
            for (int i = 0; i < elen; ++i) {
                help = janetlist[i];
                Numtest nt = testType(help);
                if (nt == JUUINT) {
                    help = to_string(besToInt(help));
                } else if (nt == JUUFLOAT) {
                    help = to_string(besToFloat(help));
                }
                p.push_back(help);
            }
        } catch (exception& e) {
           logg.logmsg4(FATAL, ERROREXECJANET, cstos(e.what()),"fiiar", to_string(elen), progress);
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "fiiar", to_string(stopExecution), "9");};
           stopExecution = 9;
        }
    } else {
        int index = names[name];
        p = execParagraph(index, thislist, janlist, janetlist, janxilist, jannyrlist, stackLevel + 1);
        if (debug_c) {logg.logdbg3(RVIS, join(p, " "), "", "");};
    }


    // warn for std, but not returnValue
   if (isStandardVerb && (p.size() == 0)) {
       // nowarnall for returnValue, include
      if ((name != "nna") && (name != "dii")) {
        logg.logmsg(WARNALL, STANDARDOUTEMPTY, name, "");
        if (contains(name, nets)) {
            logg.logmsg(WARNALL, INPUTWAS, "janetlist", join(janetlist, " "));
        }
        if (contains(name, nxis)) {
            logg.logmsg(WARNALL, INPUTWAS, "janxilist", join(janxilist, " "));
        }
        if (contains(name, nyrs)) {
            logg.logmsg(WARNALL, INPUTWAS,"jannyrlist", join(jannyrlist, " "));
        }
      }
   }

   return make_tuple(offset, p);
}

tuple<int, vector<string>> Interpreter::executePerRowSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                       vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel) {
   		if (debug_t) {logg.debugtime(TIMEINIT, "", "");};

   		if (debug_b) {logg.logdbg3(STARTPERROW, ending, name, "");};
        int offset = 1;
        vector<string> p;
		vector<string> result;
        int jlen = janlist.size();

        Rowdefs rowdefs;
        vector<Rowitem> rowitems;
        bool doIsJanet = (ending == "ge") || (ending == "e");
        if (doIsJanet) {
            rowdefs = getPerRowDefs(janetlist);
            rowitems = getPerRowItems(rowdefs.offset, janetlist);
        } else {
            rowdefs = getPerRowDefs(janxilist);
            rowitems = getPerRowItems(rowdefs.offset, janxilist);
        }
        int off = 0;
        int ix = 0;

        if (rowdefs.inits.size() > 0) {
            addToVector(result, rowdefs.inits);
        }
 		vector<string> bhelp;
		string shelp = "";
		string thelp = "";
		string entry = "";
		bool isValid = true;
		Rowitem ri = Rowitem("");
		for (int i=0; i<jlen; i+=rowdefs.rowlength) {
			isValid = true;
			off = 0;
			bhelp.clear();
			p.clear();
			int emptyEntries = 0;
			entry = "";
            for (int j=0; j<(int)rowitems.size(); ++j) {
                ri = rowitems[j];
                switch (ri.typ) {
                   case 0:
                        entry = ri.plain;
                        break;
                   case 1:
                        if (off>=rowdefs.rowlength) {off=0;}
                        if (i+off<jlen){
                           entry = janlist[i+off];
                           ++off;
                        } else if (rowdefs.hasDefjan) {
                           entry = rowdefs.defjan;
                           ++off;
                        } else {
                           bhelp.clear();
                           entry = "";
                           if (debug_x) {logg.logdbg3(INDEXHI, "janlen", to_string(i), "");};
                           isValid = false;
                        }
                        break;
                   case 2:
                      ix = ri.index-1;
                      if ((i+ix >= 0) && (i + ix <jlen)) {
                         entry = janlist[i+ix];
                      } else if (rowdefs.hasDefjan) {
                         entry = rowdefs.defjan;
                      } else {
                         bhelp.clear();
                         entry = "";
                         if (debug_x) {logg.logdbg3(INDEXHIORLO, "janlen", to_string(i), "");};
                         isValid = false;
                      }
                      break;
                   case 3:
                        ix = ri.index-1;
                      if ((i+ix >= 0) && (i + ix <(int) janetlist.size())) {
                         entry = janetlist[i+ix];
                      } else if (rowdefs.hasDefet) {
                         entry = rowdefs.defet;
                      } else {
                         entry = "";
                         bhelp.clear();
                         if (debug_x) {logg.logdbg3(INDEXHIORLO, "janetlen", to_string(i), "");};
                         isValid = false;
                      }
                      break;
                   case 4:
                        ix = ri.index-1;
                      if ((i+ix >= 0) && (i + ix <(int) janxilist.size())) {
                         entry = janxilist[i+ix];
                      } else if (rowdefs.hasDefxi) {
                         entry = rowdefs.defxi;
                      } else {
                         entry = "";
                         bhelp.clear();
                         if (debug_x) {logg.logdbg3(INDEXHIORLO, "janxilen", to_string(i), "");};
                         isValid = false;
                      }
                      break;
                   case 5:
                        ix = ri.index-1;
                        if ((i+ix >= 0) && (i + ix <(int)jannyrlist.size())) {
                           entry = p[i+ix];
                        } else if (rowdefs.hasDefnyr) {
                            entry = rowdefs.defnyr;
                        } else {
                            bhelp.clear();
                            entry = "";
                            if (debug_x) {logg.logdbg3(INDEXHIORLO, "jannyrlen", to_string(i), "");};
                            isValid = false;
                        }
                        break;
                   case 6:
                        ix = ri.index-1;
                        if ((i+ix >= 0) && (i + ix <(int)result.size())) {
                           entry = result[i+ix];
                        } else if (rowdefs.hasDefres) {
                            entry = rowdefs.defres;
                        } else {
                            bhelp.clear();
                            entry = "";
                            if (debug_x) {logg.logdbg3(INDEXHIORLO, "<res>len", to_string(i), "");};
                            isValid = false;
                        }
                        break;
                   case 7:
                        entry = intToBes(i+1);
                        break;
                   case 8:
                        entry = "#chan";
                        break;
                   case 9:
                       if (result.size() > 0) {
                            entry = result.back();
                        } else if (rowdefs.hasDefres) {
                            entry = rowdefs.defres;
                        } else {
                            bhelp.clear();
                            entry = "";
                            if (debug_y) {logg.logdbg3(CANTUSEMESEC, "", "", "");};
                            isValid = false;
                        }
                        break;
                }
                if (!isValid) {
                    logg.logmsg(WARNALL, RESULTINVALID, name, "");
                    break;
                }

                if (entry == "#chan") {
                   for (int j=i; j<i+rowdefs.rowlength; j++) {
                        bhelp.push_back(janlist[j]);
                   }
                } else if (entry.size() > 0) {
                    bhelp.push_back(entry);
                } else {
                    ++emptyEntries;
                }
             }

            if (emptyEntries > 0) {
                logg.logmsg3(WARNALL, EMPTYENTRIES, to_string(emptyEntries), name, join(bhelp, ","));
                isValid = false;
            }
			if (isValid) {
                if (isStandardVerb) {
                    //tie(offset, p) = executeStandardVerb(name, stype, bhelp, forlist, offset);
                    // different calls needed
                    if (doIsJanet) {
                        tie(offset, p) =executePlainVerb(name, isStandardVerb, jump, stype, thislist, janlist, bhelp, janxilist, jannyrlist, stackLevel);
                    } else {
                        tie(offset, p) =executePlainVerb(name, isStandardVerb, jump, stype, thislist, janlist, janetlist, bhelp, jannyrlist, stackLevel);
                    }
                } else if (names.find(name) != names.end()) {
                    int index = names[name];
                    if (doIsJanet) {
                        p = execParagraph(index, thislist, janlist, bhelp, janxilist, jannyrlist, 0);
                    } else {
                        p = execParagraph(index, thislist, janlist, janetlist, bhelp, jannyrlist, 0);
                    }
                    if (debug_c) {logg.logdbg3(RVIS, join(p, " "), "", "");};
                } else {
                    logg.logmsg(FATAL, VERBNOTALLOWED, ending, name);
                    if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, ending, to_string(stopExecution), "9");};
                    stopExecution = 9;
                }
                if (p.size() > 0) {
                    addToVector(result, p);
                } else {
                    logg.logmsg(WARNALL, EMPTYRESULTIGNORED, name , ending);
                }

                if (stopExecution>0) {break;}
			}
		}

    if (debug_b) {logg.logdbg3(FINISHEDPERROW, name, "", "");};
    stopExecution =newStopExecution(stopExecution, LOOP);
    if (debug_t) {logg.debugtime(TIMEFOR, name, "(g)e");};
    return make_tuple(offset, result);
  }

vector<string> Interpreter::getRange(string sfrom, string sto, string sprev, bool  up) {
     vector<string> result;

     int x = getRangeIndex(sfrom, sprev);
     int y = getRangeIndex(sto, sprev);

     if ((x<=y) && up){
        for (int j = x; j <= y; ++j) {
            result.push_back(intToBes(j));
        }
     } else if ((x>y) && (!up)){
        for (int j = x; j >= y; --j) {
             result.push_back(intToBes(j));
        }
    }
    return result;
}


long long int Interpreter::getRangeIndex(string r, string prev) {
     long long int result = 0;
     if (r == "sotai") {
        result = xtoll(prev);
     } else if (r == "parai") {
        result= xtoll(prev) + 1;
     } else if (r == "shax") {
        result= xtoll(prev) - 1;
     } else  {
        result= xtoll(r);
     }
     return result;
}

vector<bool> Interpreter::getUp(vector<string> therange) {
    vector<bool> up;
     long long int x = 0;
     long long int y = 0;
     string s = "";
     if (therange.size()>=2) {
        x = xtoll(therange[0]);
        y = xtoll(therange[1]);
        up.push_back(x<=y);
     }

     if (therange.size()>=4) {
        x = getRangeIndex(therange[2], therange[0]);
        y = getRangeIndex(therange[3], therange[0]);
        s = to_string(x);
        up.push_back(x<=y);
     }

     if (therange.size()>=6) {
        x = getRangeIndex(therange[4], s);
        y = getRangeIndex(therange[5], s);
        up.push_back(x<=y);
     }
    return up;
}

int Interpreter::getDefLength(string text) {
    int result = 1;
    if (text.substr(0, 3) == "tut") {
        string s = text.substr(3);
        Numtest nt = testType(s);
        if ((nt != NUMINT) && (nt != JUUINT)) {
            logg.logmsg(ERROR, TUTLENGTHNOTINT, text, "");
        } else {
            result = xtoll(s);
        }
    }
    return result;
  }

Rowdefs Interpreter::getPerRowDefs(vector<string>& v) {
      Rowdefs rowdefs;
      rowdefs.offset = 0;
      rowdefs.rowlength = 1;
      rowdefs.hasDefjan = false;
      rowdefs.hasDefnyr = false;
      rowdefs.hasDefxi = false;
      rowdefs.hasDefet = false;
      rowdefs.hasDefres = false;
      string text;
      string help;
      bool foundBar = false;
      int len = v.size();
      int i = 0;
      while (i<len) {
        text = v[i];
        if (text.substr(0, 3) == "tut") {
            help = text.substr(3);
            //if (!isXlong(help)) {
            Numtest nt = testType(help);
            if ((nt != NUMINT) && (nt != JUUINT)) {
                logg.logmsg(ERROR, TUTLENGTHNOTINT, text, "");
            } else {
                rowdefs.rowlength = xtoll(help);
            }
        } else if (text.substr(0, 6) == "jantyy") {
            rowdefs.defjan = text.substr(6);
            rowdefs.hasDefjan = true;
        } else if (text.substr(0, 5) == "nytyy") {
            rowdefs.defnyr = text.substr(5);
            rowdefs.hasDefnyr = true;
        } else if (text.substr(0, 5) == "xityy") {
            rowdefs.defxi = text.substr(5);
            rowdefs.hasDefxi = true;
        } else if (text.substr(0, 4) == "etyy") {
            rowdefs.defet = text.substr(4);
            rowdefs.hasDefet = true;
        } else if (text.substr(0, 5) == "metyy") {
            rowdefs.defres = text.substr(5);
            rowdefs.hasDefres = true;
        }  else if (text.substr(0, 3) == "tas") {
            rowdefs.inits.push_back(text.substr(3));
        }  else if (text == "|") {
            rowdefs.offset = i + 1;
            foundBar = true;
            break;
        }
        ++i;
    }

    if (!foundBar) {
        logg.logmsg(ERROR, NOBARSEP, "", "");
         if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "(g)e/(g)ete", to_string(stopExecution), "9");};
        stopExecution = 9;
    }
    return rowdefs;
  }

vector<Rowitem> Interpreter::getPerRowItems(int start, vector<string>& v) {
    vector<Rowitem> result;
    for (int i=start; i<(int) v.size(); ++i) {
        Rowitem rowitem = Rowitem(v[i]);
        result.push_back(rowitem);
    }
    return result;
  }

tuple<int, vector<string>> Interpreter::executeGainSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                      vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel) {
   		if (debug_t) {logg.debugtime(TIMEINIT, "", "");};
        int offset = 1;
        vector<string> p;
		vector<string> result;
        int janlen = janlist.size();
        int etlen = janetlist.size();
        int xilen = janxilist.size();
        int nyrlen = janxilist.size();
        int joff = 0;
        int xoff = 0;
        int noff = 0;
		vector<string> bhelp;
		string shelp = "";
		string thelp = "";
		bool hasJan = false;
		bool hasXi = false;
		bool hasNyr = false;
		bool goon = true;
		while (goon) {
			bhelp.clear();
		  	for (int j=0; j<etlen; ++j) {
		  		thelp = janetlist[j];
				if (thelp[0] == '#') {
					thelp = thelp.substr(1);
					if (thelp == "j") {
						if (joff <janlen) {
							shelp = janlist[joff];
							bhelp.push_back(shelp);
							++joff;
							hasJan = true;
						} else {
						    goon = false;
						    break;
 						}
					} else if (thelp == "x") {
						if (xoff <xilen) {
							shelp = janxilist[xoff];
							bhelp.push_back(shelp);
							++xoff;
							hasXi = true;
						} else {
   						    goon = false;
						    break;
 						}
					} else if (thelp == "n") {
						if (noff <nyrlen) {
							shelp = jannyrlist[noff];
							bhelp.push_back(shelp);
							++noff;
							hasNyr = true;
						} else {
   						    goon = false;
						    break;
 						}
					} else {
                        // do nothing, # is ignored
					}
				} else {
					bhelp.push_back(thelp);
				}
		  	}

		  	if (goon) {
                vector<string> jl;
                vector<string> xl;
                vector<string> nl;
                if (!hasNyr) {nl=jannyrlist;}
                if (!hasXi) {xl=janxilist;}
                if (!hasJan) {jl=janlist;}

                tie(offset, p) = executePlainVerb(name, isStandardVerb, jump, stype, thislist, jl, bhelp, xl, nl, stackLevel);
                addToVector(result, p);

                if (stopExecution>0) {break;}
		  	}
		}

    stopExecution = newStopExecution(stopExecution, LOOP);
    if (debug_t) {logg.debugtime(TIMEFOR, name, ending );};
    return make_tuple(offset, result);
  }

tuple<int, vector<string>> Interpreter::executeGainabSentence(string name, bool isStandardVerb, int jump, string stype, string ending,
                      vector<string> &thislist, vector<string> &janlist, vector<string> &janetlist, vector<string> &janxilist, vector<string> &jannyrlist, int stackLevel) {
   		if (debug_t) {logg.debugtime(TIMEINIT, "", "");};
        int offset = 1;
        vector<string> p;
		vector<string> result;
        int janlen = janlist.size();
        int etlen = janetlist.size();
        int xilen = janxilist.size();
        int nyrlen = jannyrlist.size();
        int eoff = 0;
        int joff = 0;
        int noff = 0;
		vector<string> bhelp;
		string shelp = "";
		string thelp = "";
		bool hasEt = false;
		bool hasJan = false;
		bool hasNyr = false;
		bool goon = true;
		while (goon) {
			bhelp.clear();
		  	for (int j=0; j<xilen; ++j) {
		  		thelp = janxilist[j];
				if (thelp[0] == '#') {
					thelp = thelp.substr(1);
					if (thelp == "e") {
						if (eoff <etlen) {
							shelp = janetlist[eoff];
							bhelp.push_back(shelp);
							++eoff;
							hasEt = true;
						} else {
						    goon = false;
						    break;
 						}
					} else if (thelp == "j") {
						if (joff <janlen) {
							shelp = janlist[joff];
							bhelp.push_back(shelp);
							++joff;
							hasJan = true;
						} else {
   						    goon = false;
						    break;
 						}
					} else if (thelp == "n") {
						if (noff <nyrlen) {
							shelp = jannyrlist[noff];
							bhelp.push_back(shelp);
							++noff;
							hasNyr = true;
						} else {
   						    goon = false;
						    break;
 						}
					} else {
                        // do nothing, # is ignored
					}
				} else {
					bhelp.push_back(thelp);
				}
		  	}

		  	if (goon) {
                vector<string> el;
                vector<string> jl;
                vector<string> nl;
                if (!hasNyr) {nl=jannyrlist;}
                if (!hasEt) {el = janetlist;}
                if (!hasJan) {jl = janlist;}

                tie(offset, p) = executePlainVerb(name, isStandardVerb, jump, stype, thislist, jl, el, bhelp, nl, stackLevel);
                addToVector(result, p);

                if (stopExecution>0) {break;}
		  	}
		}

    stopExecution = newStopExecution(stopExecution, LOOP);
     if (debug_t) {logg.debugtime(TIMEFOR, name, ending);};
    return make_tuple(offset, result);
  }


vector<string> Interpreter::vminus(vector<string> a, vector<string> b) {
    vector<string> c;
    vector<string> d;

    try {
        d = b;

        for (int i=0;i< (int) a.size(); ++i) {
            string o = a[i];
            vector<string>::iterator it = find(d.begin(), d.end(), o);
            if (it != d.end()) {
                d.erase(it);
            } else {
                c.push_back(o);
            }
        }
    } catch (exception& e) {
        logg.logmsg(FATAL, ERRORWHENEXECPLAIN, cstos(e.what()), "silav");
         if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "silav", to_string(stopExecution), "9");};
        stopExecution = 9;
    }
    return c;
  }

void Interpreter::doPrint(vector<string> v, bool withCrlf) {
    int len = v.size();
    bool suppress = false;
    for (int i = 0; i < len; ++i) {
        string s = format(v[i]);
        if ((i>0) && !suppress) {printf(" ");}
        printf("%s", s.data());
        suppress = false;
        if (last(s) == '\n') {suppress = true;}
    }
    if (withCrlf) {printf("\n");}
  }

void Interpreter::doOut(vector<string> v, bool withCrlf) {
    //setlocale(LC_ALL, "de_DE.UTF-8");
    //cout.imbue(mylocale);
    int len = v.size();
    for (int i = 0; i < len; ++i) {
        if(i>0) {cout << " ";}
        cout << (v[i]);
    }
    if (withCrlf)  {cout << endl;}
    //setlocale(LC_ALL, "C");
}

vector<string> Interpreter::fileop(string op, string filename) {
    vector<string> result;
    string x = filename;
    bool bhelp;
    try {
        if (!isPath(x)) {x=currDir+x;}
        // delete file
        if (op == "phe") {
            if (!filesystem::exists(x)) {
               result.push_back("te");
            } else {
                int res = remove(x.c_str()); // 0=ok
                if (res == 0) {
                    result.push_back("te");
                } else {
                    logg.logmsg(ERROR, FILENOTDEL, x, "");
                    result.push_back("bu");
                }
            }
        // file exists
        } else if (op == "wah") {
            if (filesystem::exists(x)) {
                result.push_back("te");
            } else {
                result.push_back("bu");
            }
        // file create
        } else if (op == "fawaan") {
            ofstream o(x);
            o.close();
            result = fileop("wah", x);
        // delete dir
        } else if (op == "rimoo phe") {
            if (!filesystem::exists(x)) {
                result.push_back("te");
            } else if (!filesystem::is_directory(x)) {
                result.push_back("te");
                logg.logmsg(ERROR, NOTADIR, x, "");
            } else {
                bhelp = filesystem::remove(x);
                if (bhelp) {
                    result.push_back("te");
                } else {
                    result.push_back("bu");
                    logg.logmsg(ERROR, DIRNOTDEL, x, "");
                }
            }
        // dir exists
        } else if (op == "rimoo wah") {
            bhelp = filesystem::is_directory(x);
            if (bhelp) {
                result.push_back("te");
            } else {
                result.push_back("bu");
            }
        // create dir
        } else if (op == "rimoo fawaan") {
            bhelp = filesystem::create_directory(x);
            if (filesystem::is_directory(x)) {
                result.push_back("te");
            } else {
                logg.logmsg(ERROR, DIRNOTCREATE, x, "");
                result.push_back("bu");
            }
        // list dir
        } else if (op == "rimoo ria") {
            if ((filename == "/") || (filename == "./")) {x = currDir;}
            for (const auto & file : filesystem::directory_iterator(x)) {
                string path = file.path().string();
                // result is always absolute path, reduce to relative path
                if (path.substr(0,currDir.size()) == currDir) {
                    path = path.substr(currDir.size()-1);
                    if ((path[0]== '/') || (path[0] == '\\')) {
                        path=path.substr(1);
                    }
                }
                result.push_back(path);
            }
        }
    } catch (exception& e) {
        logg.logmsg3(FATAL, NOEXECONFILE, cstos(e.what()), op, x);
         if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, op, to_string(stopExecution), "9");};
        stopExecution = 9;
    }
    return result;
}

vector<string> Interpreter::bulkExecLong(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist) {
    if (debug_t) {logg.debugtime(TIMEINIT, "", "");};
    vector<Opcode> codes = toOpcode(ops);
    vector<string> result;
    int len = (mode == MULTI) ? liste.size() : 1;
    int i = 0;
    vector<long long int> ihelp;
    long long int il;
    long long int ir;
    long long int i3;
    int incr = 1;
    bool hasdefault = false;
    long long int def = 0;
    int maxvar = (mode == MULTI) ? getRowLength(ops) : 1;
    Opcode code = codes[0];

    while ((code.op == ROW) || (code.op == DEFAULT)){
        if (mode == SINGLE) {
            logg.logmsg(ERROR, OPCODENOTALLOWED, "tut/tyy", "khil");
            return vempty;
        } else if (code.op == ROW) {
            incr = code.left;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETROWINCR, to_string(incr), "", "");};
        } else if (code.op == DEFAULT) {
            hasdefault = true;
            def = code.left;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETDEFAULT, to_string(def), "", "");};
            maxvar = 1;
        }
        code = codes[0];
    }

    try {
        while(i<=len-maxvar) {
            ihelp.clear();
            int j = 0;
            while (j<(int)codes.size()) {

                code = codes[j];
                if (ihelp.size() < code.operands) {
                    if ((ihelp.size() == 1) && ((code.op == AND) || (code.op == OR) || (code.op == PLUS) || (code.op == TIMES)
                        || (code.op == MIN)  || (code.op == MAX))) {
                            // do nothing, just skip operator
                    } else {
                        string opcseq = "";
                        for (int i=0; i<(int) codes.size(); ++i) {opcseq += " " + to_string((codes[i]).op);}
                        logg.logmsg(ERROR, NOTENOUGHOPSINSEQ, to_string(code.op), opcseq);
                        if (debug_y) {logg.logdbg3(CHANGEDSTOPEXECMO, to_string(stopExecution), "", "");};
                        stopExecution = 9;
                        break;
                    }
                } else if (code.op == AND) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(((ir>0) && (il>0)) ? 1 : 0);
                } else if (code.op == OR) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(((ir>0) || (il>0)) ? 1 : 0);
                } else if (code.op == NOT) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((ir == 0) ? 1 : 0);
                } else if (code.op == ABS) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((ir >= 0) ? ir : -ir);
                }else if (code.op == PLUS) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(il + ir);
                } else if (code.op == MINUS) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(il - ir);
                } else if (code.op == TIMES) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(il * ir);
                } else if (code.op == DIVIDE) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(il / ir);
                } else if (code.op == MOD) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(il % ir);
                } else if (code.op == MAX) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   if (il > ir) {
                    ihelp.push_back(il);
                   } else {
                    ihelp.push_back(ir);
                   }
                } else if (code.op == MIN) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   if (il < ir) {
                    ihelp.push_back(il);
                   } else {
                    ihelp.push_back(ir);
                   }
                } else if (code.op == DIV) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   i3 = il % ir;
                   il = (il - i3) / ir;
                   ihelp.push_back(il);
                   ihelp.push_back(i3);
                } else if (code.op == POWER) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(lpower(il, ir));
                }else if (code.op == EQ) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il == ir) ? 1 : 0);
                } else if (code.op == NEQ) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il != ir) ? 1 : 0);
                } else if (code.op == GE) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il >= ir) ? 1 : 0);
                } else if (code.op == LE) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il <= ir) ? 1 : 0);
                } else if (code.op == GT) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il > ir) ? 1 : 0);
                } else if (code.op == LT) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back((il < ir) ? 1 : 0);
                } else if (code.op == ELVIS) {
                   i3 =  ihelp.back();
                   ihelp.pop_back();
                   ir = ihelp.back();
                   ihelp.pop_back();
                   il =  ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(((il>0)) ? ir : i3);
                } else if (code.op == VAL) {
                    ihelp.push_back(xtoll(code.sleft));
                 } else if (code.op == VAR) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "suo", "khil");
                        return vempty;
                    }

                    il = i + code.left - 1;

                    if ((il < 0) || (il >= len)) {
                      if (hasdefault) {
                        ihelp.push_back(def);
                      } else {
                        ihelp.push_back(0);
                        logg.logmsg(ERROR, INDEXNOTEXISTS, to_string(il), "");
                      }
                    } else {
                        ihelp.push_back(xtoll(liste[il]));
                    }
                } else if (code.op == IDX) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "sip", "khil");
                        return vempty;
                   }
                   ihelp.push_back(i+1);
                } else if (code.op == XISIP) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "xisip", "khil");
                        return vempty;
                   }
                   ir = ihelp.back();
                   ihelp.pop_back();
                   if (debug_b) {logg.logdbg3(JANXILENGETINDEX, to_string(janxilist.size()), to_string(ir-1), "");};
                   ihelp.push_back(xtoll(janxilist[ir-1]));
                } else if (code.op == ANSIP) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "ansip", "khil");
                        return vempty;
                   }
                   ir = ihelp.back();
                   ihelp.pop_back();
                   ihelp.push_back(xtoll(janlist[ir-1]));
                } else if (code.op == OUT) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "pri", "khil");
                        return vempty;
                   }
                   ir = ihelp.back();
                   ihelp.pop_back();
                   result.push_back(intToBes(ir));
                } else if (code.op == DROP) {
                   ihelp.pop_back();
                } else if (code.op == END) {
                   break;
                } else if (code.op == SKIPZ) {
                   ir = ihelp.back();
                   ihelp.pop_back();
                   if (ir == 0) {j+= code.left;}
                }
                ++j;
            }
            i+= incr;
        }

    } catch (exception& e) {
          logg.logmsg4(FATAL, ERRORBAIKHIL, cstos(e.what()), "~thek", join(ops, " "), to_string(ihelp.size()));
          if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "bai/khil~thek", to_string(stopExecution), "9");};
          stopExecution = 9;
    }

    if (ihelp.size() > 0) {
        if (mode == SINGLE) {
            for (int i=0; i<(int) ihelp.size(); ++i) {
               result.push_back(intToBes(ihelp[i]));
            }
        } else {
           logg.logmsg(WARNALL, NOTALLRESULTSUSED, "bai", to_string(ihelp.size()));
        }
    }

    if (mode == MULTI) {
         if (debug_t) {logg.debugtime(TIMEBL, "", "");};
    } else {
         if (debug_t) {logg.debugtime(TIMEEL, "", "");};
    }

    return result;
}

vector<string> Interpreter::bulkExecFloat(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist) {
    if (debug_t) {logg.debugtime(TIMEINIT, "", "");};
    vector<Opcode> codes = toOpcode(ops);
    vector<string> result;
    int len = (mode == MULTI) ? liste.size() : 1;
    int i = 0;
    vector<long double> dhelp;
    long double dl;
    long double dr;
    long double d3;
    int incr = 1;
    int ihelp = 0;
    bool hasdefault = false;
    long double def = 0.0;
    int maxvar = (mode == MULTI) ? getRowLength(ops) : 1;
    Opcode code = codes[0];

    while ((code.op == ROW) || (code.op == DEFAULT)){
        if (mode == SINGLE) {
            logg.logmsg(ERROR, OPCODENOTALLOWED, "tut/tyy", "khil");
            return vempty;
        } else if (code.op == ROW) {
            incr = code.left;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETROWINCR, to_string(incr), "", "");};
        } else if (code.op == DEFAULT) {
            hasdefault = true;
            def = code.fleft;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETDEFAULT, to_string(def), "", "");};
            maxvar = 1;
        }
        code = codes[0];
    }
     if (debug_b) {logg.logdbg3(BAIKHILMAX, "fee", to_string(len), to_string(maxvar));};

    try {
        while(i<=len-maxvar) {
            dhelp.clear();
            int j = 0;
            while (j<(int)codes.size()) {
                code = codes[j];
                if (dhelp.size() < code.operands) {
                    if ((dhelp.size() == 1) && ((code.op == AND) || (code.op == OR) || (code.op == PLUS) || (code.op == TIMES)
                        || (code.op == MIN)  || (code.op == MAX))) {
                            // do nothing, just skip operator
                        } else {
                            logg.logmsg(ERROR, NOTENOUGHOPS, to_string(code.op), "");
                            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXECMO, to_string(stopExecution), "", "");};
                            stopExecution = 9;
                            break;
                        }
                } else if (code.op == AND) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(((dr>0) && (dl>0)) ? 1 : 0);
                } else if (code.op == OR) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(((dr>0) || (dl>0)) ? 1 : 0);
                } else if (code.op == NOT) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dr == 0) ? 1 : 0);
                } else if (code.op == ABS) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dr >= 0) ? dr : -dr);
                } else if (code.op == CEIL) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(ceil(dr));
                }  else if (code.op == FLOOR) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(floor(dr));
                } else if (code.op == ROUND) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(round(dr));
                } else if (code.op == SQRT) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(sqrt(dr));
                } else if (code.op == SIN) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(sin(dr));
                } else if (code.op == COS) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(cos(dr));
                } else if (code.op == TAN) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(tan(dr));
                } else if (code.op == ARCSIN) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(asin(dr));
                } else if (code.op == ARCCOS) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(acos(dr));
                } else if (code.op == ARCTAN) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(atan(dr));
                } else if (code.op == EXP) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(exp(dr));
                } else if (code.op == LOG) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(log(dr));
                } else if (code.op == LOG10) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(log10(dr));
                } else if (code.op == SINH) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(sinh(dr));
                } else if (code.op == COSH) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(cosh(dr));
                } else if (code.op == TANH) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(tanh(dr));
                } else if (code.op == PLUS) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(dl + dr);
                } else if (code.op == MINUS) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(dl - dr);
                } else if (code.op == TIMES) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(dl * dr);
                } else if (code.op == DIVIDE) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(dl / dr);
                } else if (code.op == MOD) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   d3 = (int) round(dl) % (int) round(dr);
                   dhelp.push_back(d3);
                } else if (code.op == MAX) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   if (dl > dr) {
                    dhelp.push_back(dl);
                   } else {
                    dhelp.push_back(dr);
                   }
                } else if (code.op == MIN) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   if (dl < dr) {
                    dhelp.push_back(dl);
                   } else {
                    dhelp.push_back(dr);
                   }
                } else if (code.op == DIV) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   d3 = (int) round(dl) % (int) round(dr);
                   dl = (dl - d3) / dr;
                   dhelp.push_back(dl);
                   dhelp.push_back(d3);
                } else if (code.op == POWER) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(pow(dl, dr));
                }else if (code.op == EQ) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl == dr) ? 1 : 0);
                } else if (code.op == NEQ) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl != dr) ? 1 : 0);
                } else if (code.op == GE) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl >= dr) ? 1 : 0);
                } else if (code.op == LE) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl <= dr) ? 1 : 0);
                } else if (code.op == GT) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl > dr) ? 1 : 0);
                } else if (code.op == LT) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back((dl < dr) ? 1 : 0);
                } else if (code.op == ELVIS) {
                   d3 =  dhelp.back();
                   dhelp.pop_back();
                   dr = dhelp.back();
                   dhelp.pop_back();
                   dl =  dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(((dl>0)) ? dr : d3);
                } else if (code.op == VAL) {
                   dhelp.push_back(xtold(code.sleft));
                } else if (code.op == VAR) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "suo", "khil");
                        return vempty;
                    }

                    ihelp = i + code.left - 1;

                    if ((ihelp < 0) || (ihelp >= len)) {
                        if (hasdefault) {
                            dhelp.push_back(def);
                        } else {
                            dhelp.push_back(0.0);
                            logg.logmsg(ERROR, INDEXNOTEXISTS, to_string(ihelp), "");
                        }
                    } else {
                        dhelp.push_back(xtold(liste[ihelp]));
                    }
                } else if (code.op == IDX) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "sip", "khil");
                        return vempty;
                   }
                   dhelp.push_back(i+1);
                } else if (code.op == XISIP) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "xisip", "khil");
                        return vempty;
                   }
                   ihelp = (int) dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(xtold(janxilist[ihelp - 1]));
                } else if (code.op == ANSIP) {
                   if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "ansip", "khil");
                        return vempty;
                   }
                   ihelp = (int) dhelp.back();
                   dhelp.pop_back();
                   dhelp.push_back(xtold(janlist[ihelp-1]));
                } else if (code.op == OUT) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "pri", "khil");
                        return vempty;
                   }
                   dr = dhelp.back();
                   dhelp.pop_back();
                   result.push_back(floatToBes30(dr));
                } else if (code.op == DROP) {
                   dhelp.pop_back();
                } else if (code.op == END) {
                   break;
                } else if (code.op == SKIPZ) {
                   dr = dhelp.back();
                   dhelp.pop_back();
                   if (dr == 0) {j+= code.left;}
                }
                ++j;
            }
            i+= incr;
        }

    } catch (exception& e) {
          logg.logmsg4(FATAL, ERRORBAIKHIL, cstos(e.what()), "~fee", join(ops, " "), to_string(dhelp.size()));
           if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "bai/khil~fee", to_string(stopExecution), "9");};
          stopExecution = 9;
    }

    if (dhelp.size() > 0) {
        if (mode == SINGLE) {
            for (int i=0; i<(int) dhelp.size(); ++i) {
                result.push_back(floatToBes30(dhelp[i]));
            }
        } else {
           logg.logmsg(WARNALL, NOTALLRESULTSUSED, "bai", to_string(dhelp.size()));
        }
    }

    if (mode == MULTI) {
         if (debug_t) {logg.debugtime(TIMEBF, "", "");};
    } else {
         if (debug_t) {logg.debugtime(TIMEEF, "", "");};
    }

    return result;
}

vector<string> Interpreter::bulkExecString(Modes mode, vector<string> &ops, vector<string> &liste, vector<string> &janxilist, vector<string> &janlist) {
     if (debug_t) {logg.debugtime(TIMEINIT, "", "");};
    vector<Opcode> codes = toOpcode(ops);
    vector<string> result;
    int len = (mode == MULTI) ? liste.size() : 1;
    int i = 0;
    long long int il = 0;
    vector<string> shelp;
    string sl;
    string sr;
    string s3;
    int incr = 1;
    int maxvar = (mode == MULTI) ? getRowLength(ops) : 1;
    Opcode code = codes[0];
    bool hasdefault = false;
    string def = "";

    while ((code.op == ROW) || (code.op == DEFAULT)){
        if (mode == SINGLE) {
            logg.logmsg(ERROR, OPCODENOTALLOWED, "tut/tyy", "khil");
            return vempty;
        } else if (code.op == ROW) {
            incr = code.left;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETROWINCR, to_string(incr), "", "");};
        } else if (code.op == DEFAULT) {
            hasdefault = true;
            def = code.sleft;
            codes.erase(codes.begin());
             if (debug_b) {logg.logdbg3(SETDEFAULT, def, "", "");};
            maxvar = 1;
        }
        code = codes[0];
    }
     if (debug_b) {logg.logdbg3(BAIKHILMAX, "<string>", to_string(len), to_string(maxvar));};

    try {
         while(i<=len-maxvar) {
            shelp.clear();
            int j = 0;
            while (j<(int)codes.size()) {
                code = codes[j];
                if (shelp.size() < code.operands) {
                    if ((shelp.size() == 1) && ((code.op == AND) || (code.op == OR) || (code.op == PLUS) || (code.op == TIMES)
                        || (code.op == MIN)  || (code.op == MAX))) {
                            // do nothing, just skip operator
                        } else {
                            logg.logmsg(ERROR, NOTENOUGHOPS, to_string(code.op), "");
                            if (debug_y) {logg.logdbg3(CHANGEDSTOPEXECMO, to_string(stopExecution), "", "");};
                            stopExecution = 9;
                            break;
                        }
                } else if (code.op == AND) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(((sr!="bu") && (sl!="bu")) ? "te" : "bu");
                } else if (code.op == OR) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(((sr!="bu") || (sl!="bu")) ? "te" : "bu");
                } else if (code.op == NOT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   shelp.push_back((sr == "bu") ? "te" : "bu");
                } else if (code.op == ISINT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   Numtest nt = testType(sr);
                   if ((nt == NUMINT) || (nt == JUUINT)) {
                        shelp.push_back("te");
                   } else {
                        shelp.push_back("bu");
                   }
                } else if (code.op == ISNUM) {
                   sr = shelp.back();
                   shelp.pop_back();
                   Numtest nt = testType(sr);
                   if (nt == STR) {
                        shelp.push_back("bu");
                   } else {
                        shelp.push_back("te");
                   }
                } else if (code.op == LEN) {
                   sr = shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(sr.size()));
                }else if (code.op == PLUS) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(xtoll(sl) + xtoll(sr)));
                } else if (code.op == CONCAT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(sl + sr);
                } else if (code.op == MINUS) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(xtoll(sl) - xtoll(sr)));
                } else if (code.op == DIVIDE) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(xtoll(sl) / xtoll(sr)));
                } else if (code.op == MOD) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(xtoll(sl) % xtoll(sr)));
                } else if (code.op == POWER) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(lpower(xtoll(sl), xtoll(sr))));
                } else if (code.op == TIMES) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(intToBes(xtoll(sl) * xtoll(sr)));
                } else if (code.op == MAX) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (sl > sr) {
                    shelp.push_back(sl);
                   } else {
                    shelp.push_back(sr);
                   }
                } else if (code.op == MIN) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (sl < sr) {
                    shelp.push_back(sl);
                   } else {
                    shelp.push_back(sr);
                   }
                } else if (code.op == EQ) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back((sl == sr) ? "te" : "bu");
                 } else if (code.op == NEQ) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back((sl != sr) ? "te" : "bu");
                 } else if (code.op == GE) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (isLong(sr) && isLong(sl)) {
                        shelp.push_back((xtoll(sl) >= xtoll(sr) ? "te" : "bu"));
                   } else {
                        shelp.push_back(sl >= sr ? "te" : "bu");
                   }
                 } else if (code.op == LE) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (isLong(sr) && isLong(sl)) {
                        shelp.push_back((xtoll(sl) <= xtoll(sr) ? "te" : "bu"));
                   } else {
                        shelp.push_back(sl <= sr ? "te" : "bu");
                   }
                 } else if (code.op == GT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (isLong(sr) && isLong(sl)) {
                        shelp.push_back((xtoll(sl) > xtoll(sr) ? "te" : "bu"));
                   } else {
                        shelp.push_back(sl > sr ? "te" : "bu");
                   }

                 } else if (code.op == LT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   if (isLong(sr) && isLong(sl)) {
                        shelp.push_back((xtoll(sl) < xtoll(sr) ? "te" : "bu"));
                   } else {
                        shelp.push_back(sl < sr ? "te" : "bu");
                   }
                } else if (code.op == CONCAT) {
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(sl + sr);
                } else if (code.op == ELVIS) {
                   s3 =  shelp.back();
                   shelp.pop_back();
                   sr = shelp.back();
                   shelp.pop_back();
                   sl =  shelp.back();
                   shelp.pop_back();
                   shelp.push_back(((sl!="bu")) ? sr : s3);
                } else if (code.op == VAL) {
                   shelp.push_back(code.sleft);
                } else if (code.op == IDX) {
                     if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "sip", "khil");
                        return vempty;
                    }
                    shelp.push_back(to_string(i+1));
                } else if (code.op == VAR) {
                     if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "suo", "khil");
                        return vempty;
                    }
                    il = i + code.left - 1;

                    if ((il < 0) || (il >= len)) {
                      if (hasdefault) {
                        shelp.push_back(def);
                      } else {
                        shelp.push_back("");
                        logg.logmsg(ERROR, INDEXNOTEXISTS, to_string(il), "");
                      }
                    } else {
                        shelp.push_back(liste[il]);
                    }
                } else if (code.op == UPPER) {
                   sr = shelp.back();
                   shelp.pop_back();
                   shelp.push_back(toUpper(sr));
                } else if (code.op == LOWER) {
                   sr = shelp.back();
                   shelp.pop_back();
                   shelp.push_back(toLower(sr));
                } else if (code.op == XISIP) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "xisip", "khil");
                        return vempty;
                    }

                    sl = shelp.back();
                    il = xtoll(sl);
                    shelp.pop_back();
                    shelp.push_back(janxilist[il-1]);
                } else if (code.op == ANSIP) {
                    if (mode == SINGLE) {
                        logg.logmsg(ERROR, OPCODENOTALLOWED, "ansip", "khil");
                        return vempty;
                    }
                   sl = shelp.back();
                   il = xtoll(sl);
                   shelp.pop_back();
                   shelp.push_back(janlist[il-1]);
                } else if (code.op == OUT) {
                   if (mode == SINGLE) {
                       logg.logmsg(ERROR, OPCODENOTALLOWED, "pri", "khil");
                       return vempty;
                   }
                   sr = shelp.back();
                   shelp.pop_back();
                   result.push_back(sr);
                } else if (code.op == DROP) {
                   shelp.pop_back();
                } else if (code.op == END) {
                   break;
                } else if (code.op == SKIPZ) {
                   sr = shelp.back();
                   shelp.pop_back();
                   if (sr == "bu") {j+= code.left;}
                }
                ++j;
            }

             i+= incr;
        }
    } catch (exception& e) {
          logg.logmsg4(FATAL, ERRORBAIKHIL, cstos(e.what()), "", join(ops, " "), to_string(shelp.size()));
          if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC2, "bai/khil", to_string(stopExecution), "9");};
          stopExecution = 9;
    }

    if (shelp.size() > 0) {
        if (mode == SINGLE) {
            for (int i=0; i<(int) shelp.size(); ++i) {
               result.push_back(shelp[i]);
            }
        } else {
           logg.logmsg(WARNALL, NOTALLRESULTSUSED, "bai", to_string(shelp.size()));
        }
    }

    if (mode == MULTI) {
         if (debug_t) {logg.debugtime(TIMEBS, "", "");};
    } else {
         if (debug_t) {logg.debugtime(TIMEES, "", "");};
    }

    return result;
}


long long int Interpreter::getRight(string op, int pre) {
    string s = op.substr(pre);
     if (debug_b) {logg.logdbg3(NOWCONVERTING, s, "", "");};
    return xtoll(s);
 }

 // get  max. var index value
long long int Interpreter::getRowLength(vector<string> v) {
    long long int result = 1;
    string code;
    for (int i=0;i<(int) v.size(); ++i) {
        code = v[i];
        if (code.substr(0,3)== "suo") {
            result = max(result, xtoll(code.substr(3)));
        }
    }
    return result;
 }

vector<Opcode> Interpreter::toOpcode(vector<string> v) {
    vector<Opcode> result;
    string code;
    Numtest nt;
    for (int i=0;i<(int) v.size(); ++i) {
        code = v[i];

        if ((code == "thek") || (code == "fee")) {continue;}
        Opcode op;
        op.sleft = "";
        op.operands = 2;
        if (code.substr(0,3) == "tut") {
            op.op = ROW;
            op.left = getRight(code, 3);
            op.operands = 0;
        } else if (code == "nea"){
            op.op = AND;
            op.left = 0;
        } else if (code == "ren") {
            op.op = OR;
            op.left = 0;
        } else if (code == "?") {
            op.op = ELVIS;
            op.left = 0;
            op.operands = 3;
         } else if (code == "parai") {
            op.op = PLUS;
            op.left = 0;
         } else if (code == "shax") {
            op.op = MINUS;
            op.left = 0;
         } else if (code == "sotai") {
            op.op = TIMES;
            op.left = 0;
         } else if (code == "nymon") {
            op.op = DIVIDE;
            op.left = 0;
         } else if (code == "sigoon") {
            op.op = CONCAT;
            op.left = 0;
         }  else if (code == "furyy") {
            op.op = POWER;
            op.left = 0;
         } else if (code == "seq") {
            op.op = EQ;
            op.left = 0;
         } else if (code == "semu") {
            op.op = NEQ;
            op.left = 0;
         } else if (code == "bosamchosoor") {
            op.op = GE;
            op.left = 0;
         } else if (code == "bosamysoor") {
            op.op = LE;
            op.left = 0;
         } else if (code == "basycho") {
            op.op = GT;
            op.left = 0;
         } else if (code == "bass") {
            op.op = LT;
            op.left = 0;
         } else if (code == "mu") {
            op.op = NOT;
            op.left = 0;
            op.operands = 1;
         } else if (code == "vae") {
            op.op = LEN;
            op.left = 0;
            op.operands = 1;
         } else if (code == "juu") {
            op.op = ABS;
            op.left = 0;
            op.operands = 1;
         } else if (code == "hor") {
            op.op = SQRT;
            op.left = 0;
            op.operands = 1;
         } else if (code == "ci") {
            op.op = CEIL;
            op.left = 0;
            op.operands = 1;
         } else if (code == "xo") {
            op.op = FLOOR;
            op.left = 0;
            op.operands = 1;
         } else if (code == "mul") {
            op.op = ROUND;
            op.left = 0;
            op.operands = 1;
         } else if (code == "sin") {
            op.op = SIN;
            op.left = 0;
            op.operands = 1;
         } else if (code == "cos") {
            op.op = COS;
            op.left = 0;
            op.operands = 1;
         } else if (code == "tan") {
            op.op = TAN;
            op.left = 0;
            op.operands = 1;
         } else if (code == "pi") {
            op.op = VAL;
            op.sleft = "3.141592653589793";
            op.operands = 0;
         } else if (code == "asin") {
            op.op = ARCSIN;
            op.left = 0;
            op.operands = 1;
         } else if (code == "acos") {
            op.op = ARCCOS;
            op.left = 0;
            op.operands = 1;
         } else if (code == "atan") {
            op.op = ARCTAN;
            op.left = 0;
            op.operands = 1;
         } else if (code == "exp") {
            op.op = EXP;
            op.left = 0;
            op.operands = 1;
         } else if (code == "log") {
            op.op = LOG;
            op.left = 0;
            op.operands = 1;
         } else if (code == "logneel") {
            op.op = LOG10;
            op.left = 0;
            op.operands = 1;
         } else if (code == "sinh") {
            op.op = SINH;
            op.left = 0;
            op.operands = 1;
         } else if (code == "cosh") {
            op.op = COSH;
            op.left = 0;
            op.operands = 1;
         } else if (code == "tanh") {
            op.op = TANH;
            op.left = 0;
            op.operands = 1;
         } else if (code == "pri") {
            op.op = OUT;
            op.left = 0;
            op.operands = 1;
        } else if (code == "mod") {
            op.op = MOD;
            op.left = 0;
        } else if (code == "ffox") {
            op.op = DIV;
            op.left = 0;
        } else if (code == "kra") {
            op.op = MIN;
            op.left = 0;
        } else if (code == "thii") {
            op.op = MAX;
            op.left = 0;
        } else if (code == "phe") {
            op.op = DROP;
            op.left = 0;
            op.operands = 1;
        } else if (code == "sec") {
            op.op = END;
            op.left = 0;
            op.operands = 0;
        }else if (code.substr(0,4) == "jaad") {
            op.op = SKIPZ;
            op.left = xtoll(code.substr(4));
            op.operands = 1;
        } else if (code.substr(0,4)== "sona") {
            op.op = VAL;
            op.sleft = code.substr(4);
            op.operands = 0;
        } else if (code.substr(0,3)== "sip") {
            op.op = IDX;
            op.left = 0;
            op.operands = 0;
        } else if (code.substr(0,3)== "suo") {
            op.op = VAR;
            op.operands = 0;
            op.left = xtoll(code.substr(3));
        } else if (code == "xisip") {
            op.op = XISIP;
            op.left = 0;
            op.operands = 1;
        } else if (code == "ansip") {
            op.op = ANSIP;
            op.left = 0;
            op.operands = 1;
        } else if (code == "thekah") {
            op.op = ISINT;
            op.left = 0;
            op.operands = 1;
        } else if (code == "feewah") {
            op.op = ISNUM;
            op.left = 0;
            op.operands = 1;
        } else if (code == "haar") {
            op.op = UPPER;
            op.left = 0;
            op.operands = 1;
        } else if (code == "lyym") {
            op.op = LOWER;
            op.left = 0;
            op.operands = 1;
        }else if (code.substr(0,3) == "tyy") {
            op.op = DEFAULT;
            op.sleft = code.substr(3);
            op.operands = 0;
            nt = testType(op.sleft);
            if ((nt == NUMINT) || (nt == JUUINT)) {
                op.left = xtoll(op.sleft);
                op.fleft = xtold(op.sleft);
                if (debug_b) {logg.logdbg3(RECOGNIZEDINTDEF, to_string(op.left), "", "");};
            } else if ((nt == NUMFLOAT) || (nt == JUUFLOAT)) {
                 op.fleft = xtold(op.sleft);
                 op.left = (long long int) op.fleft;
                if (debug_b) {logg.logdbg3(RECOGNIZEDFLOATDEF, to_string(op.fleft), "", "");};
            } else {
                op.op = VAL;
                 if (debug_b) {logg.logdbg3(RECOGNIZEDVAL, op.sleft, "", "");};
            }
        } else {
            // assume value and push item on stack
            op.op = VAL;
            op.operands = 0;
            op.sleft = code;
            Numtest nt = testType(code);
            if (nt == JUUINT) {
                op.left = xtoll(code);
                op.fleft = xtold(op.sleft);
            } else if (nt == JUUFLOAT) {
                op.fleft = xtold(code);
                op.left = (long long int) op.fleft;
                if (debug_b) {logg.logdbg3(RECOGNIZEDFLOAT, code, "", "");};
            } else {
                op.left = 0;
                op.fleft = 0.0;
            }
        }
        result.push_back(op);
    }

     return result;
}

int Interpreter::ipower(int b, int e) {
    int result = 1;
    if (e==0) {return 1;}
    for (int i=1;i<=e; ++i) {
        result *= b;
    }

    return result;
}

long long int Interpreter::lpower(long long int b, long long int e) {
    long long int result = 1;
    if (e==0) {return 1;}
    for (int i=1;i<=e; ++i) {
        result *= b;
    }

    return result;
}

// typ
int Interpreter::newStopExecution(int old, StopTypes stype) {
    int result = old;
    if (stype == PARAGRAPH) {
        if (old==3) {
            result = 2;
        } else if (old<3) {
            result = 0;
        }
    } else if (stype == LOOP) {
        if (old <= 3) {result=0;}
    }
    if (old != result) { if (debug_y) {logg.logdbg3(CHANGEDSTOPEXEC, to_string(old), to_string(result), "");};}
    return result;
}

vector<string> Interpreter::timeverb(vector<string> &tv) {
     vector<string> result;
     string help;
     string help1;
     string help2;
     for (int i = 0; i < (int) tv.size(); ++i) {
        if (tv[i] == "yue") {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            result.push_back(datetime.tmtoTime(ltm));
        } else if (tv[i] == "xab") {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            result.push_back(datetime.tmtoDate(ltm));
        } else if ((tv[i] == "parai")  && (result.size() >=2)){
            help2 = result.back();
            result.pop_back();
            help1 = result.back();
            result.pop_back();
            help = datetime.add(help1, help2);
            result.push_back(help);
        } else if ((tv[i] == "shax") && (result.size() >=2)){
            help2 = result.back();
            result.pop_back();
            help1 = result.back();
            result.pop_back();
            help = datetime.subtract(help1, help2);
            result.push_back(help);
        } else {
            help = tv[i];
            //cout << "found " << help << endl;
            Numtest nt = testType(help);
            // juubes must be checked first, then check for formats
            if (nt == JUUINT) {
                help = to_string(besToInt(help));
                // cout << tv[i] << "->" << help << endl;
                result.push_back(help);
            } else if (findInString('m', tv[i]) || findInString('p', tv[i]) || findInString('w', tv[i])) {
                datetime.setTimeFormat(tv[i]);
            } else if (findInString('W', tv[i]) || findInString('S', tv[i]) || findInString('j', tv[i])
                   || findInString('C', tv[i]) || findInString('b', tv[i]) || findInString('g', tv[i])){
                datetime.setDateFormat(tv[i]);
            } else {
                result.push_back(help);
            }

        }
     }
    return result;
}

// this function returns its result as the last variable
void Interpreter::sortRowsLong(int index, int rows, bool isDesc, vector<string> &v, vector<string> &p) {
     map<long long int,vector<string>> m;
     vector<string> w;
     long long int idx = 0;
     for (int i = 0; i < (int) v.size(); i=i+rows) {
        w.clear();
        idx = 0;
        for (int j = 0; j < rows; ++j) {
            w.push_back(v[i+j]);
            if (j==index-1) {idx = xtoll(v[i+j]);}
        }
        auto x = m.find(idx);
        if (x==m.end()) {
            m[idx]= w;
        } else {
            vector<string> z = x->second;
            addToVector(z, w);
            m[idx]= z;
        }
     }

     if (isDesc) {
         for (map<long long int,vector<string>>::reverse_iterator it=m.rbegin(); it!=m.rend(); ++it) {
            addToVector(p,it->second);
        }
      } else {
         for (map<long long int,vector<string>>::iterator it=m.begin(); it!=m.end(); ++it) {
            addToVector(p,it->second);
        }
     }
}

// this function returns its result as the last variable
void Interpreter::sortRowsString(int index, int rows, bool isDesc, vector<string> &v, vector<string> &p) {
     map<string,vector<string>> m;
     vector<string> w;
     string idx = "";
     for (int i = 0; i < (int) v.size(); i=i+rows) {
        w.clear();
        idx = "";
        for (int j = 0; j < rows; ++j) {
            w.push_back(v[i+j]);
            if (j==index-1) {
                idx = v[i+j];
            }
        }
        auto x = m.find(idx);
        if (x==m.end()) {
            m[idx]= w;
        } else {
            vector<string> z = x->second;
            addToVector(z, w);
            m[idx]= z;
        }
       }

     if (isDesc) {
         for (map<string,vector<string>>::reverse_iterator it=m.rbegin(); it!=m.rend(); ++it) {
            addToVector(p,it->second);
        }
      } else {
         for (map<string,vector<string>>::iterator it=m.begin(); it!=m.end(); ++it) {
            addToVector(p,it->second);
        }
     }
}

vector<string> Interpreter::freq(vector<string> &dolist,vector<string> &forlist) {
    unordered_map<string, long long int> f;
    vector<string> result;
    string op = forlist[0];
    try {
        // unify
        if (op == "kum") {
            for (int i = 0; i < (int) dolist.size(); i+=2) {
                string key = dolist[i];
                long long int val = xtoll(dolist[i+1]);
                 if (debug_x) {logg.logdbg3(PROCESSING, key, "", "");};
                if (f.find(key) != f.end()) {
                   val += f.at(key);
                   f.find(key)->second = val;
                    if (debug_x) {logg.logdbg3(FOUND, key, "", "");};
                } else {
                    f.insert(pair<string, long long int>(key, val));
                     if (debug_x) {logg.logdbg3(INSERT, key, "", "");};
                }
            }
            for (auto iter = f.begin(); iter != f.end(); ++iter){
                result.push_back(iter->first);
                result.push_back(intToBes(iter->second));
            }
          // most
         } else if (op == "thii") {
             if (debug_x) {logg.logdbg3(STARTPROCESSING, "thii", "", "");};
             long long int val = 0;
             vector<string> keys;
            for (int i = 0; i < (int) dolist.size(); i+=2) {
                string key2 = dolist[i];
                if (debug_x) {logg.logdbg3(PROCESSING, key2, "", "");};
                long long int val2 = xtoll(dolist[i+1]);
                if ((val2 > val) || (i==0)){
                    val = val2;
                    keys.clear();
                    keys.push_back(key2);
                } else if (val2 == val) {
                    keys.push_back(key2);
                }
            }
            for (int j = 0; j < (int) keys.size(); j++) {
                result.push_back(keys[j]);
                result.push_back(intToBes(val));
            }
        // least
        } else if (op == "kra") {
             if (debug_x) {logg.logdbg3(STARTPROCESSING, "kra", "", "");};
             long long int val = 0;
             vector<string> keys;
            for (int i = 0; i < (int) dolist.size(); i+=2) {
                string key2 = dolist[i];
                 if (debug_x) {logg.logdbg3(PROCESSING, key2, "", "");};
                long long int val2 = xtoll(dolist[i+1]);
                if ((val2 < val) || (i==0)) {
                    val = val2;
                    keys.clear();
                    if (debug_x) {logg.logdbg3(FOUNDNEWMIN, key2, "", "");};
                    keys.push_back(key2);
                } else if (val2 == val) {
                    keys.push_back(key2);
                    if (debug_x) {logg.logdbg3(ADDEDNEWMIN, key2, "", "");};
                }
            }
            for (int j = 0; j < (int) keys.size(); j++) {
                result.push_back(keys[j]);
                result.push_back(intToBes(val));
            }
        }
    } catch (exception& e) {
        logg.logmsg3(FATAL, COULDNOTEXECUTE, cstos(e.what()), op, "faon");
        stopExecution = 9;
    }

    return result;

}

int Interpreter::vmemsize(vector<string> v) {
    return sizeof(v) + sizeof(string) * v.capacity();
}

int Interpreter::mmemsize(unordered_map<int, vector<string>> slists) {
    int ms = 0;
    auto iter = slists.begin();
    while (iter != slists.end()) {
        ms +=  sizeof(iter->first) + sizeof(iter->second) * iter->second.capacity();
        ++iter;
    }
    return ms;
}

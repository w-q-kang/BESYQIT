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

#include "Third.h"
#include "globals.h"
#include <unordered_map>

using namespace std;

Third::Third()
{
    //ctor
}

Third::~Third()
{
    //dtor
}

int Third::thirdParse(vector<Sentence> v) {
    int ok = 0;
    paragraphs.clear();
    Paragraph *p;
    int len = v.size();
    int seq;
    p = new Paragraph();
    vector<int> relativedists;
    vector<int> absdists;
    unordered_map<string,int> snames;
    string help;
    for (int i = 0; i < len; ++i) {
        Sentence s = v[i];
        seq = s.getSeq();
        if (seq == 0) {
            if (p->getLength() > 0) {
                if (debug_P) {log.logdbg3(PARAGRAPHFOUND, to_string(p->getLength()), "", "");}
                for (int j = 0; j < (int) absdists.size(); ++j) {
                    int index = absdists[j];
                    if (index >= 0) {
                        p->setSaved(index);
                    } else {
                        log.logmsg(FATAL, WRONGREFDISTANCE0, "", "");
                        ok = -1;
                    }
                }

                paragraphs.push_back(*p);
                absdists.clear();
                relativedists.clear();
            }
            p = new Paragraph();
            snames.clear();
            if (debug_P) {log.logdbg3(CLEARSNAMES, "", "", "");}
        }

        try {
            help = s.getRefname();
            if (help.size()>0) {
               if (snames.find(help) != snames.end()) {
                    log.logmsg(FATAL, SENTENCENAMENOTUNIQUE, help, "");
                    ok = -1;
               } else {
                    snames[help] = i;
                    s.setSaved();
               }
            }
        } catch (exception& e) {
            string s = e.what();
            log.logmsg3(FATAL, ERRTHIRDPARSE3, s, to_string(i), help);
            ok = -1;
        }

        // set absolute
        relativedists = s.getThatRefs();
        for (int j = 0; j < (int) relativedists.size(); ++j) {
            absdists.push_back(seq - relativedists[j]);
        }
        // jump must always be calculated
        int jump = calculateJump(i, v);
        s.setJump(jump);

        p->addSentence(s);
    }

    if (p->getLength() > 0) {
         for (int j = 0; j < (int) absdists.size(); ++j) {
             int index = absdists[j];
             if (index >= 0) {
                p->setSaved(index);
             } else {
                log.logmsg(FATAL, WRONGREFDISTANCE, joinint(absdists, ","), "");
                ok = -1;
             }
        }
        paragraphs.push_back(*p);
    }

    if (!checkParagraphs()) {ok=-1;}
    return ok;
 };


string Third::getContentString() {
    int len = paragraphs.size();
    string result = "";
    result = to_string(len ) + " " + log.getMessage(PARRECOGNIZED) + "\n\n";
    for (int i = 0; i < len; ++i) {
        result += paragraphs[i].getContentString() + "\n";
        result += "------------- " + log.getMessage(PAREND) + " ------------\n\n";
    }
    return result;
};

vector<Paragraph> Third::getParagraphs() {
    return paragraphs;
}

// check, if all referenced paragraphs exist
//        and all paragraphs are named
//        and no paragraph name is defined more than once
bool Third::checkParagraphs() {
    vector<string> pnames;
    vector<string> vnames;
    string notfound = "";
    string multiples = "";
    int unnamed = 0;
    bool result = true;
    for (int i=0; i<(int) paragraphs.size(); ++i) {
        Paragraph p = paragraphs[i];
        string pname = p.getName();
        if (pname.length() == 0) {
            if (i>0) {++unnamed;}
        } else if (!contains(pname, pnames)) {
            pnames.push_back(p.getName());
        } else {
            if (multiples.size() > 0) {multiples += ",";}
             multiples += pname;
        }
        int len = p.getLength();
        for (int j=0; j<len; ++j) {
            Sentence s = p.getSentence(j);
            Item item = s.getLastItem();
            if (item.getVerbType() == 3) {
                string name = item.getName();
                if (!contains(name, vnames)) {
                    vnames.push_back(name);
                }
            }
        }
    }
    for (int i=0; i<(int) vnames.size(); ++i) {
        if (!contains(vnames[i], pnames)) {
            if (notfound.size() > 0) {notfound += ",";}
             notfound += vnames[i];
        }
    }

    if (notfound.size()>0) {
        log.logmsg(FATAL, PARAGRAPHSNOTFOUND, notfound, "");
        result = false;
    }
    if (multiples.size()>0) {
        log.logmsg(FATAL, PARAGRAPHSMORETHANONCE, multiples, "");
        result = false;
    }
    if (unnamed>0) {
        log.logmsg(FATAL, PARAGRAPHSUNNAMED, to_string(unnamed), "");
        result = false;
    }
    return result;
}

int Third::calculateJump(int index, vector<Sentence> v) {
    int jump = 1;
    Sentence s = v[index];
    int currLT = s.getLabeltype();
    if (currLT == 0) {
        jump = 1;
    } else if (currLT == 1) {
        jump = calculateJumpLabel(index, v);
    } else if (currLT == 2) {
        jump = calculateJumpInside(index, v);
    } else if (currLT == 3) {
        jump = calculateJumpEnd(index, v);
    }

    return jump;
}


int Third::calculateJumpLabel(int index, vector<Sentence> &v) {
    int jump = 1;
    bool found = false;
    Sentence s = v[index];
    int currLevel = s.getCondLevel();
    int currSeq = s.getSeq();

    try {
        for (int i=index+1; i<(int) v.size(); ++i) {
            Sentence succ = v[i];
            int succLevel = succ.getCondLevel();
            int succLT = succ.getLabeltype();
            int succSeq = succ.getSeq();
            // found next paragraph
            if (succSeq < currSeq) {break;}

            if (succLT == 0) {
               log.logmsg(ERROR, MALFORMEDCONDSEQ, to_string(index), "");
               break;
            } else if ((succLT == 3) && (succLevel == 1)) {
                // found last end label
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT1/3(0)", to_string(index), "");};
                    found = true;
                    break;
            } else if (((succLT == 1) || (succLT == 3)) && (currLevel == succLevel)) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT1/1,3", to_string(index), "");};
                    found = true;
                    break;
            } else if (succLevel < currLevel) {
                currLevel = succLevel;
            }
        }
    } catch(exception &e) {
         string s = e.what();
         log.logmsg(FATAL, ERRTHIRDPARSE, s, "");
    }

	if (!found) {
      log.logmsg(ERROR, MISSINGENDLABEL, to_string(index), "");
	}

	if (debug_j) {log.logdbg3(CALCJUMP, to_string(index), to_string(jump), "");};
    return jump;
}

int Third::calculateJumpEnd(int index, vector<Sentence> &v) {
    int jump = 1;
    bool found = false;
    bool missed = false;
    Sentence s = v[index];
    int currLevel = s.getCondLevel() - 1;
    int currSeq = s.getSeq();
    if (currLevel == 0) {return jump;}
    bool isNext = true;

    try {
        for (int i=index+1; i<(int) v.size(); ++i) {
            Sentence succ = v[i];
            int succLevel = succ.getCondLevel();
            int succLT = succ.getLabeltype();
            int succSeq = succ.getSeq();
            // found next paragraph
            if (succSeq < currSeq) {break;}

            if (succLT == 0) {
                if (currLevel == 0) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT3/0", to_string(index), "");};
                    found = true;
                    break;
                } else {
                    log.logmsg(ERROR, MALFORMEDCONDSEQ, to_string(index), "");
                }
                break;
            } else if ((succLT == 2) && (currLevel == succLevel) && !missed && isNext) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT3/2", to_string(index), "");};
                    found = true;
                    break;
            } else if ((succLT == 3) && (currLevel == succLevel)) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT3/3", to_string(index), "");};
                    found = true;
                    break;
            } else if (succLevel < currLevel) {
                currLevel = succLevel;
                missed = true;
            }

            isNext = false;
        }
    } catch(exception &e) {
         string s = e.what();
         log.logmsg(FATAL, ERRTHIRDPARSE, s,"");
    }

	if (!found) {
      log.logmsg(ERROR, MISSINGENDLABEL, to_string(index), "");
	}

	if (debug_j) {log.logdbg3(CALCJUMP, to_string(index), to_string(jump), "");};
    return jump;
}

int Third::calculateJumpInside(int index, vector<Sentence> &v) {
    int jump = 1;
    bool found = false;
    bool missed = false;
    Sentence s = v[index];
    int currLevel = s.getCondLevel();
    int currSeq = s.getSeq();

    try {
        for (int i=index+1; i<(int) v.size(); ++i) {
            Sentence succ = v[i];
            int succLevel = succ.getCondLevel();
            int succLT = succ.getLabeltype();
            int succSeq = succ.getSeq();
            // found next paragraph
            if (succSeq < currSeq) {break;}

            if (succLT == 0) {
               log.logmsg(ERROR, MALFORMEDCONDSEQ, to_string(index), "");
               break;
            } else if ((succLT == 3) && (succLevel == 1)) {
                // found last end label
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT2/3(0)", to_string(index), "");};
                    found = true;
                    break;
            } else if ((succLT == 1) && (currLevel == succLevel)) {
                    missed = true;
            } else if ((succLT == 1) && (currLevel < succLevel) && !missed) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT2/1", to_string(index), "");};
                    found = true;
                    break;
            } else if ((succLT == 2) && (currLevel == succLevel) && !missed) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT2/2", to_string(index), "");};
                    found = true;
                    break;
            } else if ((succLT == 3) && (currLevel == succLevel)) {
                    jump = i - index;
                    if (debug_j) {log.logdbg3(CALCJUMPBREAK, "LT2/3", to_string(index), "");};
                    found = true;
                    break;
            } else if (succLevel < currLevel) {
                currLevel = succLevel;
                missed = true;
            }

        }
    } catch(exception &e) {
         string s = e.what();
         log.logmsg(FATAL, ERRTHIRDPARSE, s, "");
    }

	if (!found) {
      log.logmsg(ERROR, MISSINGENDLABEL, to_string(index), "");
	}

	if (debug_j) {log.logdbg3(CALCJUMP, to_string(index), to_string(jump), "");};
    return jump;
}


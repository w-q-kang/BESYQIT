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

#include "Second.h"
#include <unordered_map>
#include <algorithm>
#include "globals.h"
#include "Compliance.h"

using namespace std;

Second::Second()
{

}

Second::~Second()
{
}

void Second::setAllVerbs(vector<string>& paragraphs) {
    verbnames = paragraphs;
    addToVector(verbnames, standard);
    addToVector(verbnames, other);
    // we have to sort by string length
    sort(verbnames.begin(), verbnames.end(), [](string a, string b) {return a.length() > b.length();});

    if (debug_P) {log.debug("complete verb stem list: " + join(verbnames, ", "));}
}

// group the items to (numbered) sentences
int Second::secondParse(vector<Item> v) {
    int ok = 0;
    sentences.clear();
    Sentence *s;
    int len = v.size();
    char t;
    char oldType = '?';
    int count = -1;
    s = new Sentence();
    int scount = 1;
    bool isHeader = true;
    int labelType = 0; // 0=no label, 1=ordinary,2 = inside, 3 = end
    int oldCondLevel = 0;
    int oldLT = 0;
    LogMsg errpos;
    string errmsg;
    unordered_map<string, int> snames;
    vector<string> snameslist; // duplicate of snames but for easier usage
    Compliance cpl = Compliance();
    try {
        for (int i = 0; i < len; ++i) {
            Item item = v[i];
            t = item.getTyp();
            if ((oldType == 's') || (t == 's')) {
                // skip, not included in sentence but used to modify ref
            } else if (t == 'c') {
                labelType = 1;
                if (isEndLabel(item.getName())) {labelType = 3;}
                s->addItem(item);
                oldCondLevel = item.getCondLevel();
                oldLT = labelType;
            } else if (t == 'l') {
                snameslist.clear();
                // should also clear snames?
                if (s->getName().size() > 0) {
                    log.logmsg(WARN, MORETHANONEPAR, "", "");
                } else {
                    s->setName(item.getName());
                }
                oldCondLevel = 0;
                oldLT = 0;

                if (!isWeak) {
                    tie(errpos, errmsg) = cpl.test(item.getName());
                    if(errpos != NOMSG) {
                       log.logmsg(FATAL, errpos, errmsg , item.getName());
                       ok = -1;
                    }
                }
            } else if (t == 'v') {
               ++count;
               item.secondRoundVerb(verbnames);
               s->addItem(item);
               s->setSeq(count);
               s->setLabelType(labelType);
               s->setCondLevel(oldCondLevel);
               // if the last sentence was an end label, then decrement condlevel by 1
               if (oldLT == 3) {s->setCondLevel(oldCondLevel - 1);}

               try {
                   string sentref = item.getSentenceRef();
                   if (sentref.size() > 0) {
                        s->setRefname(sentref);
                        snames[sentref] = scount;
                        snameslist.push_back(sentref);
                        // we have to sort by string length
                        sort(snameslist.begin(), snameslist.end(), [](string a, string b) {return a.length() > b.length();});
                       if (!isWeak) {
                            tie(errpos, errmsg) = cpl.test("bes" + sentref);
                            if(errpos != NOMSG) {
                               log.logmsg(FATAL, errpos, errmsg , "bes" + sentref);
                               ok = -1;
                            }
                        }
                   }
               } catch (exception& e) {
                    string s = e.what();
                    log.logmsg(FATAL, TRYSETREFNAME, s , "");
                    ok = -1;
               }
               if (isHeader) {s->setHeader();}
               s->setParams();
               sentences.push_back(*s);
               s = new Sentence();
               ++scount;
               labelType = changeLT(labelType, oldCondLevel);
            } else if (t == 'n') {
                if (i>0) {
                  if ((labelType == 1) || (labelType == 3)) {
                     ++count;
                     Item item = Item("janxi");
                     s->addItem(item);
                     string x = (labelType == 1) ? "hekana." : "mugana.";
                     item = Item(x);
                     s->addItem(item);
                     s->setSeq(count);
                     s->setLabelType(labelType);
                     s->setCondLevel(oldCondLevel);
                     s->setParams();
                     sentences.push_back(*s);
                     s = new Sentence();
                     ++scount;
                     labelType = changeLT(labelType, oldCondLevel);
                  }
                  if (v[i-1].getTyp() == 'n') {
                     s = new Sentence();
                     ++scount;
                     count=-1;
                     isHeader = false;
                  }
                }
            } else if (t == 'r') {
                try {

                    item.secondRoundRef(snameslist);
                    // sipar must be set after secondroundref
                    if (i+2 < len) {
                        Item item2 = v[i+1];
                        if (item2.getTyp() == 's') {
                            item2 = v[i+2];
                            item.setSipar(besToInt(item2.getName()));
                        }
                    }

                    if (item.getName() == "bes") {
                        string sref = item.getSentenceRef();
                        if(sref.size() > 0) {
                            if (snames.find(sref) != snames.end()) {
                                int dist = scount - snames[sref];
                                 if (dist <= 0) {
                                    log.logmsg(FATAL, INVALIDREFDIST, item.content(true, true), "");
                                    ok = -1;
                                } else {
                                    item.setRefdistance(dist);
                                    if (dist == 1) {
                                        log.logmsg(FATAL, DONTREPLACETHIS, sref, "");
                                        ok = -1;
                                    }
                                }
                            } else {
                                log.logmsg(FATAL, SENTREFNOTFOUND, sref, "");
                                ok = -1;
                            }
                        }
                    }
                    s->addItem(item);
                } catch (exception& e) {
                    string s = e.what();
                    log.logmsg(FATAL, TRYSETREF, s, "");
                    ok = -1;
                }
            } else {
                s->addItem(item);
            }
            oldType = t;
        }
    } catch(exception &e) {
       string s = e.what();
       log.logmsg(FATAL, ERRSECONDPARSE, s, "");
       ok = -1;
    }
    return ok;
}

string Second::getContentString() {
    int len = sentences.size();
    string result = "";
    for (int i = 0; i < len; ++i) {
        result += sentences[i].getContentString(false, false) + "\n";
    }
    return result;
}

vector<Sentence> Second::getSentences() {
    return sentences;
}

int Second::changeLT(int labelType, int condLevel) {
    int result = labelType;
    if (labelType == 1) {
        result = 2;
    } else if (labelType == 3) {
        if (condLevel <= 1) {
            result = 0;
        } else {
            result = 2;
        }
    }

    return result;
}

bool Second::isEndLabel(string name) {
    bool result = name == "sec";

    if (!result && name.size() >=4) {
        string s = name.substr(0,4);
        result = s == "sec.";
    }
    return result;
}

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
    jan siparavlea swigana
*/

#include "Log.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

Log::Log()
{
    if (!init) {
        init = true;
        messagemap[MSG1] = {"#", "#"};
        messagemap[MSG2] = {"##", "##"};
        messagemap[BARSONLYFORREF] ={"entry # must not be enclosed in ||", "'#' jan sagoet '||' jannyr  kummusoorana." };
        messagemap[ERRORLEXANLYZEREF] ={"#: error when lexical analyzing reference #", "#: '#' jan ikethi tuuty taaqi jan booloryyet khacana."};
        messagemap[MUSTBEVERB] = {"entry # must be a verb", "'#' jan dooet guas gejuqxi guahetnyr swigana."};
        messagemap[VERBMODNOTRECOG] = {"verb modifier # not recognized.", "'#' jaadydoo jan guas gejuqet sanmugana."};
        messagemap[REFMODNOTRECOG] = {"ref modifier # not recognized.", "'#' jaadydoo jan iket sanmugana."};
        messagemap[MODSEQNOTRECOG] = {"modifier sequence # not recognized.", "'#' jaadydoo jan tutyyet sanmugana."};
        messagemap[MORETHANONEPAR] = {"more than one paragraph name found - new names will be ignored.", "jan gejuqet faoryygana. beqav jan sagoet khoesana."};
        messagemap[TRYSETREFNAME] = {"#: error when trying to set refname", "#: sago jan besav iket beqonyr suunavydooet khacana."};
        messagemap[INVALIDREFDIST] = {"invalid ref. distance found for item #", "# jan dooet khac jan bassydoo jan iket beqavana."};
        messagemap[DONTREPLACETHIS] = {"you must not replace 'this' by a named sentence reference - #", "'yii'et besav ikxi ikmusoorana - #"};
        messagemap[SENTREFNOTFOUND] = {"sentence reference # not found.", "# jan besav iket beqavmugana."};
        messagemap[TRYSETREF] = {"#: error when trying to set ref", "#: besav iket beqonyr suunavydooet khacana."};
        messagemap[ERRSECONDPARSE] = {"#: error when doing second parse", "#: <parys>hi sipar te khacana."};
        messagemap[UNKNOWNGROUP] = {"unknown group name #", "# esojan bes copydooet beqavmugana."};
        messagemap[IGNOREUNKNOWNTYPE] = {"item with unknown type # is ignored.", "'#' sanmudoo rimoodoo jan dooet khoesana."};
        messagemap[UNKNOWNFILE] = {"file to be executed doesn't exist: #", "# jan benien vooset swimugana."};
        messagemap[UNKNOWNOPTION] = {"Unknown option #", "# jan renydooet sanmugana."};
        messagemap[MISSINGGROUP] = {"group name missing for verb # - parameters will be ignored: #", "esojan bes copydooet  # guas gejuqnyr beqavmugana. - # jan ikyyet  khoesana."};
        messagemap[MISSINGENDLABEL] = {"condition jump end not found for sentence #. Maybe end) label missing.", "# jan besavet hek ar jan secet beqavmugana. sec sagoet sipar da swimelomugana."};
        messagemap[SENTENCENAMENOTUNIQUE] = {"You must not use the same sentence name (#) more than once in a program.", "besav jan sago (#) da jan beqavydoonyr beqonyr loabmusoorana."};
        messagemap[ERRTHIRDPARSE] = {"#: error when doing third parse (when calculating jump)", "#: <parys>hi sipar sol (aret khilhi) khacana."};
        messagemap[ERRTHIRDPARSE3] = {"#: setting ref in sentence [#]: error when doing third parse #", "#: iket [#] jan besavhi suunavana: <parys>hi sipar sol khacana. - #"};
        messagemap[PARAGRAPHSNOTFOUND] = {"referenced paragraph(s) # do not exist.", "# ik jan gejuq swimugana."};
        messagemap[PARAGRAPHSMORETHANONCE] = {"paragraph name(s) # are defined more than once.", "# gejuq jan sagoet faonnyr fawaanana."};
        messagemap[PARAGRAPHSUNNAMED] = {"# paragraph(s) are unnamed.", "# jan gejuqet sagomugana."};
        messagemap[PARAGRAPHUNNAMED] = {"paragraph # is unnamed.", "# jan gejuqet sagomugana."};
        messagemap[MALFORMEDCONDSEQ] = {"malformed conditional sequence found for sentence #. Maybe end) label missing.", "# jan besavhi khac hek jan tutyyet beqavana. sec sagoet sipar da swimelomugana."};
        messagemap[WRONGREFDISTANCE] = {"wrong ref distance found - ck . program. Abs dists are: \n\n #", "besav ik jan bassydooet khacana. - besyqit khacana! - juu(bassydoo) swigana: \n\n #"};
        messagemap[WRONGREFDISTANCE0] = {"wrong ref distance found for seq 0 - ck . program.", "besav ik jan bassydooet bu tutyyhi khacana. - besyqit khacana!"};
        messagemap[USERDEFNOG] = {"#: a user defined paragraph must not end with a 'g'", "#: beqo jan loab jan gejuqlea 'g' jan ganoryyet swimusoorana."};
        messagemap[INITNOTCOMPLIES] = {"cluster '#' not allowed initially in name #.", "'#' jan gejucet '#' benienvhi naagoryyhi swimusoorana."};
        messagemap[NAMENOTCOMPLIES] = {"cluster '#' not allowed in name #.", "'#' jan gejucet '#' benienhi swimusoorana."};
        messagemap[ENDNOTCOMPLIES] = {"cluster '#' not allowed finally in name #.", "'#' jan gejucet '#' benienhi gaanoryyhi swimusoorana."};
        messagemap[INITENDNOTCOMPLIES] = {"cluster '#' not allowed initially or finally in name #.", "'#' jan gejucet '#' benienhi gaanoryyhilea naagoryyhi swimusoorana."};
        messagemap[PARSEFAIL] = {"no execution because parsing failed." , "<parys>et khacana. khil swimugana."};
        messagemap[CURRENTVERSION] = {"current version is #", "<kyren> thek # jan doo swigana."};
        messagemap[FIRSTPARSEFAIL] = {"#: error when trying to execute first parse", "#: <parys>et sipar te beqonyr khilydooet khacana."};
        messagemap[PREPROCESSING] = {"preprocessing file #", "# jan benien vooset khilydoo sipar te swigana."};
        messagemap[WHENPREPROCESSHEAD] = {"#: when executing Preprocess process/header", "#: ci jan khilydooet sipar te khilana."};
        messagemap[WHENPREPROCESSBODY] = {"#: when executing Preprocess process/bodies", "#: xo jan khilydooet sipar te khilana."};
        messagemap[PARRECOGNIZED] = {"paragraph(s) recognized", "gejuq swigana."};
        messagemap[PAREND] = {"PARAGRAPH END", "GEJUQ SECANA."};
        messagemap[NOTANINT] = {"# is not an integer", "'#' thek swimugana."};
        messagemap[NOTANUMBER] = {"# is not a number", "'#' (fee) thek swimugana."};
        messagemap[NAMEISHEADER] = {"name=header", "ci sago swigana."};
        messagemap[NAMEIS] = {"name = #", "sago # swigana."};
        messagemap[CURRENTLOGLEVEL] = {"Current log level is #", "<kyren>  soon jan siparavet # jan doo swigana."};
        messagemap[WORKDIR] = {"Working directory is '#'", "loab lundataret '#' swigana."};
        messagemap[CURRENTDIR] = {"Current directory is '#'", "<kyren> lundataret '#' swigana."};
        messagemap[PARAGRAPHMULTIPLE] = {"paragraph name # was used multiple times", "# gejuq jan sagoet faonnyr fawaanana."};
        messagemap[OPCODENOTALLOWED] = {"Opcodes # not allowed in #", " # jan 'Opcode'et '#'hi swimusoorana."};
        messagemap[NOTENOUGHOPS] = {"not enough operands for opcode #", "theket # jan 'Opcode'nyr faonavana."};
        messagemap[NOTENOUGHOPSINSEQ] = {"not enough operands for Opcode # in seq. #", "theket # jan 'Opcode'nyr # jan tutyyhi faonavana."};
        messagemap[COULDNOTEXECUTE] = {"#: could not execute # on #", "#: '#'et '#'nyr khilmugana."};
        messagemap[INDEXNOTEXISTS] = {"non-existing index # found and no default set.", "# jan oorut jan theket swimudoo beqavana. lunakhiet swimugana."};
        messagemap[NOTALLRESULTSUSED] = {"not all op results used for # - # left.", " chanmu jan thek jaadydoo jan meeret '#'nyr  beqofo loabana. '#'et faoryygana."};
        messagemap[ERRORWHENEXEC] = {"#: error when executing # for #", " ", "'#'et khacana. '#'et '#'nyr khilana."};
        messagemap[ERRORWHENEXECPLAIN] = {"#: error when executing #", " ", "'#'et khacana. '#'et khilana."};
        messagemap[STANDARDOUTEMPTY] = {"the output of standard verb # is empty.", "# jan guas gejuqhi haanavydooet wayuqana."};
        messagemap[ERROREXECJANET] = {"#: error when executing # for janetlist of length # at #", "#: '#'et khilydooet # jan vae jan 'janet' jan soonnyr '#'hi khacana."};
        messagemap[INPUTWAS] = {"input # was #", "# jan taqivet # jan doo swigana."};
        messagemap[MODIFIERIGNORED] = {"modifier '#' is ignored for verb #.", "# jan jaadydooet # jan guas gejuqhi khoesana."};
        messagemap[ERRORBAIKHIL] = {"#: error when executing bai/khil# for # - ihelp.size = #", "#: bai/khil#et khilydoohi '#'nyr khacana. vae jan 'ihelp'et # swigana."};
        messagemap[TUTLENGTHNOTINT] = {"tut length is not an integer: # - assume 1.", "'tut' vaeet thek swimugana: #. teet suunavana."};
        messagemap[FILENOTDEL] = {"file # not deleted.", "# jan benien voosxi phemugana."};
        messagemap[NOTADIR] = {"# is not a directory -  no deletion.", "'#' lundatar swimugana. phemugana."};
        messagemap[DIRNOTDEL] = {"directory # not deleted.", "# jan lundatarxi phemugana."};
        messagemap[DIRNOTCREATE] = {"directory # not created.", "# jan lundatarxi fawaanmugana."};
        messagemap[NOBARSEP] = {"no bar separator '|' found in (g)e/(g)ete pattern", "'|'et 'ge/gete' jan rimoohi beqavmugana."};
        messagemap[FILEMISSING] = {"file name missing for mud", "benien voos jan sagoet 'mud'xi beqavmugana."};
        messagemap[MUDNOEXEC] = {"#: could not execute mud on file #", "#: # jan benien vooset 'mud'xi khilmugana."};
        messagemap[COPMISSINGVAL] = {"missing value for cop was ignored", "sonaet 'cop'nyr beqavmudooet khoesana."};
        messagemap[ERROREXECKHI] = {"error when executing khi: size of output != input", "khiet khilhi khacana. vae jan haanavydooet vae jan taqiv swimugana."};
        messagemap[NOEXECONFILE] = {"#: could not execute # on file #", "#: '#'et # jan benien vooshi khilmugana."};
        messagemap[VERBNOTALLOWED] = {"verb not allowed for # or unknown: #.", "#2 jan guas gejuq '#1'nyr swimusoorana. sanmudooet renana."};
        messagemap[EMPTYRESULTIGNORED] = {"empty result for # in ~# - ignored.", "wayuq meeret '#'nyr ~# jan doohi swigana. khoesana."};
        messagemap[ERRINSENTOFPAR] = {"# in sentence # of paragraph # at level #", "'#'et # jan besavhi # jan gejuqlea # jan siparavlea swigana."};
        messagemap[ERREXECSENTENCE] = {"error occured when executing # - hint: #", "khilydoo jan '#'et khacana. '#'et beqofo diigana."};
        messagemap[UNKNOWNTYPE] = {"item with unknown type # is ignored.", "# jan sanmudoo rimoodoo jan dooet khoesana."};
        messagemap[ERRINPREP] = {"# in preprocess of sentence # at level # - hint: #", "'#'et # jan khilydoo sipar te jan besavhi # jan siparavlea swigana. '#'et beqofo diigana"};
        messagemap[ERRWHENPROCESSING] = {"#: when processing ref.+modifiers for #[#..#]", "#: iket jadydoolea '#[#..#]'nyr khilana."};
        messagemap[SENTENCEREFNOTFOUND] = {"could not find sentence reference at #", "'#'hi besav jan iket beqavmugana."};
        messagemap[RESULTINVALID] = {"no valid result for # in (g)e/(g)ete.", "'ge/gete'hi # jan doonyr meeret beqavmugana."};
        messagemap[EMPTYENTRIES] = {"# empty entries found. Possibly incomplete pattern for #: # was ignored.", "# jan wayuq jan dooet beqavana. '#'nyr rimooet chanmumelogana. '#'et khoesana."};

        debugmap[TIMEINIT] = {"", ""};
        debugmap[TIMEFOR] ={"time for # ~#: #","vooshi # ~# swigana: #"};
        debugmap[TIMEBL] ={"#;bulk;long","#;bai;thek"};
        debugmap[TIMEEL] ={"#;expr;long","#;khil;thek"};
        debugmap[TIMEBF] ={"#;bulk;float", "#;bai;fee"};
        debugmap[TIMEEF] ={"#;expr;float","#;khil;fee"};
        debugmap[TIMEBS] ={"#;bulk;string","#;bai;benien"};
        debugmap[TIMEES] ={"#;expr;string","#;khil;benien"};

        debugmap[ENTRY] = {"Entry = #", "doo # swigana."};
        debugmap[FILELINECOUNT] = {"file line count is #", "thek jan mycypan jan benien vooset # swigana."};
        debugmap[PARSEVECTOR] = {"Parsing vector = #", "oorut <parys> # swigana."};
        debugmap[FIRSTFILELENGTH] = {"first file length is #", "vae jan benien voos sipar te # swigana."};
        debugmap[HEADERLENGTH] = {"header length is #", "vae jan ci # swigana."};
        debugmap[INCLUDENUMBER] = {"number of includes is #", "thek dii # swigana."};
        debugmap[HEADERLINECOUNT] = {"header line count is #", "thek jan mycypan jan ci # swigana."};
        debugmap[FIRSTBODYLINECOUNT] = {"first body line count is #", "thek jan mycypan jan xo sipar te # swigana."};
        debugmap[READINGINCLUDEFILE] = {"now reading include file #", "# jan dii jan benien vooset mudana."};
        debugmap[BODIESLINECOUNT] = {"bodies line count is #", "thek jan mycypan jan xo # swigana."};
        debugmap[PARAGRAPHFOUND] = {"paragraph found with length #", "# jan vaenyr gejuqet beqavana."};
        debugmap[CLEARSNAMES] = {"snames clear", "'snames' phegana."};
        debugmap[CALCJUMPBREAK] = {"calculateJump break # for sentence #", "# jan khil secydoo aret # jan besav swigana."};
        debugmap[CALCJUMP] = {"calculated jump for sentence # is #", "# jan besavet # jan khil ar swigana."};
        debugmap[SYMBOLS] = {"Symbols\n #", "<simbyl>et\n # \nswigana."};
        debugmap[VERBDEFS] = {"Found verbdefs: #", "guas gejuq fawaan jan lundooet #\nbeqavana."};
        debugmap[PARSERES] = {"Parser result is\n #", "<parys>et\n # \nswigana."};
        debugmap[PARSESECOND] = {"Second parse is\n #", "<parys>et sipar da\n # \nswigana."};
        debugmap[PARSETHIRD] = {"Third parse: \n #", "<parys>et sipar sol\n # \nswigana."};
        debugmap[PARAGRAPHENTER] = {"entering Paragraph # on level #", "# jan gejuqxi # jan siparavhi jaadana."};
        debugmap[PARAGRAPHLEAVE] = {"leaving Paragraph #", "# jan gejuqet jaadana."};
        debugmap[CHANGEDSTOPEXEC] = {"changed stop execution: #->#", "#-># jan sec khilydooet jaadana."};
        debugmap[CHANGEDSTOPEXEC2] = {"changed stop execution(#): #->#", "(#)jan sec khilydooet jaadana. #-># swigana."};
        debugmap[PROCESSING] = {"processing #", "'#'et <kyren> jan khilydoo swigana."};
        debugmap[RECOGNIZEDINTDEF] = {"recognized int default #", "'#'et thek lunakhiemi boolana."};
        debugmap[RECOGNIZEDFLOATDEF] = {"recognized float default #", "'#'et fee lunakhiemi boolana."};
        debugmap[RECOGNIZEDVAL] = {"# regarded as val", "# jan dooet sonaemi boolana."};
        debugmap[RECOGNIZEDFLOAT] = {"recognized # as float", "# jan dooet feeyemi boolana."};
        debugmap[FINISHEDPERROW] = {"finished (g)e/(g)ete for verb #", "# jan guas gejuqxi 'ge/gete'et secana."};
        debugmap[STARTPERROW] = {"starting ~# for Verb #", "~'#'et # jan guas gejuqxi hekana."};
        debugmap[REFEVALTO] = {"ref # evaluated to #.", "# jan iket # jan doo swigana."};
        debugmap[EXECVERB] = {"executing verb #", "# jan guas gejuqet khilana."};
        debugmap[CONDITIONIS] = {"condition + # is #" ,"# jan hekydooet # jan doo swigana." };
        debugmap[CONDITIONRESULT] = {"condition result is #" ,"# jan dooet hek jan meer swigana." };
        debugmap[READFILEIS] = {"read file is '#'", "# jan benien vooset mudydoo swigana."};
        debugmap[RESULTSIZE] = {"result.size is #", "vae jan meeret # jan doo swigana."};
        debugmap[INDEXHIORLO] = {"<n>:i+ix>=# or <0 for i=#", "<n>:i+ix>=# <0 jan ren  'i=#'nyr swigana."};
        debugmap[INDEXHI] = {"<n>:i+off>=# for i=#", "<n>:i+off>=# 'i=#'nyr swigana."};
        debugmap[STARTPROCESSING]= {"start processing #", "# jan khilydooxi hekana."};
        debugmap[CANTUSEMESEC] = {"can't use #mesec on empty result list", "#mesec jan doo wayuq jan meerhi loabmugana."};
        debugmap[RVIS] = {"RV=#", "# nnadoo swigana."};
        debugmap[SETROWINCR] = {"setting row incr to #" , "tutyy jan juryydooet # jan dooxi suunavana."};
        debugmap[SETDEFAULT] = {"setting default to #", "lunakhiet # jan dooxi suunavana."};
        debugmap[CHANGEDSTOPEXECMO] = {"changed stop execution (missing operands): #->9", "#->juban jan sec khilydooet jaadana. theket swimugana."};
        debugmap[NOWCONVERTING] = {"now converting #", "# janet rimoofo jaadana."};
        debugmap[FOUNDNEWMIN] = {"found new min #", "# jan lunykraxi beqavana."};
        debugmap[ADDEDNEWMIN] = {"added new min #", "# jan lunykraet paraigana."};
        debugmap[FOUND] = {"found #", "# jan dooxi beqavana."};
        debugmap[INSERT] = {"insert #", "# jan suunhi siloogana."};
        debugmap[JANXILENGETINDEX]= {"janxi len # - get index #", "'janxi' vaeet  # jan doo swigana. # jan oorut jan thek riachogana."};
        debugmap[BAIKHILMAX]= {"bai/khil#, len= #, maxvar = #", "bai/khil# swigana. vae # seqana. thiidoo jan suoni # seqana."};
    }
    //ctor
}

Log::~Log()
{
    //dtor
}

void Log::setFile(string name) {
    filename = name;
    fstream fs (filename, ios::out | ios::trunc);
    fs.close ();
}

void Log::setLevel(Level lv) {
    level = lv;
}

void Log::setUseConsole(bool uc) {
    useconsole = uc;
}

void Log::setLanguage(int lng) {
    language = lng;
}

void Log::msg(Level lv, string text) {
    if (lv>=level) {
        if (useconsole) {cerr << datum() << " " << levelString(lv) << text << endl;}
        fstream fs;
        fs.open(filename,ios::app);
        if (fs.is_open()){
            fs << datum() << " " << levelString(lv) <<text << "\n";
        }
        fs.close(); //close the file object.
   }
}

void Log::logmsg(Level lv, LogMsg lmsg, string par1, string par2) {
    logmsg4(lv, lmsg, par1, par2, "", "");
}

void Log::logmsg3(Level lv, LogMsg lmsg, string par1, string par2, string par3) {
    logmsg4(lv, lmsg, par1, par2, par3, "");
}

void Log::logmsg4(Level lv, LogMsg lmsg, string par1, string par2, string par3, string par4) {
    string text;
    size_t i = 0;

    if (lv>=level) {
        text = getMessage(lmsg);
        if (par1 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par1);}
        }
        if (par2 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par2);}
        }
        if (par3 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par3);}
        }
        if (par4 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par4);}
        }
        if (useconsole) {cerr << datum() << " " << levelString(lv) << text << endl;}
        fstream fs;
        fs.open(filename,ios::app);
        if (fs.is_open()){
            fs << datum() << " " << levelString(lv) <<text << "\n";
        }
        fs.close(); //close the file object.
   }
}


void Log::logdbg3(LogDbg lmsg, string par1, string par2, string par3) {
    string text;
    size_t i = 0;

    if (DEBUG>=level) {
        text = getDebugMsg(lmsg);
        if (par1 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par1);}
        }
        if (par2 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par2);}
        }
        if (par3 != "") {
            i = text.find_first_of('#');
            if (i != string::npos) {text.replace(i,1,par3);}
        }
        if (useconsole) {cerr << datum() << " " << levelString(DEBUG) << text << endl;}
        fstream fs;
        fs.open(filename,ios::app);
        if (fs.is_open()){
            fs << datum() << " " << levelString(DEBUG) <<text << "\n";
        }
        fs.close(); //close the file object.
   }
}


void Log::plain(string text) {
    fstream fs;
    fs.open(filename,ios::app);
    if (fs.is_open()){
        fs <<text << "\n";
        fs.close();
    }
}

string Log::datum() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];

    sprintf(buffer, "%02d", ltm->tm_mday);
    string result = buffer;
    sprintf(buffer, "%02d", (ltm->tm_mon) + 1);
    result += ".";
    result += buffer;
    sprintf(buffer, "%02d", (ltm->tm_year) - 100);
    result += ".";
    result += buffer;
    sprintf(buffer, "%02d", (ltm->tm_hour));
    result += " ";
    result += buffer;
    sprintf(buffer, "%02d", (ltm->tm_min));
    result += ":";
    result += buffer;
    sprintf(buffer, "%02d", (ltm->tm_sec));
    result += ":";
    result += buffer;

    return result;

}

void Log::debug(string text) {
    msg(DEBUG, text);
}

void Log::debugtime(LogDbg lmsg, string par1, string par2) {
    if (lmsg == TIMEINIT) {
        tstart = chrono::high_resolution_clock::now();
    } else {
        auto tstop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(tstop - tstart);
        logdbg3(lmsg, par1, par2, to_string(duration.count()/1000000.0));
    }
}

string Log::levelString(Level level) {
        string result = "";
        switch(level) {
            case DEBUG:
                result = (language == 0) ? "DEBUG - " : "DIBAG - ";
                break;
            case INFO:
                result = (language == 0) ? "INFO - " : "RIACHO - ";
                break;
            case WARN:
                result = (language == 0) ? "WARN - " : "KHACAV - ";
                break;
            case WARNALL:
                result = (language == 0) ? "WARNALL - " : "CHAN KHACAV - ";
                break;
            case ERROR:
                result = (language == 0) ? "ERROR - " : "KHAC - ";
                break;
            case FATAL:
                result = (language == 0) ? "FATAL - " : "KHACYROO - ";
                break;
            case TOTAL:
                result = (language == 0) ? "TOTAL - " : "CHAN - ";
        }
        return result;
}

Level Log::getLevel() {
    return level;
};

string Log::getMessage(LogMsg lmsg) {
    vector<string> result = messagemap[lmsg];
    return result[language];
}

string Log::getMessage1(LogMsg lmsg, string par) {
    vector<string> result = messagemap[lmsg];
    string text = result[language];
    size_t i = 0;
    if (par != "") {
       i = text.find_first_of('#');
       if (i != string::npos) {text.replace(i,1,par);}
    }
    return text;
}

string Log::getDebugMsg(LogDbg lmsg) {
    vector<string> result = debugmap[lmsg];
    return result[language];
}

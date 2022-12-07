//
// Created by ziqi on 2022/12/3.
//

#include "TM.h"
#include "ErrorCode.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cassert>

static inline int len_digit(int i) {
    if (i >= 1000000000) return 10;
    if (i >= 100000000) return 9;
    if (i >= 10000000) return 8;
    if (i >= 1000000) return 7;
    if (i >= 100000) return 6;
    if (i >= 10000) return 5;
    if (i >= 1000) return 4;
    if (i >= 100) return 3;
    if (i >= 10) return 2;
    return 1;
}


TM::TM() {
    verbose = false;
    ready = false;
    blank = '_';
    N_tape = 0;
    accepted = false;
    isHalt = true;
    step = 0;
    // the initial state is halt, when runTM is called, the flag is set true
}

// trim from left
inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}

// copying versions

inline std::string ltrim_copy(std::string s, const char *t = " \t\n\r\f\v") {
    return ltrim(s, t);
}

inline std::string rtrim_copy(std::string s, const char *t = " \t\n\r\f\v") {
    return rtrim(s, t);
}

inline std::string trim_copy(std::string s, const char *t = " \t\n\r\f\v") {
    return trim(s, t);
}

void TM::Tokenize(std::vector<std::string> &tokens, const std::string &str, const std::string &delimiters) {
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.emplace_back(std::string(str.substr(lastPos, pos - lastPos)));
        lastPos = str.find_first_not_of(delimiters, pos);  //find the last position that's not delimiter
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void TM::eraseWS(std::string &str) {
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
}

void TM::checkBraces(const std::string &str) {
    if (!(*str.begin() == '{' && str.back() == '}'))
        throw TMException(ERROR_SYNTAX, "braces mismatch.");
}

char TM::checkInputDefSymbolValidity(const std::string &symbol) {
    if (symbol.length() != 1) {
        throw TMException(ERROR_SYNTAX, "%s,input symbol has to be one character.");
    }
    char sym = symbol.at(0);
    if (sym == ' ' || sym == ',' || sym == ';' ||
        sym == '{' || sym == '}' || sym == '*' || sym == '_') {
        throw TMException(ERROR_SYNTAX, "input symbol definition error.");
    }
    return sym;
}

char TM::checkTapeDefSymbolValidity(const std::string &symbol) {
    if (symbol.length() != 1) {
        throw TMException(ERROR_SYNTAX, "tape symbol has to be one character.");
    }
    char sym = symbol.at(0);
    if (sym == ' ' || sym == ',' || sym == ';' ||
        sym == '{' || sym == '}' || sym == '*') {
        throw TMException(ERROR_SYNTAX, "tape symbol definition error.");
    }
    return sym;
}

void TM::checkStateStrValidity(const std::string &statStr) {
    std::regex alphas("[0-9a-zA-Z_]+");
    if (!std::regex_match(statStr, alphas)) {
        throw TMException(ERROR_SYNTAX, "state should consists of numbers, alphabets and underline.");
    }
}

void TM::checkStateExistence(const std::string &state) {
    if (statesSet.find(state) == statesSet.end()) {
        throw TMException(ERROR_STATE, "the state doesn't exist.");
    }
}

void TM::checkTapeSymExistence(char sym) {
    if (sym != '*' && sym != '_' && tapeSymSet.find(sym) == tapeSymSet.end())
        throw TMException(ERROR_TAPEMIS, "tape symbol not found.");
}

void TM::checkInputSymExistence(char sym) {
    if (inputSymSet.find(sym) == inputSymSet.end())
        throw TMException(ERROR_INMIS, "input symbol not found.");
}

void TM::checkDirectExistence(char direct) {
    if (direct != 'l' && direct != 'r' && direct != '*')
        throw TMException(ERROR_TAPEMIS, "direction can only be l, r or *.");
}

void TM::parseDefinition(std::string &kind, std::string &body, const std::string &def) {
    std::string::size_type pos_assign = def.find_first_of('=', 0);
    if (pos_assign == std::string::npos) {
        throw TMException(ERROR_SYNTAX, "\'=\'not found in the definition.");
    }
    kind = def.substr(0, pos_assign);
    body = def.substr(pos_assign + 1, def.length());
    if (kind.empty()) {
        throw TMException(ERROR_DEF, "definition receiver is empty.");
    } else if (body.empty()) {
        throw TMException(ERROR_DEF, "definition body is empty.");
    }
}


void TM::parseDefBody(const std::string &bodyStr, char forSym) {
    // check braces
    std::string contentStr = bodyStr;
    checkBraces(contentStr);
    // parse content
    contentStr = trim(contentStr, "{}");

    std::vector<std::string> tokens;
    Tokenize(tokens, contentStr, ", ");
    try {
        for (const std::string &token: tokens) {
            if (forSym == 'Q') {
                checkStateStrValidity(token);
                statesSet.emplace(std::string(token));
            } else if (forSym == 'S') {
                char sym = checkInputDefSymbolValidity(token);
                inputSymSet.emplace(sym);
            } else if (forSym == 'G') {
                char sym = checkTapeDefSymbolValidity(token);
                tapeSymSet.emplace(sym);
            } else if (forSym == 'F') {
                checkStateExistence(token);
                finStatSet.emplace(std::string(token));
            } else {
                throw TMException(ERROR_OTHER, "code error");
            }
        }
    } catch (TMException &e) {
        throw e;
    }
}

void TM::setMode(bool isVerbose) {
    verbose = isVerbose;
}


int TM::loadTM(const std::string &file) {
    std::ifstream fs;
    fs.open(file, std::ios::in);
    try {
        if (!fs.is_open()) {
            throw TMException(ERROR_FILE_NOT_FOUND, ".tm file not found.");
        }
        for (std::string line; getline(fs, line);) {
            // comment
            std::string::size_type semi_pos = line.find_first_of(';', 0);
            if (semi_pos != std::string::npos)
                line.erase(semi_pos);
            line = trim(line);
            if (line.empty())continue;
            if (line.at(0) == '#') {
                // definitions
                eraseWS(line);
                std::string kind;
                std::string body;
                parseDefinition(kind, body, line);
                if (kind == "#Q") {
                    parseDefBody(body, 'Q');
                } else if (kind == "#S") {
                    parseDefBody(body, 'S');
                } else if (kind == "#G") {
                    parseDefBody(body, 'G');
                } else if (kind == "#F") {
                    parseDefBody(body, 'F');
                } else if (kind == "#q0") {
                    checkStateExistence(body);
                    q0 = body;
                } else if (kind == "#B") {
                    if (body != "_")
                        throw TMException(ERROR_DEF, "blank symbol is supposed to be \'_\'.");
                    checkTapeDefSymbolValidity(body);
                    blank = '_';
                } else if (kind == "#N") {
                    try {
                        int num_tape = std::stoi(body, nullptr, 10);
                        if (num_tape <= 0)
                            throw TMException(ERROR_DEF, "number can not be negative.");
                        N_tape = num_tape;
                    } catch (std::exception &e) {
                        throw TMException(ERROR_DEF, "type number convert error.");
                    }
                }
            } else {
                // transfer functions
                std::vector<std::string> tokens;
                Tokenize(tokens, line, " ");
                if (tokens.size() != 5)
                    throw TMException(ERROR_DElTA, "delta size error.");
                std::string fromStat = tokens.at(0), toStat = tokens.at(4);
                std::string oldSymStr = tokens.at(1), newSymStr = tokens.at(2);
                std::string directStr = tokens.at(3);
                checkStateExistence(fromStat);
                checkStateExistence(toStat);
                if (oldSymStr.length() != N_tape || newSymStr.length() != N_tape)
                    throw TMException(ERROR_DElTA, "old(new) symbol doesn't match tape number.");
                if (directStr.length() != N_tape)
                    throw TMException(ERROR_DElTA, "length of direction doesn't match tape number.");
                for (int i = 0; i < N_tape; ++i) {
                    checkTapeSymExistence(oldSymStr[i]);
                    checkTapeSymExistence(newSymStr[i]);
                    checkDirectExistence(directStr[i]);
                }
                Transfer transfer(fromStat, oldSymStr, newSymStr, directStr, toStat);
                deltaMap.emplace(std::make_pair(fromStat, transfer));
            }

        }
    } catch (TMException &e) {
        std::cerr << "Error " << e.code << ": " << e.message << std::endl;
        fs.close();
        return e.code;
    }
    fs.close();
    ready = true;
    return 0;
}

void TM::advanceTM() {
    // use the current state and symbols on the tape, find the matched
    // transfer function and change the content of tapes and move tape
    // heads, change the current state to the next
    if (!ready)
        throw TMException(ERROR_START, "TM is not ready or is halt");

    // check whether tm falls into a final state
    if (finStatSet.find(curStat) != finStatSet.end()) {
        accepted = true;
    }

    ++step;
    auto deltas = deltaMap.equal_range(curStat);
    auto found = deltaMap.end();
    for (auto fun = deltas.first; fun != deltas.second; ++fun) {
        Transfer transfer = fun->second;
        const std::string oldSym = transfer.oldSym;
        int i = 0;
        for (; i < oldSym.length(); ++i) {
            if (oldSym[i] == '*' && tapeHeadSet[i]->second == blank)break;
            if (oldSym[i] != '*' && oldSym[i] != tapeHeadSet[i]->second)break;
        }
        if (i == oldSym.length()) {
            // function matched
            found = fun;
            break;
        }
    }
    // can not find the next move, then halt
    if (found == deltaMap.end()) {
        isHalt = true;
    } else {
        // write the new symbol
        Transfer transfer = found->second;
        const std::string newSym = transfer.newSym;
        for (int i = 0; i < newSym.length(); ++i) {
            if (newSym[i] == '*')continue;
            else tapeHeadSet[i]->second = newSym[i];
        }
        // move the type
        const std::string direct = transfer.direct;
        for (int i = 0; i < direct.length(); ++i) {
            if (direct[i] == '*')continue;
            else if (direct[i] == 'l') {
                if (tapeHeadSet[i] == tapeVec[i].begin()) {
                    tapeVec[i].push_front(std::make_pair(tapeHeadSet[i]->first + 1, blank));
                }
                tapeHeadSet[i]--;
            } else if (direct[i] == 'r') {
                tapeHeadSet[i]++;
                if (tapeHeadSet[i] == tapeVec[i].end()) {
                    // FIXME: may find a better solution
                    tapeHeadSet[i]--;
                    tapeVec[i].push_back(std::make_pair(tapeHeadSet[i]->first + 1, blank));
                    tapeHeadSet[i]++;
                }
            } else   //should never reach here
                assert(false);
        }
        // determine next state
        curStat = transfer.toStat;
    }
}

static void print_spaces(int cnt) {
    for (int i = 0; i < cnt; ++i) {
        printf(" ");
    }
}

void TM::printID() {
    assert(verbose);
    int len_header = 5 + len_digit((int) N_tape) + 1;
    printf("Step"), print_spaces(len_header - 4), printf(": %d\n", step);
    printf("State"), print_spaces(len_header - 5), printf(": %s\n", curStat.c_str());
    for (int i = 0; i < N_tape; ++i) {
        int dlen_curtape = len_digit(i);
        printf("Index%d", i), print_spaces(len_header - (5 + dlen_curtape)), printf(": ");
        auto idx = tapeVec[i].begin();
        while (idx != tapeVec[i].end()) {
            if (idx->second == '_' && idx != tapeHeadSet[i])idx++;
            else break;
        }
        auto first_pos = idx;
        idx = tapeVec[i].end(), idx--;
        while (idx != tapeVec[i].begin()) {
            if (idx->second == '_' && idx != tapeHeadSet[i])idx--;
            else break;
        }
        idx++;
        auto last_pos = idx;
        for (idx = first_pos; idx != last_pos; idx++) {
            printf("%d ", idx->first);
        }
        printf("\n");
        printf("Tape%d", i), print_spaces(len_header - (4 + dlen_curtape)), printf(": ");
        int head_pos = 0;
        bool foundHead = false;
        for (idx = first_pos; idx != last_pos; idx++) {
            printf("%c", idx->second);
            if (idx == tapeHeadSet[i])foundHead = true;
            int d_len = len_digit(idx->first);
            if (!foundHead) head_pos += d_len + 1;
            for (int j = 0; j < d_len; ++j) {
                printf(" ");
            }
        }
        printf("\n");
        printf("Head%d", i), print_spaces(len_header - (4 + dlen_curtape)), printf(": ");
        for (int j = 0; j < head_pos; ++j) printf(" ");
        printf("^\n");
    }
    printf("---------------------------------------------\n");
}

void TM::printResult() {
    assert(isHalt);
    std::string result;
    auto p_valid = tapeVec[0].begin();
    auto q_valid = tapeVec[0].end();
    q_valid--;
    while (p_valid->second == '_')p_valid++;
    while(q_valid->second == '_')q_valid--;
    q_valid++;
    for (; p_valid != q_valid; ++p_valid) {
        result += p_valid->second;
    }
    if (verbose) {
        printf("Result: %s\n", result.c_str());
        printf("==================== END ====================");
    } else {
        printf("%s\n", result.c_str());
    }
}


void TM::initTM(const std::string &input) {
    assert(ready);
    for (int i = 0; i < N_tape; ++i) {
        tapeVec.emplace_back(std::list<std::pair<int, char>>());
        if (i == 0) {
            for (int j = 0; j < input.length(); ++j) {
                char c = input[j];
                if (c == ' ')c = blank;
                tapeVec[i].emplace_back(std::make_pair(j, c));
            }
            if (input.empty())tapeVec[i].emplace_back(std::make_pair(0, blank));
        } else {
            tapeVec[i].emplace_back(std::make_pair(0, blank));
        }
        tapeHeadSet.emplace_back(tapeVec[i].begin());
    }
    curStat = q0;
}

int TM::runTM(const std::string &input) {
    assert(ready && isHalt);
    if (verbose) {
        printf("Input: %s\n", input.c_str());
    }
    for (int i = 0; i < input.length(); ++i) {
        try {
            checkInputSymExistence(input[i]);
        } catch (TMException &e) {
            if (verbose) {
                printf("==================== ERR ====================\n");
                printf("error: '%c' was not declared in the set of input symbols\n", input[i]);
                printf("Input: %s\n", input.c_str());
                for (int j = 0; j < 7 + i; ++j) {
                    printf(" ");
                }
                printf("^\n");
            } else {
                printf("illegal input.\n");
            }

            return ERROR_INPUT;
        }
    }

    initTM(input);
    if (verbose) {
        printf("==================== RUN ====================\n");
    }

    /// let's dance!

    isHalt = false;
    if (verbose)printID();
    while (!isHalt) {
        advanceTM();
        if (verbose && !isHalt)printID();
        // step++
    }
    printResult();
    return 0;
}

TM::Transfer::Transfer(std::string &fromStat, std::string &oldSymStr, std::string &newSymStr, std::string &directStr,
                       std::string &toStat) {
    this->fromStat = fromStat;
    this->toStat = toStat;
    this->oldSym = oldSymStr;
    this->newSym = newSymStr;
    this->direct = directStr;
}
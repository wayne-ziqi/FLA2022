//
// Created by ziqi on 2022/12/3.
//

#ifndef TMP_TM_H
#define TMP_TM_H

/// this TM will serve as both interpreter and simulator

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>

class TM {
private:

    struct Transfer {
        std::string fromStat;
        std::string oldSym;
        std::string newSym;
        std::string direct;
        std::string toStat;

        Transfer(std::string &fromStat, std::string &oldSymStr,
                 std::string &newSymStr, std::string &directStr, std::string &toStat);

        Transfer(const Transfer &transfer) {
            fromStat = transfer.fromStat;
            oldSym = transfer.oldSym;
            newSym = transfer.newSym;
            direct = transfer.direct;
            toStat = transfer.toStat;
        }
    };

    /// basic information
    // mode of the TM
    bool verbose;
    // if no error occurs, the TM is ready to run
    bool ready;
    /// TM definition
    // state set
    std::unordered_set<std::string> statesSet;
    // input symbol set
    std::unordered_set<char> inputSymSet;
    // tape symbol set
    std::unordered_set<char> tapeSymSet;
    // initial state
    std::string q0;
    // blank symbol
    char blank;
    // final state set
    std::unordered_set<std::string> finStatSet;
    // tape amount
    unsigned int N_tape;
    // transfer functions, fromStat-Transfer
    std::unordered_multimap<std::string, Transfer> deltaMap;

    /// instant description
    // record whether the string can be accepted
    bool accepted;
    // record whether the machine is halt
    bool isHalt;
    // step counter
    int step;
    // record all tapes， start from the first
    std::vector<std::list<std::pair<int, char>>> tapeVec;
    // record read-write heads
    std::vector<std::list<std::pair<int, char>>::iterator> tapeHeadSet;
    // current state
    std::string curStat;

    /// MARK: interpreter's functions
    /**
     * tokenize string by delimiters and store the tokens into a vector
     * @param tokens
     * @param str
     * @param delimiters can be set to multiple chars
     */
    static void Tokenize(std::vector<std::string> &tokens, const std::string &str, const std::string &delimiters);

    // use group of trim to erase leading and trailing characters

    /**
     * remove all spaces in the expression
     * @param str
     */
    static void eraseWS(std::string &str);

    /**
     * check braces for definition  body
     * @param string
     * @return
     */
    static void checkBraces(const std::string &str);

    static char checkInputDefSymbolValidity(const std::string &symbol);

    static char checkTapeDefSymbolValidity(const std::string &symbol);

    /**
     * check validity of state string which can only include 0-9 and alphabets
     * @param statStr
     */
    static void checkStateStrValidity(const std::string &statStr);

    /**
     * check whether state exists in the state set
     * @param state
     */
    void checkStateExistence(const std::string &state);

    void checkTapeSymExistence(char sym);

    void checkInputSymExistence(char sym);

    static void checkDirectExistence(char direct);

    static void parseDefinition(std::string &kind, std::string &body, const std::string &def);

    /**
     * parse body surrounded by braces, will check braces
     * if syntax error happens, Error code will be set
     * @param bodyStr
     * @param forSym
     */
    void parseDefBody(const std::string &bodyStr, char forSym);

    /// MARK: simulator's functions

    /**
     * transfer the TM for one step, if no transfer function can be found
     * halt the TM
     */
    void advanceTM();

    void initTM(const std::string &input);

    /**
     * print ID for the current state
     */
    void printID();


    /**
     * print the result recorded on the first tape when TM halts
     * should notice the format diffs between verbose and common mode
     */
    void printResult();


public:
    TM();

    void setMode(bool isVerbose);

    /**
     * load and parse tm from the file
     * @param file
     * @return 0 load successfully, else error code
     */
    int loadTM(const std::string &file);

    /**
     * run tm with the input string
     * @param input
     */
    int runTM(const std::string &input);

};


#endif //TMP_TM_H

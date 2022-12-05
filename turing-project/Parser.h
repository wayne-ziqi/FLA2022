//
// Created by ziqi on 2022/12/3.
//

#ifndef TMP_PARSER_H
#define TMP_PARSER_H
#include <string>
#include <vector>

class Parser {
private:
    std::string file_name;
    std::vector<std::string> arg_list;
    bool verbose;
    bool needHelp;
    bool ready;
    std::string input;

public:
    Parser(int argc, char** argv);

    bool isVerbose() const;

    bool isReady()const;

    bool helpNeeded()const;

    void showHelp();

    void doParse();

    std::string getInput()const;

    std::string getFileName()const;
};


#endif //TMP_PARSER_H

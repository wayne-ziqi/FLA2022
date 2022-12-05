//
// Created by ziqi on 2022/12/3.
//

#include "Parser.h"
#include <iostream>

Parser::Parser(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        arg_list.emplace_back(argv[i]);
    }
    verbose = false;
    ready = false;
    needHelp = false;
}

bool Parser::isVerbose() const {
    return verbose;
}

void Parser::showHelp() {
    if (needHelp) {
        std::cout << "usage: turing [-v|--verbose] [-h|--help] <tm> <input>\n";
        ready = false;
    }
}

bool Parser::helpNeeded()const{
    return needHelp;
}


std::string Parser::getInput() const{
    return input;
}

std::string Parser::getFileName() const{
    return file_name;
}

void Parser::doParse() {
    if (arg_list.size() < 3) {
        std::cerr << "argument count error, place use --help|-h to find more\n";
        return;
    } else {
        int argc = (int) arg_list.size();
        for (int i = 1; i < argc; ++i) {
            if (arg_list[i] == "-h" || arg_list[i] == "--help")
                needHelp = true;
            else if (arg_list[i] == "-v" || arg_list[i] == "--verbose")
                verbose = true;
            if (i == argc - 2)
                file_name = arg_list[i];
            if (i == argc - 1)
                input = arg_list[i];
        }
    }
    ready = true;
}

bool Parser::isReady() const {
    return ready;
}

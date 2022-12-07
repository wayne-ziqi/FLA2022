//
// Created by ziqi on 2022/12/3.
//

#ifndef TMP_ERRORCODE_H
#define TMP_ERRORCODE_H

enum ErrorCode {
    NO_ERROR,               // error free
    ERROR_CMD,              // cmd format error
    ERROR_FILE_NOT_FOUND,   // tm file not found
    ERROR_SYNTAX,           // syntax errors
    ERROR_UNDEF,            // symbol used before defined
    ERROR_DEF,              // definition error
    ERROR_STATE,            // state undefined
    ERROR_DElTA,            // transfer function error
    ERROR_TAPEMIS,          // tape symbol not found
    ERROR_INMIS,            // input symbol not found
    ERROR_START,            // starts in a bad state
    ERROR_INPUT,            // input string error
    ERROR_OTHER,
};

struct TMException : std::exception {
    ErrorCode code;
    const char *message;

    explicit TMException(ErrorCode code = NO_ERROR, const char *message = "no error") : code(code), message(message) {}
};

#endif //TMP_ERRORCODE_H

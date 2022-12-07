#include "Parser.h"
#include "ErrorCode.h"
#include "TM.h"

using namespace std;

int main(int argc, char *argv[]) {
    Parser parser(argc, argv);
    parser.doParse();
    parser.showHelp();
    if (parser.helpNeeded())return 0;
    if (parser.isReady()) {
        // tm begins
        TM turingMachine;
        turingMachine.setMode(parser.isVerbose());
        int load = turingMachine.loadTM(parser.getFileName());
        if (load != NO_ERROR) return load;
        return turingMachine.runTM(parser.getInput());
    } else {
        return ERROR_CMD;
    }
}
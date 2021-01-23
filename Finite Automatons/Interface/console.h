#ifndef CONSOLE_H
#define CONSOLE_H
#include "../Regular expressions to auto/regExpressionParser.h"
#include "../Auto to regular expression/autoParser.h"
#include "../Nondeterministic/nondetermAuto.h"
#include "../Deterministic/determAuto.h"

///Class Console provides the user with the opportunity to use all the operations in the other classes.
class Console
{
    std::string command;
    NondetermAutomaton *nondetAuto;
    DetermAutomaton *detAuto;

    ///enters a command
    void enterCommand();

    ///checks the command and calls the requested function
    void doCommand();

    ///builds an automaton
    void buildAuto();

    ///enter a deterministic automaton
    static DetermAutomaton *enterDeterm();

    ///enter a nondeterministic automaton
    static NondetermAutomaton *enterNondeterm();

    ///parses a regular expression to a nondeterministic automaton
    void parseRegExpression();

    ///checks whether the word is from the language of the auto
    void checkWord();

    ///parses a deterministic automaton to a regular expression
    void parseAuto();

    ///builds a deterministic automaton
    void buildDeterm();

    ///builds a nondeterministic automaton
    void buildNondeterm();

    ///prints out information about an auto
    void show();

    ///checks whether the word from the stream is from the language of the deterministic auto
    void wordFromStream();

    ///ask the user with which automaton to work(determ or nondeterm)
    bool choiceIsDetertm();

    ///prints information about the commands
    void help();

    ///performs an operation with an auto
    void operation();

    void operationHelper(std::function<DetermAutomaton(const DetermAutomaton &, const DetermAutomaton &)>,
                         std::function<NondetermAutomaton(const NondetermAutomaton &, const NondetermAutomaton &)>);

    ///makes a complementation of an auto
    void complementation();

    ///concatenates two autos
    void concatenation();

public:
    Console();

    ~Console();

    ///allows the user to enter commands until "exit" is entered
    void myMain();
};

#endif

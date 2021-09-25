#ifndef _REG_EXPRESSION_PARSER_H_
#define _REG_EXPRESSION_PARSER_H_

#include <string>
#include <stack>
#include <optional>
#include <functional>
#include "../Nondeterministic/nondetermAuto.h"
///Class Parser parses a regular expression to an automaton.
class Parser
{
    std::string expression;
    std::vector<std::pair<std::optional<NondetermAutomaton>, std::optional<char>>> parsedString;
    std::stack<char> operations;
    std::stack<NondetermAutomaton> autos;

    ///checks whether the char is operation
    bool isOperation(const char &symbol);

    //checks whether there is an auto in the pair
    bool isAuto(const std::pair<std::optional<NondetermAutomaton>, std::optional<char>> &);

    ///gets an operation char and returns the operation's priority
    int priority(const char &symbol);

    ///returns the substring before the next operation char or the end of the string
    std::string getString(const std::string &, int &);

    ///gets two automatons and returns the result of the operation on the automatons
    NondetermAutomaton calculate(NondetermAutomaton &first, NondetermAutomaton &second, const char &oper);

    ///returns automaton with transformed with Kleene star
    NondetermAutomaton calculateKleeneStar(NondetermAutomaton &first);

public:
    Parser() = delete;
    
    Parser(std::string &str);

    ///parses the expression to a vector of automatons and operations
    std::vector<std::pair<std::optional<NondetermAutomaton>, std::optional<char>>> parse();

    ///uses parse() and then calculates the final result
    NondetermAutomaton solve();
};
#endif
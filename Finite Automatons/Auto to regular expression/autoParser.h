#ifndef AUTO_PARSER_H
#define AUTO_PARSER_H
#include "../Deterministic/determAuto.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <optional>

///Class AutoParser parses a deterministic automaton to a regular expression
class AutoParser
{
    DetermAutomaton myAuto;
    std::deque<std::pair<std::string, const DetermAutomaton::Node *>> currentStates;
    std::set<const DetermAutomaton::Node *> prevStates;
    std::map<std::string, std::vector<std::string>> statesLanguage;
    std::string regExpression;

    ///parses a state to a regular expression of the language of the state
    void parseState(const std::vector<std::pair<const char, const DetermAutomaton::Node *>> &nexts, const DetermAutomaton::Node *currentNode, const std::string &word);

    ///changes the languages of the next states
    void changeNexts(const DetermAutomaton::Node *currentNode, std::set<std::string> &);

public:
    AutoParser() = delete;

    AutoParser(const DetermAutomaton &);

    ///parses the auto to a reg expression
    void parse();

    ///combines a list of regular expressions in an union
    static void compress(const std::vector<std::string> &words, std::string &currentStateLanguage);

    std::string getRegExpression();

    ///prints the regular expression
    friend std::ostream &operator<<(std::ostream &out, const AutoParser &myParser);
};

#endif

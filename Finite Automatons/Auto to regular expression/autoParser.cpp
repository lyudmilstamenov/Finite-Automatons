#include "autoParser.h"
#include <functional>

AutoParser::AutoParser(const DetermAutomaton &_myAuto) : myAuto{_myAuto}
{
    currentStates.push_back(std::pair("", myAuto.nodes[*myAuto.begin()]));
}

void AutoParser::parse()
{
    while (!currentStates.empty())
    {
        std::pair<std::string, const DetermAutomaton::Node *> myPair = currentStates.front();
        currentStates.pop_front();
        const std::string word = myPair.first;
        const DetermAutomaton::Node *currentNode = myPair.second;
        std::vector<std::pair<const char, const DetermAutomaton::Node *>> nexts = currentNode->getNext();
        parseState(nexts, currentNode, word);
    }
    for (int i = 0; i < myAuto.finalNodes.size(); ++i)
    {
        if (myAuto.finalNodes[i] == myAuto.first)
        {
            statesLanguage[myAuto.finalNodes[i]->getNodeName()].push_back("@");
        }
        std::vector<std::string> words = statesLanguage[myAuto.finalNodes[i]->getNodeName()];
        std::string currentStateLanguage;
        compress(words, currentStateLanguage);
        if (i == 0)
        {
            regExpression += currentStateLanguage;
        }
        else
        {
            regExpression += "+" + currentStateLanguage;
        }
    }
}

void AutoParser::compress(const std::vector<std::string> &words, std::string &expression)
{
    if (words.empty())
    {
        return;
    }
    expression += "(" + words[0];

    for (int k = 1; k < words.size(); ++k)
    {
        expression += "+" + words[k];
    }
    expression += ")";
}

void AutoParser::parseState(const std::vector<std::pair<const char, const DetermAutomaton::Node *>> &nexts, const DetermAutomaton::Node *currentNode, const std::string &word)
{
    for (int i = 0; i < nexts.size(); ++i)
    {
        std::vector<std::string> path = myAuto.hasPathFromTo(nexts[i].second, currentNode);

        if (prevStates.count(nexts[i].second) == 0)
        {
            prevStates.insert(nexts[i].second);
            currentStates.push_back(std::pair(word + nexts[i].first, nexts[i].second));
            statesLanguage[nexts[i].second->getNodeName()].push_back(word + nexts[i].first);
        }
        else if (!path.empty())
        {
            std::string fullExpresion;
            if (path.size() == 1)
            {
                fullExpresion = path[0];
            }
            else
            {
                compress(path, fullExpresion);
            }
            std::set<std::string> excludedNodes;
            statesLanguage[currentNode->getNodeName()].push_back("(" + word + "(" + nexts[i].first + fullExpresion + ")*)");
            excludedNodes.insert(currentNode->getNodeName());
            excludedNodes.insert(nexts[i].second->getNodeName());
            changeNexts(currentNode, excludedNodes);
            statesLanguage[nexts[i].second->getNodeName()].push_back("(" + word + "(" + nexts[i].first + fullExpresion + ")*" + nexts[i].first + ")");
            changeNexts(nexts[i].second, excludedNodes);
        }
        else
        {
            statesLanguage[nexts[i].second->getNodeName()].push_back(word + nexts[i].first);
        }
    }
}

std::string AutoParser::getRegExpression()
{
    return regExpression;
}

std::ostream &operator<<(std::ostream &out, const AutoParser &myParser)
{
    out << "L= " << myParser.regExpression << "\n";
    return out;
}

void AutoParser::changeNexts(const DetermAutomaton::Node *currentNode, std::set<std::string> &excludedNodes)
{
    std::vector<std::pair<const char, const DetermAutomaton::Node *>> nexts = currentNode->getNext();
    for (int i = 0; i < nexts.size(); ++i)
    {
        if (excludedNodes.count(nexts[i].second->getNodeName()) == 0)
        {
            excludedNodes.insert(nexts[i].second->getNodeName());
            statesLanguage[nexts[i].second->getNodeName()].push_back(statesLanguage[currentNode->getNodeName()].back() + nexts[i].first);
            changeNexts(nexts[i].second, excludedNodes);
        }
    }
}
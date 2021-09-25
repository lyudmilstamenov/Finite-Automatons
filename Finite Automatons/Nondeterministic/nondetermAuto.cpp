#ifndef NONDETERMINISTIC_AUTOMATON_CPP
#define NONDETERMINISTIC_AUTOMATON_CPP
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <time.h>
#include <utility>
#include <exception>
#include <cstring>
#include "nondetermAuto.h"

void NondetermAutomaton::Node::copy(const NondetermAutomaton::Node &other)
{
    this->nodeName = other.nodeName;
    this->next.clear();
}

NondetermAutomaton::Node::Node(const Node &other)
{
    copy(other);
}

typename NondetermAutomaton::Node &NondetermAutomaton::Node::operator=(const Node &other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

std::string NondetermAutomaton::Node::getNodeName() const
{
    return nodeName;
}

std::vector<std::pair<std::optional<char>, const NondetermAutomaton::Node *>> NondetermAutomaton::Node::getNext() const
{
    return next;
}

void NondetermAutomaton::Node::add(std::optional<char> transitionChar, const Node *node)
{
    if (node->isNodeFromTransitionList(this->hasSymbolTransition(transitionChar)))
    {
        throw std::overflow_error("There is already the same transition.\n");
    }
    else
    {
        if (transitionChar)
        {
            next.push_back(std::pair<std::optional<char>, const Node *>(transitionChar, node));
        }
        else
        {
            next.push_back(std::pair<std::optional<char>, const Node *>(std::nullopt, node));
        }
    }
}

bool NondetermAutomaton::Node::isNodeFromTransitionList(std::vector<Node const *> nodes) const
{
    for (int i = 0; i < nodes.size(); ++i)
    {
        if (this == nodes[i])
        {
            return true;
        }
    }
    return false;
}

void NondetermAutomaton::Node::pushBackToName(const std::string &symbols)
{
    nodeName.append(symbols);
}

typename std::vector<NondetermAutomaton::Node const *> NondetermAutomaton::Node::hasSymbolTransition(std::optional<char> transitionChar) const
{
    std::vector<NondetermAutomaton::Node const *> nextState;

    for (int i = 0; i < next.size(); ++i)
    {
        if (equalChars(next[i].first, transitionChar))
        {
            nextState.push_back(next[i].second);
        }
    }
    return nextState;
}

 

void NondetermAutomaton::AutoIterator::copy(const AutoIterator &other)
{
    current.clear();
    for (int i = 0; i < other.current.size(); ++i)
    {
        current.push_back(other.current[i]);
    }
}

NondetermAutomaton::AutoIterator::AutoIterator(NondetermAutomaton::Node *node) { current.push_back(node); }

NondetermAutomaton::AutoIterator::AutoIterator(std::vector<Node *> nodes)
{
    for (int i = 0; i < nodes.size(); ++i)
    {
        current.push_back(nodes[i]);
    }
}

NondetermAutomaton::AutoIterator &NondetermAutomaton::AutoIterator::operator=(const AutoIterator &other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

bool NondetermAutomaton::AutoIterator::operator!=(const AutoIterator &other)
{
    if (current.empty() && other.current.empty())
    {
        return false;
    }
    if (current.size() != other.current.size())
    {
        return true;
    }
    bool same = true;
    for (int i = 0; i < current.size(); ++i)
    {
        bool found = false;
        for (int k = 0; k < other.current.size(); ++k)
        {
            if (current[i] == other.current[k])
                found = true;
        }
        if (!found)
            same = false;
    }
    return !same;
}

bool NondetermAutomaton::AutoIterator::operator==(const AutoIterator &other)
{
    for (int i = 0; i < current.size(); ++i)
    {

        for (int k = 0; k < other.current.size(); ++k)
        {
            if (current[i] == other.current[k])
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> NondetermAutomaton::AutoIterator::operator*()
{
    std::vector<std::string> nodesNames;
    for (int i = 0; i < current.size(); ++i)
    {
        nodesNames.push_back(current[i]->getNodeName());
    }
    return nodesNames;
}

NondetermAutomaton::AutoIterator &NondetermAutomaton::AutoIterator::transition(const char transitionChar, const NondetermAutomaton &automaton)
{
    if (this->current.empty())
    {
        throw std::out_of_range("nullptr");
    }

    std::vector<const NondetermAutomaton::Node *> newNodes;

    for (int i = 0; i < current.size(); ++i)
    {
        transitionHelper(transitionChar, automaton, newNodes, i);
    }

    if (newNodes.empty())
    {
        current.clear();
    }
    else
    {
        current = newNodes;
    }
    return *this;
}

void NondetermAutomaton::AutoIterator::transitionHelper(const char &transitionChar, const NondetermAutomaton &automaton,
                                                        std::vector<const NondetermAutomaton::Node *> &newNodes, const int &i)
{
    std::optional<char> symbol(transitionChar);
    std::vector<const NondetermAutomaton::Node *> currentVector = current[i]->hasSymbolTransition(symbol);
    for (int i = 0; i < currentVector.size(); ++i)
    {
        bool repetition = false;
        for (int k = 0; k < newNodes.size(); ++k)
        {
            if ((currentVector[i]->getNodeName()).compare(newNodes[k]->getNodeName()) == 0)
            {
                repetition = true;
            }
        }
        if (!repetition)
        {
            newNodes.push_back(currentVector[i]);
        }
    }
}

NondetermAutomaton::AutoIterator &NondetermAutomaton::AutoIterator ::epsilonTransition(const NondetermAutomaton &automaton)
{

    if (this->current.empty())
    {
        throw std::out_of_range("nullptr");
    }
    std::vector<const NondetermAutomaton::Node *> newNodes;

    for (int i = 0; i < current.size(); ++i)
    {
        std::vector<const NondetermAutomaton::Node *> currentVector = current[i]->hasSymbolTransition(std::nullopt);
        newNodes.push_back(current[i]);
        newNodes.insert(newNodes.end(), currentVector.begin(), currentVector.end());
    }

    current.clear();

    if (newNodes.empty())
    {
        return *this;
    }

    for (int i = 0; i < newNodes.size(); ++i)
    {
        epsilonTransitionHelper(i, newNodes);
    }
    return *this;
}

void NondetermAutomaton::AutoIterator ::epsilonTransitionHelper(const int &i, std::vector<const NondetermAutomaton::Node *> &newNodes)
{
    bool repetition = false;
    for (int k = 0; k < current.size(); ++k)
    {
        if ((current[k]->getNodeName()).compare(newNodes[i]->getNodeName()) == 0)
        {
            repetition = true;
        }
    }
    if (!repetition)
    {
        current.push_back(newNodes[i]);
    }
}

bool NondetermAutomaton::AutoIterator::hasEpsilonTransition()
{

    for (int i = 0; i < current.size(); ++i)
    {
        if (!current[i]->hasSymbolTransition(std::nullopt).empty())
        {
            return true;
        }
    }
    return false;
}


void NondetermAutomaton::deleteNodes()
{
    first = nullptr;
    for (auto i = nodes.begin();
         i != nodes.end(); i++)
    {
        delete (i->second);
    }
    nodes.clear();
    finalNodes.clear();
}

void NondetermAutomaton::copyTransitions(const NondetermAutomaton &other)
{
    int size = other.nodes.size();
    int previousSize = nodes.size();
    std::vector<std::string> listOfNodes;
    std::unordered_map<std::string, int> listOfIndices;
    int counter = 0;

    for (auto i = other.nodes.begin();
         i != other.nodes.end(); i++)
    {
        Node *node = new Node(i->second->getNodeName());
        this->nodes[node->getNodeName()] = node;
        listOfNodes.push_back(i->second->getNodeName());
        listOfIndices[i->second->getNodeName()] = counter++;
    }

    std::vector<std::optional<char>> **adjacencyMatrix = new std::vector<std::optional<char>> *[size];
    for (int i = 0; i < size; ++i)
    {
        std::vector<std::optional<char>> *arr = new std::vector<std::optional<char>>[size];
        adjacencyMatrix[i] = arr;
    }

    other.buildAdjencyMatrix(adjacencyMatrix, listOfNodes, listOfIndices, (previousSize > 0));

    for (int rows = 0; rows < size; ++rows)
    {
        copyTransitionsHelper(size, adjacencyMatrix, rows, listOfNodes);
    }
    for (int i = 0; i < size; ++i)
    {
        delete adjacencyMatrix[i];
    }
    delete adjacencyMatrix;
}

void NondetermAutomaton::copyTransitionsHelper(const int &size, std::vector<std::optional<char>> **adjacencyMatrix,
                                               const int &rows, const std::vector<std::string> &listOfNodes)
{
    for (int cols = 0; cols < size; ++cols)
    {
        for (int counter = 0; counter < adjacencyMatrix[rows][cols].size(); ++counter)
        {
            try
            {
                nodes[listOfNodes[rows]]->add(adjacencyMatrix[rows][cols][counter], nodes[listOfNodes[cols]]);
            }
            catch (std::overflow_error error)
            {
                std::cout << error.what();
            }
        }
    }
}

void NondetermAutomaton::copyFinals(const NondetermAutomaton &other)
{
    for (int i = 0; i < other.finalNodes.size(); ++i)
    {
        finalNodes.push_back(nodes[other.finalNodes[i]->getNodeName()]);
    }
}

void NondetermAutomaton::copy(const NondetermAutomaton &other)
{
    deleteNodes();
    copyTransitions(other);
    first = nodes[other.first->getNodeName()];
    copyFinals(other);
}

void NondetermAutomaton::buildAdjencyMatrix(std::vector<std::optional<char>> **adjacencyMatrix, std::vector<std::string> listOfNodes,
                                            std::unordered_map<std::string, int> listOfIndices, bool isNew) const
{
    int size = nodes.size();
    for (int rows = 0; rows < size; ++rows)
    {
        Node *s = (this->nodes.lower_bound(listOfNodes[rows]))->second;
        std::vector<std::pair<std::optional<char>, const NondetermAutomaton::Node *>> nextStates;
        if (!isNew)
        {
            nextStates = (this->nodes.at(listOfNodes[rows]))->getNext();
        }
        else
        {
            nextStates = s->getNext();
        }

        for (int k = 0; k < nextStates.size(); ++k)
        {
            adjacencyMatrix[rows][listOfIndices[nextStates[k].second->getNodeName()]].push_back(nextStates[k].first);
        }
    }
}

NondetermAutomaton::NondetermAutomaton() : first{nullptr} {}

NondetermAutomaton::~NondetermAutomaton()
{
    deleteNodes();
}

NondetermAutomaton::NondetermAutomaton(const NondetermAutomaton &other)
{
    copy(other);
}

NondetermAutomaton &NondetermAutomaton::operator=(const NondetermAutomaton &other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

NondetermAutomaton::NondetermAutomaton(const std::string &word)
{

    if (word.compare("@") == 0)
    {
        buildEpsilonAuto();
        return;
    }
    std::string nodeName = word;
    Node *node = new Node(nodeName + std::to_string(0));
    Node *prevNode;
    first = node;
    nodes[nodeName + std::to_string(0)] = first;

    for (int i = 0; i < word.size(); ++i)
    {
        prevNode = node;
        node = new Node(nodeName + std::to_string(i + 1));
        try
        {
            prevNode->add(std::optional(word[i]), node);
        }
        catch (std::overflow_error error)
        {
            std::cout << error.what();
        }

        nodes[nodeName + std::to_string(i + 1)] = node;
    }
    finalNodes.push_back(node);
}

void NondetermAutomaton::buildEpsilonAuto()
{
    srand(time(NULL));
    std::string name = "epsilonNode" + std::string(std::to_string(rand() % 100));
    Node *node = new Node(name);
    first = node;
    nodes[name] = first;
    finalNodes.push_back(node);
}

void NondetermAutomaton::addFirstNode(const std::string &nodeName)
{
    if (!isEmpty())
    {
        throw std::invalid_argument("There is already a first element!");
    }

    this->first = new Node(nodeName);
    nodes[first->getNodeName()] = first;
}

void NondetermAutomaton::addTransition(const std::string &nodeStartName, const char &transitionChar, const std::string &nodeEndName)
{
    if (isEmpty())
    {
        throw std::bad_alloc();
    }

    if (nodes.count(nodeStartName) == 0)
    {
        throw std::invalid_argument("No starting node");
    }
    if (nodes.count(nodeEndName) == 0)
    {
        nodes[nodeEndName] = new Node(nodeEndName);
    }
    try
    {
        if (transitionChar == '@')
        {
            nodes[nodeStartName]->add(std::nullopt, nodes[nodeEndName]);
        }
        else
        {
            nodes[nodeStartName]->add(std::optional(transitionChar), nodes[nodeEndName]);
        }
    }
    catch (std::overflow_error warning)
    {
        std::cout << warning.what();
    }
}

void NondetermAutomaton::addFinals(const std::string &nodeName)
{

    if (nodes.count(nodeName) == 0)
    {
        throw std::invalid_argument("No such final state");
    }
    finalNodes.push_back(nodes[nodeName]);
}

bool NondetermAutomaton::isEmpty() const
{
    return !first;
}

bool NondetermAutomaton::isWordFromAuto(const std::string &transitionWord)
{
    int i = 0;
    NondetermAutomaton::AutoIterator it = this->begin();
    NondetermAutomaton::AutoIterator out = this->out();
    NondetermAutomaton::AutoIterator prev;
    while (i < transitionWord.size() && it != out)
    {
        if (it.hasEpsilonTransition() && it != prev)
        {
            prev = it;
            it.epsilonTransition(*this);
        }
        else
        {
            it.transition(transitionWord[i++], *this);
            prev = out;
        }
    }
    NondetermAutomaton::AutoIterator finals = this->end();

    if (i == transitionWord.size() && (it == finals))
    {
        return true;
    }

    if (!(it != out))
    {
        return false;
    }
    prev = it;
    i = 0;
    while (i < nodes.size() && it != out && !(it == finals))
    {
        it.epsilonTransition(*this);

        if (!(prev != it))
        {
            it = out;
        }
        prev = it;
        ++i;
    }

    return (it == finals);
}

typename NondetermAutomaton::AutoIterator NondetermAutomaton::begin() const
{
    if (isEmpty())
    {
        throw std::out_of_range("Empty automaton");
    }
    return AutoIterator(first);
}

typename NondetermAutomaton::AutoIterator NondetermAutomaton::end() const
{
    return AutoIterator(finalNodes);
}

typename NondetermAutomaton::AutoIterator NondetermAutomaton::out() const
{
    return AutoIterator();
}

NondetermAutomaton NondetermAutomaton::complementation() const
{
    NondetermAutomaton newAutomaton(*this);
    newAutomaton.swapFinalStates();
    return newAutomaton;
}

void NondetermAutomaton::swapFinalStates()
{
    std::vector<Node *> newFinalNodes;

    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        if (!isFinalState(i->second))
        {
            newFinalNodes.push_back(i->second);
        }
    }
    finalNodes.clear();
    finalNodes = newFinalNodes;
}

bool NondetermAutomaton::isFinalState(const NondetermAutomaton::Node *myNode)
{
    for (int i = 0; i < finalNodes.size(); ++i)
    {
        if (myNode == finalNodes[i])
        {
            return true;
        }
    }
    return false;
}

void NondetermAutomaton::visualize(std::ostream &out) const
{
    out << "Automat\n";
    out << "First: " << first->getNodeName() << "\n";
    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        visualizeHelper(out, i);
    }
    out << "Finals: ";
    for (int i = 0; i < finalNodes.size(); ++i)
    {
        out << finalNodes[i]->getNodeName() << " ";
    }
    out << std::endl;
}

void NondetermAutomaton::visualizeHelper(std::ostream &out, std::map<std::string, NondetermAutomaton::Node *>::const_iterator i) const
{
    out << i->second->getNodeName() << " ";
    std::vector<std::pair<std::optional<char>, const NondetermAutomaton::Node *>> next = i->second->getNext();
    for (int k = 0; k < next.size(); ++k)
    {
        out << "(";
        if (!next[k].first)
        {
            out << "epsilon";
        }
        else
        {
            out << *next[k].first;
        }
        out << ", " << next[k].second->getNodeName() << ") ";
    }
    out << " \n";
}

bool NondetermAutomaton::equalChars(std::optional<char> first, std::optional<char> second)
{
    return (!first && !second) || ((first && second) && (*first == *second));
}

NondetermAutomaton NondetermAutomaton::unions(const NondetermAutomaton &firstAuto, const NondetermAutomaton &secondAuto)
{
    NondetermAutomaton myAuto(firstAuto);
    NondetermAutomaton secondAutoCopy(secondAuto);
    NondetermAutomaton::makeUnique(firstAuto, secondAutoCopy);
    myAuto.copyTransitions(secondAutoCopy);

    myAuto.copyFinals(secondAutoCopy);

    std::string firstStateName = "unique";
    while (!myAuto.isUnique(firstStateName))
    {
        firstStateName.push_back('1');
    }
    myAuto.first = new Node(firstStateName);
    myAuto.nodes[myAuto.first->getNodeName()] = myAuto.first;
    myAuto.addTransition(firstStateName, '@', firstAuto.first->getNodeName());
    myAuto.addTransition(firstStateName, '@', secondAutoCopy.first->getNodeName());

    return myAuto;
}

bool NondetermAutomaton::isUnique(const std::string &uniqueName) const
{
    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        if (uniqueName.compare(i->second->getNodeName()) == 0)
        {
            return false;
        }
    }
    return true;
}

NondetermAutomaton NondetermAutomaton::concatenation(const NondetermAutomaton &firstAuto, NondetermAutomaton &secondAuto)
{
    NondetermAutomaton myAuto;
    myAuto.copyTransitions(firstAuto);
    myAuto.first = myAuto.nodes[firstAuto.first->getNodeName()];

    NondetermAutomaton::makeUnique(firstAuto, secondAuto );
    myAuto.copyTransitions(secondAuto);

    for (int i = 0; i < firstAuto.finalNodes.size(); ++i)
    {
        myAuto.addTransition(firstAuto.finalNodes[i]->getNodeName(), '@', secondAuto .first->getNodeName());
    }
    myAuto.copyFinals(secondAuto );

    return myAuto;
}

void NondetermAutomaton::makeUnique(const NondetermAutomaton &firstAuto, NondetermAutomaton &secondAuto)
{
    if (areUnique(firstAuto, secondAuto))
    {
        return;
    }
    srand(time(NULL));
    std::map<std::string, Node *> newNodes;
    for (auto s = secondAuto.nodes.begin(); s != secondAuto.nodes.end(); s++)
    {
        const std::string addToName = std::to_string(rand() % 100) + s->second->getNodeName();
        s->second->pushBackToName(addToName);
        newNodes[s->second->getNodeName()] = s->second;
    }
    secondAuto.nodes = newNodes;
}

bool NondetermAutomaton::areUnique(const NondetermAutomaton &firstAuto, NondetermAutomaton &secondAuto)
{
    for (auto f = firstAuto.nodes.begin(); f != firstAuto.nodes.end(); f++)
    {
        for (auto s = secondAuto.nodes.begin(); s != secondAuto.nodes.end(); s++)
        {
            if (f->first.compare(s->first) == 0)
            {
                return false;
            }
        }
    }
    return true;
}

NondetermAutomaton NondetermAutomaton::intersection(const NondetermAutomaton &firstAuto, const NondetermAutomaton &secondAuto)
{
    NondetermAutomaton firstAutoCopy(firstAuto);
    NondetermAutomaton secondAutoCopy(secondAuto);
    NondetermAutomaton myAuto(intersectionHelper(firstAutoCopy, secondAutoCopy));

    for (int i = 0; i < firstAutoCopy.finalNodes.size(); ++i)
    {
        for (int k = 0; k < secondAutoCopy.finalNodes.size(); ++k)
        {
            std::string name = NondetermAutomaton::constructPair(firstAutoCopy.finalNodes[i], secondAutoCopy.finalNodes[k]);
            myAuto.finalNodes.push_back(myAuto.nodes[name]);
        }
    }
    return myAuto;
}

NondetermAutomaton NondetermAutomaton::intersectionHelper(NondetermAutomaton &firstAuto, NondetermAutomaton &secondAuto)
{

    NondetermAutomaton myAuto;
    std::vector<std::string> listOfNodes;
    NondetermAutomaton::makeUnique(firstAuto, secondAuto);
    for (auto firstI = firstAuto.nodes.begin(); firstI != firstAuto.nodes.end(); firstI++)
    {
        for (auto secondI = secondAuto.nodes.begin(); secondI != secondAuto.nodes.end(); secondI++)
        {
            std::string name = NondetermAutomaton::constructPair(firstI->second, secondI->second);
            Node *node = new Node(name);
            myAuto.nodes[name] = node;
            listOfNodes.push_back(name);
        }
    }
    myAuto.first = myAuto.nodes[NondetermAutomaton::constructPair(firstAuto.first, secondAuto.first)];

    int matrixSize = secondAuto.nodes.size() * firstAuto.nodes.size();
    std::vector< std::optional<char>> **adjencyMatrix = new std::vector<std::optional<char> > *[matrixSize];

    for (int i = 0; i < matrixSize; ++i)
    {
        adjencyMatrix[i] = new std::vector< std::optional<char>>[matrixSize];
    }

    myAuto.buildAdjencyMatrixIntersection(adjencyMatrix, firstAuto, secondAuto);

    for (int row = 0; row < matrixSize; ++row)
    {
        for (int col = 0; col < matrixSize; ++col)
        {
            addAllTransitionChars(myAuto.nodes[listOfNodes[row]], adjencyMatrix[row][col], myAuto.nodes[listOfNodes[col]]);
        }
    }
    for (int i = 0; i < matrixSize; ++i)
    {
        delete adjencyMatrix[i];
    }
    delete adjencyMatrix;
    return myAuto;
}

std::string NondetermAutomaton::constructPair(const NondetermAutomaton::Node *firstNode, const NondetermAutomaton::Node *secondNode)
{
    return "(" + firstNode->getNodeName() + "," + secondNode->getNodeName() + ")";
}

void NondetermAutomaton::buildAdjencyMatrixIntersection(std::vector<std::optional<char>> **adjencyMatrix, const NondetermAutomaton &firstAuto, const NondetermAutomaton &secondAuto)
{
    int sizeFirstAutoNodes = firstAuto.nodes.size();
    int sizeSecondAutoNodes = secondAuto.nodes.size();
    std::vector<std::optional<char>> **matrixFirstAuto, **matrixSecondAuto;
    matrixFirstAuto = new std::vector<std::optional<char>> *[sizeFirstAutoNodes];
    matrixSecondAuto = new std::vector<std::optional<char>> *[sizeSecondAutoNodes];

    std::vector<std::string> listOfNodes1;
    std::unordered_map<std::string, int> listOfIndices1;
    std::vector<std::string> listOfNodes2;
    std::unordered_map<std::string, int> listOfIndices2;

    int counter = 0;
    for (auto i = firstAuto.nodes.begin(); i != firstAuto.nodes.end(); i++)
    {
        matrixFirstAuto[counter] = new std::vector<std::optional<char>>[sizeFirstAutoNodes];
        listOfNodes1.push_back(i->second->getNodeName());
        listOfIndices1[i->second->getNodeName()] = counter++;
    }
    counter = 0;
    for (auto i = secondAuto.nodes.begin(); i != secondAuto.nodes.end(); i++)
    {
        matrixSecondAuto[counter] = new std::vector<std::optional<char>>[sizeSecondAutoNodes];
        listOfNodes2.push_back(i->second->getNodeName());
        listOfIndices2[i->second->getNodeName()] = counter++;
    }

    firstAuto.buildAdjencyMatrix(matrixFirstAuto, listOfNodes1, listOfIndices1, false);
    secondAuto.buildAdjencyMatrix(matrixSecondAuto, listOfNodes2, listOfIndices2, false);

    for (int rowFirst = 0; rowFirst < sizeFirstAutoNodes; ++rowFirst)
    {
        for (int rowSecond = 0; rowSecond < sizeSecondAutoNodes; ++rowSecond)
        {
            buildAdjencyMatrixIntersectionHelper(adjencyMatrix,
                                                 firstAuto, secondAuto,
                                                 sizeFirstAutoNodes, sizeSecondAutoNodes, listOfNodes1, listOfNodes2,
                                                 rowFirst, rowSecond);
        }
    }

    for (int i = 0; i < sizeFirstAutoNodes; ++i)
    {
        delete matrixFirstAuto[i];
    }
    for (int i = 0; i < sizeSecondAutoNodes; ++i)
    {
        delete matrixSecondAuto[i];
    }
    delete matrixFirstAuto;
    delete matrixSecondAuto;
}

void NondetermAutomaton::buildAdjencyMatrixIntersectionHelper(std::vector<std::optional<char>> **adjencyMatrix,
                                                              const NondetermAutomaton &firstAuto, const NondetermAutomaton &secondAuto,
                                                              const int &sizeFirstAutoNodes, const int &sizeSecondAutoNodes, const std::vector<std::string> &listOfNodes1, const std::vector<std::string> &listOfNodes2,
                                                              const int &rowFirst, const int &rowSecond)
{
    for (int colFirst = 0; colFirst < sizeFirstAutoNodes; ++colFirst)
    {
        for (int colSecond = 0; colSecond < sizeSecondAutoNodes; ++colSecond)
        {
            std::vector<std::optional<char>> transitionFirst = firstAuto.reach(listOfNodes1[rowFirst], listOfNodes1[colFirst]);
            std::vector<std::optional<char>> transitionSecond = secondAuto.reach(listOfNodes2[rowSecond], listOfNodes2[colSecond]);
            NondetermAutomaton::pushBackEqualChars(adjencyMatrix[rowFirst * sizeSecondAutoNodes + rowSecond]
                                                                [colFirst * sizeSecondAutoNodes + colSecond],
                                                   transitionFirst, transitionSecond);
        }
    }
}

void NondetermAutomaton::addAllTransitionChars(Node *startNode, std::vector<std::optional<char>> &transitionChars, const Node *destinationNode)
{
    for (int i = 0; i < transitionChars.size(); ++i)
    {
        try
        {
            startNode->add(transitionChars[i], destinationNode);
        }
        catch (std::overflow_error error)
        {
            std::cout << error.what();
        }
    }
}

void NondetermAutomaton::pushBackEqualChars(std::vector<std::optional<char>> &destination, const std::vector<std::optional<char>> &str1, const std::vector<std::optional<char>> &str2)
{
    for (int i = 0; i < str1.size(); ++i)
    {
        for (int k = 0; k < str2.size(); ++k)
        {
            if (!str1[i] || !str2[k] || *str1[i] == *str2[k])
            {
                destination.push_back(str1[i]);
            }
        }
    }
}

std::vector<std::optional<char>> NondetermAutomaton::reach(const std::string &index1, const std::string &index2) const
{

    std::vector<std::optional<char>> transitionChars;
    std::vector<std::pair<std::optional<char>, const NondetermAutomaton::Node *>> nexts = nodes.at(index1)->getNext();
    for (int i = 0; i < nexts.size(); ++i)
    {
        if (nodes.at(index2)->getNodeName().compare(nexts[i].second->getNodeName()) == 0)
        {
            transitionChars.push_back(nexts[i].first);
        }
    }
    return transitionChars;
}

NondetermAutomaton NondetermAutomaton::kleeneStar(const NondetermAutomaton &firstAuto)
{
    NondetermAutomaton myAuto(firstAuto);
    for (int i = 0; i < myAuto.finalNodes.size(); ++i)
    {
        try
        {
            myAuto.finalNodes[i]->add(std::nullopt, myAuto.first);
        }
        catch (std::overflow_error error)
        {
            std::cout << error.what();
        }
    }
    if (!myAuto.isFinalState(myAuto.first))
    {
        myAuto.finalNodes.push_back(myAuto.first);
    }

    return myAuto;
}

std::ostream &operator<<(std::ostream &out, const NondetermAutomaton &myAuto)
{
    myAuto.visualize(out);
    return out;
}

#endif

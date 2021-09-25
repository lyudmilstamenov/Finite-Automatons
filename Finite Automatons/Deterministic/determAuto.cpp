#ifndef DETERMINISTIC_AUTOMATON_CPP
#define DETERMINISTIC_AUTOMATON_CPP
#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <exception>
#include <cstring>
#include "determAuto.h"

void DetermAutomaton::Node::copy(const DetermAutomaton::Node &other)
{
    this->nodeName = other.nodeName;
    this->next.clear();
}

void DetermAutomaton::deleteNodes()
{
    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        delete i->second;
    }
}

DetermAutomaton::Node::Node(const Node &other)
{
    copy(other);
}

typename DetermAutomaton::Node &DetermAutomaton::Node::operator=(const Node &other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

std::string DetermAutomaton::Node::getNodeName() const
{
    return nodeName;
}

std::vector<std::pair<const char, const DetermAutomaton::Node *>> DetermAutomaton::Node::getNext() const
{
    return next;
}

void DetermAutomaton::Node::pushBackToName(const char &symbol)
{
    nodeName.push_back(symbol);
}

void DetermAutomaton::Node::add(const char &transitionChar, const Node *node)
{
    if (hasSymbolTransition(transitionChar) == node)
    {
        throw std::overflow_error("There is already the same transition.\n");
    }
    else if (hasSymbolTransition(transitionChar))
    {
        throw std::overflow_error("Nondeterministic actions.\n");
    }
    else
    {
        next.push_back(std::pair<const char, const Node *>(transitionChar, node));
    }
}

typename DetermAutomaton::Node const *DetermAutomaton::Node::hasSymbolTransition(const char &transitionChar) const
{
    for (int i = 0; i < next.size(); ++i)
    {
        if (next[i].first == transitionChar)
        {
            return next[i].second;
        }
    }
    return nullptr;
}

bool DetermAutomaton::AutoIterator::operator!=(const AutoIterator &other)
{
    return current != other.current;
}

bool DetermAutomaton::AutoIterator::operator==(const AutoIterator &other)
{
    return !(*this != other);
}

std::string DetermAutomaton::AutoIterator::operator*()
{
    if (!current)
    {
        throw std::out_of_range("There is no node!\n");
    }
    return current->getNodeName();
}

DetermAutomaton::AutoIterator &DetermAutomaton::AutoIterator::transition(const char &transitionChar, const DetermAutomaton &automaton)
{
    if (!this->current)
    {
        throw std::out_of_range("nullptr");
    }
    const DetermAutomaton::Node *newNode = current->hasSymbolTransition(transitionChar);

    if (!newNode)
    {
        current = nullptr;
    }
    else
    {
        current = newNode;
    }
    return *this;
}

void DetermAutomaton::copy(const DetermAutomaton &other)
{
    deleteNodes();
    copyTransitions(other);
    first = nodes[other.first->getNodeName()];
    copyFinals(other);
}

void DetermAutomaton::copyTransitions(const DetermAutomaton &other)
{
    int size = other.nodes.size();
    int prevSize = nodes.size();
    std::vector<std::string> listOfNodes;
    std::unordered_map<std::string, int> listOfIndices;
    std::string **adjacencyMatrix = new std::string *[size];
    for (int i = 0; i < size; ++i)
    {
        std::string *arr = new std::string[size];
        adjacencyMatrix[i] = arr;
    }

    int counter = 0;
    for (auto i = other.nodes.begin(); i != other.nodes.end(); i++)
    {
        Node *node = new Node(i->second->getNodeName());
        this->nodes[node->getNodeName()] = node;
        listOfNodes.push_back(i->second->getNodeName());
        listOfIndices[i->second->getNodeName()] = counter++;
    }
    other.buildAdjencyMatrix(adjacencyMatrix, listOfNodes, listOfIndices, (prevSize > 0));

    for (int rows = 0; rows < size; ++rows)
    {
        copyTransitionsHelper(rows, size, adjacencyMatrix, listOfNodes, other);
    }
}

void DetermAutomaton::copyTransitionsHelper(const int &rows, const int &size, std::string **adjacencyMatrix, const std::vector<std::string> &listOfNodes, const DetermAutomaton &other)
{
    for (int cols = 0; cols < size; ++cols)
    {
        for (int counter = 0; counter < adjacencyMatrix[rows][cols].size(); ++counter)
        {
            try
            {
                nodes[listOfNodes[rows]]->add(adjacencyMatrix[rows][cols].back(), nodes[listOfNodes[cols]]);
            }
            catch (std::overflow_error error)
            {
                std::cout << error.what();
            }
        }
    }
}

void DetermAutomaton::copyFinals(const DetermAutomaton &other)
{
    for (int i = 0; i < other.finalNodes.size(); ++i)
    {
        finalNodes.push_back(nodes[other.finalNodes[i]->getNodeName()]);
    }
}

void DetermAutomaton::buildAdjencyMatrix(std::string **adjacencyMatrix, std::vector<std::string> listOfNodes,
                                         std::unordered_map<std::string, int> listOfIndices, bool isNew) const
{
    int size = nodes.size();
    for (int rows = 0; rows < size; ++rows)
    {
        Node *s = (this->nodes.lower_bound(listOfNodes[rows]))->second;
        std::vector<std::pair<const char, const DetermAutomaton::Node *>> nextStates;
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

DetermAutomaton::DetermAutomaton() : first{nullptr} {}

DetermAutomaton::~DetermAutomaton()
{
    deleteNodes();
}

DetermAutomaton::DetermAutomaton(const DetermAutomaton &other)
{
    copy(other);
}

DetermAutomaton &DetermAutomaton::operator=(const DetermAutomaton &other)
{
    if (this != &other)
    {
        copy(other);
    }
    return *this;
}

void DetermAutomaton::addFirstNode(const std::string &nodeName)
{
    if (!isEmpty())
    {
        throw std::invalid_argument("There is already a first element!");
    }

    this->first = new Node(nodeName);
    nodes[nodeName] = first;
}

void DetermAutomaton::addTransition(const std::string &nodeStartName, const char &transitionChar, const std::string &nodeEndName)
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
        nodes[nodeStartName]->add(transitionChar, nodes[nodeEndName]);
    }
    catch (std::overflow_error warning)
    {
        std::cout << warning.what();
    }
}

void DetermAutomaton::addFinals(const std::string &nodeName)
{
    if (nodes.count(nodeName) == 0)
    {
        throw std::invalid_argument("No such final state");
    }
    finalNodes.push_back(nodes[nodeName]);
}

bool DetermAutomaton::isEmpty() const
{
    return !first;
}

bool DetermAutomaton::isWordFromAuto(const std::string &transitionWord)
{
    int i = 0;
    DetermAutomaton::AutoIterator it = this->begin();
    while (i < transitionWord.size() && it != this->out())
    {
        it.transition(transitionWord[i++], *this);
    }

    return (i == transitionWord.size() && it.isMember(this->end()));
}

typename DetermAutomaton::AutoIterator DetermAutomaton::begin() const
{
    if (isEmpty())
    {
        throw std::out_of_range("Empty automaton");
    }
    return AutoIterator(first);
}

typename std::vector<DetermAutomaton::AutoIterator> DetermAutomaton::end() const
{
    std::vector<DetermAutomaton::AutoIterator> finalStates;
    for (int i = 0; i < finalNodes.size(); ++i)
    {
        finalStates.push_back(DetermAutomaton::AutoIterator(finalNodes[i]));
    }
    return finalStates;
}

typename DetermAutomaton::AutoIterator DetermAutomaton::out() const
{
    return AutoIterator(nullptr);
}

bool DetermAutomaton::AutoIterator::isMember(const std::vector<AutoIterator> &iterators)
{
    for (int i = 0; i < iterators.size(); ++i)
    {
        if ((*this) == iterators[i])
        {
            return true;
        }
    }
    return false;
}

const DetermAutomaton::Node *DetermAutomaton::AutoIterator::getCurrent() { return current; }

std::istream &DetermAutomaton::getWordFromStream(std::istream &in) const
{
    DetermAutomaton::AutoIterator it = this->begin();
    DetermAutomaton::AutoIterator out = this->out();
    std::vector<AutoIterator> finals = this->end();
    char symbol;
    in >> symbol;
    it.transition(symbol, *this);
    while (in && it != out && (!it.isMember(finals)))
    {
        in >> symbol;
        it.transition(symbol, *this);
    }

    if (it.isMember(finals))
    {
        std::cout << "The word is from the automaton. ";
    }
    else
    {
        std::cout << "The word is not from the automaton. ";
    }
    return in;
}

DetermAutomaton DetermAutomaton::complementation()
{
    DetermAutomaton newAutomaton(*this);
    newAutomaton.swapFinalStates();
    return newAutomaton;
}

void DetermAutomaton::swapFinalStates()
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

bool DetermAutomaton::isFinalState(const DetermAutomaton::Node *myNode) const
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

void DetermAutomaton::visualize(std::ostream &out) const
{
    out << "Automat\n";
    out << "First: " << first->getNodeName() << "\n";
    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        out << i->second->getNodeName() << " ";
        std::vector<std::pair<const char, const DetermAutomaton::Node *>> next = i->second->getNext();
        for (int k = 0; k < next.size(); ++k)
        {
            out << "(" << next[k].first << ", " << next[k].second->getNodeName() << ") ";
        }
        out << " \n";
    }
    out << "Finals: ";
    for (int i = 0; i < finalNodes.size(); ++i)
    {
        out << finalNodes[i]->getNodeName() << " ";
    }
    out << std::endl;
}

DetermAutomaton DetermAutomaton::intersection(const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto)
{
    DetermAutomaton myAuto(intersectionUnionHelper(firstAuto, secondAuto));

    for (int i = 0; i < firstAuto.finalNodes.size(); ++i)
    {
        for (int k = 0; k < secondAuto.finalNodes.size(); ++k)
        {
            std::string name = DetermAutomaton::constructPair(firstAuto.finalNodes[i], secondAuto.finalNodes[k]);
            myAuto.finalNodes.push_back(myAuto.nodes[name]);
        }
    }
    return myAuto;
}

std::string DetermAutomaton::constructPair(const DetermAutomaton::Node *firstNode, const DetermAutomaton::Node *secondNode)
{
    return "(" + firstNode->getNodeName() + "," + secondNode->getNodeName() + ")";
}

void DetermAutomaton::buildAdjencyMatrixIntersection(std::string **adjencyMatrix, const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto)
{
    int sizeFirstAutoNodes = firstAuto.nodes.size();
    int sizeSecondAutoNodes = secondAuto.nodes.size();
    std::string **matrixFirstAuto, **matrixSecondAuto;
    matrixFirstAuto = new std::string *[sizeFirstAutoNodes];
    matrixSecondAuto = new std::string *[sizeSecondAutoNodes];

    std::vector<std::string> listOfNodes1;
    std::unordered_map<std::string, int> listOfIndices1;
    std::vector<std::string> listOfNodes2;
    std::unordered_map<std::string, int> listOfIndices2;

    int counter = 0;
    for (auto i = firstAuto.nodes.begin(); i != firstAuto.nodes.end(); i++)
    {
        matrixFirstAuto[counter] = new std::string[sizeFirstAutoNodes];
        listOfNodes1.push_back(i->second->getNodeName());
        listOfIndices1[i->second->getNodeName()] = counter++;
    }
    counter = 0;
    for (auto i = secondAuto.nodes.begin(); i != secondAuto.nodes.end(); i++)
    {
        matrixSecondAuto[counter] = new std::string[sizeSecondAutoNodes];
        listOfNodes2.push_back(i->second->getNodeName());
        listOfIndices2[i->second->getNodeName()] = counter++;
    }

    firstAuto.buildAdjencyMatrix(matrixFirstAuto, listOfNodes1, listOfIndices1, false);
    secondAuto.buildAdjencyMatrix(matrixSecondAuto, listOfNodes2, listOfIndices2, false);

    for (int rowFirst = 0; rowFirst < sizeFirstAutoNodes; ++rowFirst)
    {
        for (int rowSecond = 0; rowSecond < sizeSecondAutoNodes; ++rowSecond)
        {
            buildAdjencyMatrixIntersectionHelper(adjencyMatrix, firstAuto, secondAuto,
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

void DetermAutomaton::buildAdjencyMatrixIntersectionHelper(std::string **adjencyMatrix, const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto,
                                                           const int &sizeFirstAutoNodes, const int &sizeSecondAutoNodes, const std::vector<std::string> &listOfNodes1, const std::vector<std::string> &listOfNodes2,
                                                           const int &rowFirst, const int &rowSecond)
{
    for (int colFirst = 0; colFirst < sizeFirstAutoNodes; ++colFirst)
    {
        for (int colSecond = 0; colSecond < sizeSecondAutoNodes; ++colSecond)
        {
            std::string transitionFirst = firstAuto.reach(listOfNodes1[rowFirst], listOfNodes1[colFirst]);
            std::string transitionSecond = secondAuto.reach(listOfNodes2[rowSecond], listOfNodes2[colSecond]);
            DetermAutomaton::pushBackEqualChars(adjencyMatrix[rowFirst * sizeSecondAutoNodes + rowSecond]
                                                             [colFirst * sizeSecondAutoNodes + colSecond],
                                                transitionFirst, transitionSecond);
        }
    }
}

std::string DetermAutomaton::reach(const std::string &index1, const std::string &index2) const
{
    std::string transitionChars;
    std::vector<std::pair<const char, const DetermAutomaton::Node *>> nexts = nodes.at(index1)->getNext();
    for (int i = 0; i < nexts.size(); ++i)
    {
        if (nodes.at(index2)->getNodeName().compare(nexts[i].second->getNodeName()) == 0)
        {
            transitionChars.push_back(nexts[i].first);
        }
    }
    return transitionChars;
}

void DetermAutomaton::pushBackEqualChars(std::string &destination, const std::string &str1, const std::string &str2)
{
    for (int i = 0; i < str1.size(); ++i)
    {
        for (int k = 0; k < str2.size(); ++k)
        {
            if (str1[i] == str2[k])
            {
                destination.push_back(str1[i]);
            }
        }
    }
}

void DetermAutomaton::addAllTransitionChars(Node *startNode, std::string &transitionChars, const Node *destinationNode)
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

DetermAutomaton DetermAutomaton::unions(const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto)
{
    DetermAutomaton myAuto(intersectionUnionHelper(firstAuto, secondAuto));

    for (int i = 0; i < firstAuto.finalNodes.size(); ++i)
    {
        for (auto k = secondAuto.nodes.begin(); k != secondAuto.nodes.end(); k++)
        {
            std::string name = DetermAutomaton::constructPair(firstAuto.finalNodes[i], k->second);
            myAuto.finalNodes.push_back(myAuto.nodes[name]);
        }
    }
    for (auto i = firstAuto.nodes.begin(); i != firstAuto.nodes.end(); i++)
    {
        if (!firstAuto.isFinalState(i->second))
        {
            for (int k = 0; k < secondAuto.finalNodes.size(); ++k)
            {
                std::string name = DetermAutomaton::constructPair(i->second, secondAuto.finalNodes[k]);
                myAuto.finalNodes.push_back(myAuto.nodes[name]);
            }
        }
    }

    return myAuto;
}

DetermAutomaton DetermAutomaton::intersectionUnionHelper(const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto)
{

    DetermAutomaton myAuto;
    std::vector<std::string> listOfNodes;
    for (auto firstI = firstAuto.nodes.begin(); firstI != firstAuto.nodes.end(); firstI++)
    {
        for (auto secondI = secondAuto.nodes.begin(); secondI != secondAuto.nodes.end(); secondI++)
        {
            std::string name = DetermAutomaton::constructPair(firstI->second, secondI->second);
            Node *node = new Node(name);
            myAuto.nodes[name] = node;
            listOfNodes.push_back(name);
        }
    }
    myAuto.first = myAuto.nodes[DetermAutomaton::constructPair(firstAuto.first, secondAuto.first)];

    int matrixSize = secondAuto.nodes.size() * firstAuto.nodes.size();
    std::string **adjencyMatrix = new std::string *[matrixSize];

    for (int i = 0; i < matrixSize; ++i)
    {
        adjencyMatrix[i] = new std::string[matrixSize];
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

bool DetermAutomaton::isUnique(const std::string &uniqueName) const
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

std::vector<std::string> DetermAutomaton::hasPathFromTo(const Node *startingNode, const Node *goalNode)
{
    return hasPathFromToHelper(startingNode, goalNode, 0);
}

std::vector<std::string> DetermAutomaton::hasPathFromToHelper(const Node *startingNode, const Node *goalNode, int counter)
{
    if (startingNode == goalNode)
    {
        return std::vector<std::string>(1, "");
    }
    if (counter > nodes.size() || startingNode->getNext().empty())
    {
        return std::vector<std::string>();
    }

    std::vector<std::pair<const char, const DetermAutomaton::Node *>> nexts = startingNode->getNext();
    std::vector<std::string> newPaths;
    for (int i = 0; i < nexts.size(); ++i)
    {
        std::vector<std::string> currentPath = hasPathFromToHelper(nexts[i].second, goalNode, counter + 1);
        for (int k = 0; k < currentPath.size(); ++k)
        {
            newPaths.push_back(nexts[i].first + currentPath[k]);
        }
    }
    return newPaths;
}

std::ostream &operator<<(std::ostream &out, const DetermAutomaton &myAuto)
{
    myAuto.visualize(out);
    return out;
}
#endif

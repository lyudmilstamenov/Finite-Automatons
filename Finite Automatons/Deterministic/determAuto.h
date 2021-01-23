#ifndef DETERMINISTIC_AUTOMATON_H
#define DETERMINISTIC_AUTOMATON_H
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>
#include <fstream>

///class DetermAutomaton allows the user to work with a deterministic automaton
class DetermAutomaton
{ 
    struct Node
    {
    private:
        std::string nodeName;
        std::vector<std::pair<const char, const Node *>> next;
        void copy(const Node &);

    public:
        Node(const std::string &name) : nodeName{name} {}

        ~Node() = default;

        Node &operator=(const Node &);

        Node(const Node &);

        ///adds a transition from this node to another node
        void add(const char &, const Node *);

        std::string getNodeName() const;

        ///returns the node to which there is a transition from this node with the char
        const Node *hasSymbolTransition(const char &) const;

        ///returns the list of transitions from this node
        std::vector<std::pair<const char, const Node *>> getNext() const;

        ///changes the name of the node by appending a string to the end
        void pushBackToName(const char &);
    };

    Node *first;
    std::vector<Node *> finalNodes;
    std::map<std::string, Node *> nodes;

    void deleteNodes();

    void copy(const DetermAutomaton &);

    void copyTransitions(const DetermAutomaton &);

    void copyFinals(const DetermAutomaton &other);

    bool isFinalState(const Node *) const;

    ///builds an adjency matrix of the nodes of the automaton
    void buildAdjencyMatrix(std::string **, std::vector<std::string> listOfNodes,
                            std::unordered_map<std::string, int> listOfIndices, bool isNew) const;

    ///returns a string constructed with the names of both nodes
    static std::string constructPair(const DetermAutomaton::Node *, const DetermAutomaton::Node *);

    static void buildAdjencyMatrixIntersection(std::string **, const DetermAutomaton &, const DetermAutomaton &);

    static void buildAdjencyMatrixIntersectionHelper(std::string **adjencyMatrix,
                                                     const DetermAutomaton &firstAuto, const DetermAutomaton &secondAuto,
                                                     const int &sizeFirstAutoNodes, const int &sizeSecondAutoNodes,
                                                     const std::vector<std::string> &listOfNodes1, const std::vector<std::string> &listOfNodes2,
                                                     const int &rowFirst, const int &rowSecond);

    static DetermAutomaton intersectionUnionHelper(const DetermAutomaton &, const DetermAutomaton &);

    std::vector<std::string> hasPathFromToHelper(const Node *, const Node *, int counter);

    void copyTransitionsHelper(const int &rows, const int &size, std::string **adjacencyMatrix,
                               const std::vector<std::string> &listOfNodes, const DetermAutomaton &other);

    ///builds a vector of chars with the chars which repeats in the const vectors
    static void pushBackEqualChars(std::string &, const std::string &, const std::string &);

    ///builds a vector of chars with the chars which repeats in the const vectors
    static void addAllTransitionChars(Node *startNode, std::string &chars, const Node *destination);

    ///returns the transition chars from node with index1 to node with index2
    std::string reach(const std::string &index1, const std::string &index2) const;

public:
    class AutoIterator
    {
        const DetermAutomaton::Node *current;

    public:
        AutoIterator(const DetermAutomaton::Node *node = nullptr) : current{node} {}

        ///checks whether the current nodes of both iterators are not equal
        bool operator!=(const AutoIterator &other);

        ///checks whether the current nodes of both iterators are equal
        bool operator==(const AutoIterator &other);

        ///returns the name of the current node
        std::string operator*();

        ///makes a transition with the char in the automaton
        AutoIterator &transition(const char &transitionChar, const DetermAutomaton &automaton);

        ///checks whether the iterator is from the vector of iterators
        bool isMember(const std::vector<AutoIterator> &);

        const DetermAutomaton::Node *getCurrent();
    };

    DetermAutomaton();

    ~DetermAutomaton();

    DetermAutomaton(const DetermAutomaton &);

    DetermAutomaton &operator=(const DetermAutomaton &);

    void addFirstNode(const std::string &);

    void addTransition(const std::string &, const char &, const std::string &);

    void addFinals(const std::string &);

    bool isEmpty() const;

    bool isWordFromAuto(const std::string &);

    ///returns an iterator from the starting node
    AutoIterator begin() const;

    ///returns iterators from the final nodes
    std::vector<AutoIterator> end() const;

    ///returns an iterator with no node
    AutoIterator out() const;

    ///gets the word from the stream which is from the language of the automaton
    std::istream &getWordFromStream(std::istream &) const;

    ///checks whether the string is unique to the names of the nodes
    bool isUnique(const std::string &) const;

    ///returns an auto the language of which is a complementation to the language of this auto
    DetermAutomaton complementation();

    ///makes the final states nonfinal and makes the nonfinal states final
    void swapFinalStates();

    ///prints information about the auto
    void visualize(std::ostream &) const;

    ///returns an auto the language of which is an intersection of the languages of the two autos
    static DetermAutomaton intersection(const DetermAutomaton &, const DetermAutomaton &);

    ///returns an auto the language of which is an union of the languages of the two autos
    static DetermAutomaton unions(const DetermAutomaton &, const DetermAutomaton &);

    ///returns the path from the first to the second node
    std::vector<std::string> hasPathFromTo(const Node *, const Node *);

    friend class AutoParser;
};

std::ostream &operator<<(std::ostream &out, const DetermAutomaton &myAuto);

#endif

#ifndef NONDETERMINISTIC_AUTOMATON_H
#define NONDETERMINISTIC_AUTOMATON_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <utility>
#include <fstream>
#include <optional>

///class NondetermAutomaton allows the user to work with a nondeterministic automaton
class NondetermAutomaton
{
    struct Node
    {
    private:
        std::string nodeName;
        std::vector<std::pair<std::optional<char>, const Node *>> next;
        void copy(const Node &);

    public:
        Node(const std::string &name) : nodeName{name} {}

        ~Node() = default;

        Node &operator=(const Node &);

        Node(const Node &);

        ///adds a transition from this node to another node
        void add(std::optional<char>, const Node *);

        std::string getNodeName() const;

        ///returns the list of nodes to which there is a transition from this node with the char
        std::vector<NondetermAutomaton::Node const *> hasSymbolTransition(std::optional<char>) const;

        ///returns the list of transitions from this node
        std::vector<std::pair<std::optional<char>, const Node *>> getNext() const;

        ///checks whether the node is from the list of nodes
        bool isNodeFromTransitionList(std::vector<Node const *>) const;

        ///changes the name of the node by appending a string to the end
        void pushBackToName(const std::string &);
    };

    Node *first;
    std::vector<Node *> finalNodes;
    std::map<std::string, Node *> nodes;

    void deleteNodes();

    void copy(const NondetermAutomaton &);

    void copyTransitions(const NondetermAutomaton &);

    void copyFinals(const NondetermAutomaton &other);

    bool isFinalState(const Node *);

    ///builds an adjency matrix of the nodes of the automaton
    void buildAdjencyMatrix(std::vector<std::optional<char>> **adjacencyMatrix, std::vector<std::string> listOfNodes,
                            const std::unordered_map<std::string, int>, bool b) const;

    static void buildAdjencyMatrixIntersection(std::vector<std::optional<char>> **, const NondetermAutomaton &, const NondetermAutomaton &);

    ///builds an automaton with language = {epsilon}
    void buildEpsilonAuto();

    ///returns a string constructed with the names of both nodes
    static std::string constructPair(const NondetermAutomaton::Node *, const NondetermAutomaton::Node *);

    ///adds transitions from the first node to the second with the chars
    static void addAllTransitionChars(Node *startNode, std::vector<std::optional<char>> &chars, const Node *destination);

    ///builds a vector of chars with the chars which repeats in the const vectors
    static void pushBackEqualChars(std::vector<std::optional<char>> &,
                                   const std::vector<std::optional<char>> &, const std::vector<std::optional<char>> &);

    ///returns the transition chars from node with index1 to node with index2
    std::vector<std::optional<char>> reach(const std::string &index1, const std::string &index2) const;

    static NondetermAutomaton intersectionHelper(NondetermAutomaton &, NondetermAutomaton &);

    void copyTransitionsHelper(const int &size, std::vector<std::optional<char>> **adjacencyMatrix,
                               const int &rows, const std::vector<std::string> &listOfNodes);

    void visualizeHelper(std::ostream &out, std::map<std::string, NondetermAutomaton::Node *>::const_iterator i) const;

    static void buildAdjencyMatrixIntersectionHelper(std::vector<std::optional<char>> **adjencyMatrix,
                                                     const NondetermAutomaton &firstAuto, const NondetermAutomaton &secondAuto,
                                                     const int &sizeFirstAutoNodes, const int &sizeSecondAutoNodes,
                                                     const std::vector<std::string> &listOfNodes1, const std::vector<std::string> &listOfNodes2,
                                                     const int &rowFirst, const int &rowSecond);

public:
    ///class AutoIterator iterates the automaton
    class AutoIterator
    {
        std::vector<const NondetermAutomaton::Node *> current;

        void copy(const AutoIterator &other);

        void transitionHelper(const char &transitionChar, const NondetermAutomaton &automaton,
                              std::vector<const NondetermAutomaton::Node *> &newNodes, const int &i);

        void epsilonTransitionHelper(const int &i, std::vector<const NondetermAutomaton::Node *> &newNodes);

    public:
        AutoIterator() = default;

        AutoIterator(NondetermAutomaton::Node *node);

        AutoIterator(std::vector<NondetermAutomaton::Node *>);

        AutoIterator &operator=(const AutoIterator &other);

        ///checks whether the current nodes of the two iterators are not the same
        bool operator!=(const AutoIterator &other);

        ///checks whether there is an equal node in both iterators
        bool operator==(const AutoIterator &other);

        ///returns the list of names of the current nodes
        std::vector<std::string> operator*();

        ///makes a transition with the char in the automaton
        AutoIterator &transition(const char transitionChar, const NondetermAutomaton &automaton);

        ///makes an epsilon transition in the automaton
        AutoIterator &epsilonTransition(const NondetermAutomaton &automaton);

        ///checks whether there is an epsilon transition from the current nodes
        bool hasEpsilonTransition();
    };

    NondetermAutomaton();

    ~NondetermAutomaton();

    NondetermAutomaton(const NondetermAutomaton &);

    NondetermAutomaton &operator=(const NondetermAutomaton &);

    NondetermAutomaton(const std::string &);

    void addFirstNode(const std::string &);

    void addTransition(const std::string &, const char &, const std::string &);

    void addFinals(const std::string &);

    bool isEmpty() const;

    bool isWordFromAuto(const std::string &);

    ///returns an iterator from the starting node
    AutoIterator begin() const;

    ///returns an iterator of the final nodes
    AutoIterator end() const;

    ///returns an iterator with no nodes
    AutoIterator out() const;

    ///returns an auto the language of which is a complementation to the language of this auto
    NondetermAutomaton complementation() const;

    ///makes the final states nonfinal and makes the nonfinal states final
    void swapFinalStates();

    ///prints information about the auto
    void visualize(std::ostream &) const;

    ///checks whether the chars are equal
    static bool equalChars(std::optional<char>, std::optional<char>);

    ///checks whether the string is unique to the names of the nodes
    bool isUnique(const std::string &) const;

    ///returns an auto the language of which is an union of the languages of the two autos
    static NondetermAutomaton unions(const NondetermAutomaton &, const NondetermAutomaton &);

    ///returns an auto the language of which is a concatenation of the languages of the two autos
    static NondetermAutomaton concatenation(const NondetermAutomaton &, NondetermAutomaton &);

    ///returns an auto the language of which is an intersection of the languages of the two autos
    static NondetermAutomaton intersection(const NondetermAutomaton &, const NondetermAutomaton &);

    ///returns an auto the language of which is transformation of the languages of the two autos with  Kleene's star
    static NondetermAutomaton kleeneStar(const NondetermAutomaton &);

    ///makes the names of the nodes of the second auto unique to the names of the nodes of the first one
    static void makeUnique(const NondetermAutomaton &, NondetermAutomaton &);

    ///checks whether the names of the nodes of the two autos are unique to each other
    static bool areUnique(const NondetermAutomaton &firstAuto, NondetermAutomaton &secondAuto);
};

std::ostream &operator<<(std::ostream &out, const NondetermAutomaton &);
#endif

#ifndef CONSOLE_CPP
#define CONSOLE_CPP
#include "../Interface/console.h"
#include "../Interface/tests.h"

Console::Console() : detAuto{nullptr}, nondetAuto{nullptr} {}

Console::~Console()
{
    delete detAuto;
    delete nondetAuto;
}

void Console::enterCommand()
{
    std::cout << "Enter a command: ";
    std::cin >> command;
}

void Console::doCommand()
{
    if (command.compare("auto") == 0)
    {
        buildAuto();
        return;
    }
    if (command.compare("test") == 0)
    {
        test();
        return;
    }
    if (command.compare("parseExpr") == 0)
    {
        parseRegExpression();
        return;
    }
    if (command.compare("parseAuto") == 0)
    {
        parseAuto();
        return;
    }
    if (command.compare("check") == 0)
    {
        checkWord();
        return;
    }
    if (command.compare("show") == 0)
    {
        show();
        return;
    }
    if (command.compare("help") == 0)
    {
        help();
        return;
    }
    if (command.compare("stream") == 0)
    {
        wordFromStream();
        return;
    }
    if (command.compare("operation") == 0)
    {
        operation();
        return;
    }
    std::cout << "Wrong command\n";
}

void Console::buildAuto()
{
    bool choice;
    try
    {
        choice = choiceIsDetertm();
    }
    catch (std::invalid_argument a)
    {
        std::cout << a.what() << std::endl;
        return;
    }
    if (choice)
    {
        buildDeterm();
        return;
    }
    buildNondeterm();
}

void Console::parseRegExpression()
{
    std::cout << "Regular expression: ";
    std::string regExpr;
    std::cin >> regExpr;
    Parser myRegParser(regExpr);
    myRegParser.parse();
    delete nondetAuto;
    nondetAuto = new NondetermAutomaton(myRegParser.solve());
    std::cout << "expression parsed!\n";
}

void Console::checkWord()
{
    std::string word;
    std::cout << "word: ";
    std::cin >> word;
    bool choice;
    try
    {
        choice = choiceIsDetertm();
    }
    catch (std::invalid_argument a)
    {
        std::cout << a.what() << std::endl;
        return;
    }
    if (choice)
    {
        if ((detAuto) && (detAuto->isWordFromAuto(word)))
        {
            std::cout << "yes\n";
        }
        else
        {
            std::cout << "no\n";
        }
        return;
    }

    if ((nondetAuto) && (nondetAuto->isWordFromAuto(word)))
    {
        std::cout << "yes\n";
    }
    else
    {
        std::cout << "no\n";
    }
}

void Console::parseAuto()
{
    if (!detAuto)
    {
        std::cout << "no determinsitic automaton\n";
        return;
    }
    AutoParser myParser(*detAuto);
    myParser.parse();
    std::cout << myParser;
}

void Console::buildDeterm()
{
    if (detAuto)
    {
        delete detAuto;
    }
    detAuto = enterDeterm();
}

bool Console::choiceIsDetertm()
{
    std::cout << "Deterministic(determ) or nondeterministic(nondeterm): \n";
    std::string choice;
    std::cin >> choice;
    if (choice.compare("determ") == 0)
    {
        return true;
    }
    if (choice.compare("nondeterm") == 0)
    {
        return false;
    }
    throw std::invalid_argument("wrong choice\n");
}

void Console::show()
{
    bool choice;
    try
    {
        choice = choiceIsDetertm();
    }
    catch (std::invalid_argument a)
    {
        std::cout << a.what() << std::endl;
        return;
    }
    if (choice)
    {
        if (detAuto)
        {
            std::cout << *detAuto;
            return;
        }
        std::cout << "No determ auto!\n";
        return;
    }
    if (nondetAuto)
    {
        std::cout << *nondetAuto;
        return;
    }
    std::cout << "No nondeterm auto!\n";
}

void Console::buildNondeterm()
{
    if (nondetAuto)
    {
        delete nondetAuto;
    }
    nondetAuto = enterNondeterm();
}

void Console::wordFromStream()
{
    if (!detAuto)
    {
        std::cout << "There is no det auto!\n";
        return;
    }
    std::string nextWord;
    detAuto->getWordFromStream(std::cin) >> nextWord;
    std::cout << std::endl;
}

void Console::operation()
{
    std::cout << "Choose an operation: union/intersection(inter)/concatenation(concat)/complementation(complem)\n";
    std::string choice;
    std::cin >> choice;
    if (choice.compare("union") == 0)
    {
        operationHelper(DetermAutomaton::unions, NondetermAutomaton::unions);
        return;
    }
    if (choice.compare("intersection") == 0 || choice.compare("inter") == 0)
    {
        operationHelper(DetermAutomaton::intersection, NondetermAutomaton::intersection);
        return;
    }
    if (choice.compare("complementation") == 0 || choice.compare("complem") == 0)
    {
        complementation();
        return;
    }
    if (choice.compare("concatenation") == 0 || choice.compare("concat") == 0)
    {
        concatenation();
        return;
    }
    std::cout << "wrong choice!\n";
}

DetermAutomaton *Console::enterDeterm()
{
    DetermAutomaton *myDetAuto = new DetermAutomaton();
    std::cout << "Enter a deterministic automaton\n";
    std::cout << "Enter first node: ";
    std::string nodeName1, nodeName2;
    std::cin >> nodeName1;
    try
    {
        myDetAuto->addFirstNode(nodeName1);
    }
    catch (std::invalid_argument exc)
    {
        std::cout << exc.what() << "\n";
    }
    char transitionSymbol;

    std::string choice;
    bool toContinue = true;
    while (toContinue)
    {
        std::cout << "enter transition: ";
        std::cin >> nodeName1 >> transitionSymbol >> nodeName2;
        try
        {
            myDetAuto->addTransition(nodeName1, transitionSymbol, nodeName2);
        }
        catch (std::invalid_argument exc)
        {
            std::cout << exc.what() << "\n";
        }
        std::cout << "Add more transitions? ";
        std::cin >> choice;

        toContinue = (choice.compare("yes") == 0);
    }
    toContinue = true;
    std::cout << "Final nodes:\n";
    while (toContinue)
    {
        std::cin >> nodeName2;
        try
        {
            myDetAuto->addFinals(nodeName2);
        }
        catch (std::invalid_argument exc)
        {
            std::cout << exc.what() << "\n";
        }
        std::cout
            << "Add more final nodes? ";
        std::cin >> choice;

        toContinue = (choice.compare("yes") == 0);
    }
    return myDetAuto;
}

NondetermAutomaton *Console::enterNondeterm()
{
    NondetermAutomaton *myNondetAuto = new NondetermAutomaton();
    std::cout << "Enter a nondeterministic automaton\n";
    std::cout << "Enter first node: ";
    std::string nodeName1, nodeName2;
    std::cin >> nodeName1;
    try
    {
        myNondetAuto->addFirstNode(nodeName1);
    }
    catch (std::invalid_argument exc)
    {
        std::cout << exc.what() << "\n";
    }
    char transitionSymbol;

    std::string choice;
    bool toContinue = true;
    while (toContinue)
    {
        std::cout << "enter transition: ";
        std::cin >> nodeName1 >> transitionSymbol >> nodeName2;
        try
        {
            myNondetAuto->addTransition(nodeName1, transitionSymbol, nodeName2);
        }
        catch (std::invalid_argument exc)
        {
            std::cout << exc.what() << "\n";
        }
        std::cout << "Add more transitions? ";
        std::cin >> choice;

        toContinue = (choice.compare("yes") == 0);
    }
    toContinue = true;
    std::cout << "Final nodes:\n";
    while (toContinue)
    {
        std::cin >> nodeName2;
        try
        {
            myNondetAuto->addFinals(nodeName2);
        }
        catch (std::invalid_argument exc)
        {
            std::cout << exc.what() << "\n";
        }
        std::cout
            << "Add more final nodes? ";
        std::cin >> choice;

        toContinue = (choice.compare("yes") == 0);
    }
    return myNondetAuto;
}

void Console::operationHelper(std::function<DetermAutomaton(const DetermAutomaton &, const DetermAutomaton &)> detOper,
                              std::function<NondetermAutomaton(const NondetermAutomaton &, const NondetermAutomaton &)> nondetOper)
{
    bool choice;
    try
    {
        choice = choiceIsDetertm();
    }
    catch (std::invalid_argument a)
    {
        std::cout << a.what() << std::endl;
        return;
    }
    if (choice)
    {
        if (!detAuto)
        {
            detAuto = enterDeterm();
        }
        DetermAutomaton *other = enterDeterm();
        DetermAutomaton *result = new DetermAutomaton(detOper(*detAuto, *other));
        delete other;
        delete detAuto;
        detAuto = result;
        std::cout << "Succesful operation\n";
        return;
    }
    if (!nondetAuto)
    {
        nondetAuto = enterNondeterm();
    }
    NondetermAutomaton *other = enterNondeterm();
    NondetermAutomaton *result = new NondetermAutomaton(nondetOper(*nondetAuto, *other));
    delete other;
    delete nondetAuto;
    nondetAuto = result;
    std::cout << "Succesful operation\n";
}

void Console ::complementation()
{
    bool choice;
    try
    {
        choice = choiceIsDetertm();
    }
    catch (std::invalid_argument a)
    {
        std::cout << a.what() << std::endl;
        return;
    }
    if (choice)
    {
        if (!detAuto)
        {
            detAuto = enterDeterm();
        }

        DetermAutomaton *result = new DetermAutomaton(detAuto->complementation());
        delete detAuto;
        detAuto = result;
        std::cout << "Succesful operation\n";
        return;
    }
    if (!nondetAuto)
    {
        nondetAuto = enterNondeterm();
    }
    NondetermAutomaton *result = new NondetermAutomaton(nondetAuto->complementation());
    delete nondetAuto;
    nondetAuto = result;
    std::cout << "Succesful operation\n";
}

void Console ::concatenation()
{
    if (!nondetAuto)
    {
        nondetAuto = enterNondeterm();
    }
    NondetermAutomaton *other = enterNondeterm();
    NondetermAutomaton *result = new NondetermAutomaton(NondetermAutomaton::concatenation(*nondetAuto, *other));
    delete other;
    delete nondetAuto;
    nondetAuto = result;
    std::cout << "Succesful operation\n";
}

void Console::help()
{
    std::cout << "auto : builds an auto\n"
              << "test : run tests\n"
              << "parseExpr : parse a regular expression to a nondeterm auto\n"
              << "parseAuto : parses a determ auto to a regular expression and"
              << "prints the expression\n"
              << "check : checks whether a word is from the language of an auto\n"
              << "stream : checks whether a word from a stream is from the language of an auto\n"
              << "show : prints info about an auto\n"
              << "operation : performs an operation with an auto\n";
}

void Console::myMain()
{
    enterCommand();
    while (!command.empty() && command.compare("exit") != 0)
    {
        doCommand();
        enterCommand();
    }
    std::cout << "Exit the programme!\n";
}

#endif

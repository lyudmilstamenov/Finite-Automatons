#include "regExpressionParser.h"

bool Parser::isOperation(const char &symbol)
{
    return symbol == '+' || symbol == '.' || symbol == '*' || symbol == '&';
}

int Parser::priority(const char &symbol)
{
    switch (symbol)
    {
    case '+':
        return 1;
    case '&':
        return 1;
    case '.':
        return 2;
    case '*':
        return 3;
    default:
        return 0;
    }
}

NondetermAutomaton Parser::calculate(NondetermAutomaton &first, NondetermAutomaton &second, const char &oper)
{
    switch (oper)
    {
    case '+':
        return (NondetermAutomaton::unions(first, second));
    case '&':
        return NondetermAutomaton::intersection(first, second);
    case '.':
        return (NondetermAutomaton::concatenation(second, first));

    default:
        return NondetermAutomaton("@");
    }
}

NondetermAutomaton Parser::calculateKleeneStar(NondetermAutomaton &first)
{
    return NondetermAutomaton::kleeneStar(first);
}

Parser::Parser(std::string &str) : expression{str} {}

std::vector<std::pair<std::optional<NondetermAutomaton>, std::optional<char>>> Parser::parse()
{
    for (int i = 0; i < expression.size(); i++)
    {
        if (isOperation(expression[i]))
        {
            while (!operations.empty() && priority(expression[i]) < priority(operations.top()))
            {
                parsedString.push_back(std::pair(std::nullopt, operations.top()));
                operations.pop();
            }
            operations.push(expression[i]);
        }
        else if (expression[i] == '(')
        {
            operations.push('(');
        }
        else if (expression[i] == ')')
        {
            while (!operations.empty() && operations.top() != '(')
            {
                parsedString.push_back(std::pair(std::nullopt, operations.top()));
                operations.pop();
            }

            if (!operations.empty() && operations.top() == '(')
            {
                operations.pop();
            }
        }
        else
        {
            std::string str = getString(expression, i);
            if (expression[++i] == '*')
            {
                std::string repeatedSymbol = str.substr(str.size() - 1, 1);
                str.pop_back();
                NondetermAutomaton charAuto(repeatedSymbol);
                NondetermAutomaton stringAuto(str);
                NondetermAutomaton kleeneAuto = NondetermAutomaton::kleeneStar(charAuto);

                parsedString.push_back(std::pair(NondetermAutomaton::concatenation(stringAuto,
                                                                                   kleeneAuto),
                                                 std::nullopt));
            }
            else
            {
                --i;
                parsedString.push_back(std::pair(NondetermAutomaton(str), std::nullopt));
            }
        }
    }

    while (!operations.empty())
    {
        parsedString.push_back(std::pair(std::nullopt, operations.top()));
        operations.pop();
    }

    return parsedString;
}

std::string Parser::getString(const std::string &expression, int &index)
{

    if (expression[index] == '@')
    {
        return "@";
    }
    std::string str;
    while (index < expression.size() && expression[index] != '(' && expression[index] != ')' && !isOperation(expression[index]))
    {
        str.push_back(expression[index++]);
    }
    index--;
    return str;
}

bool Parser::isAuto(const std::pair<std::optional<NondetermAutomaton>, std::optional<char>> &myPair)
{
    return (bool)(myPair.first);
}

NondetermAutomaton Parser::solve()
{
    parse();
    while (!autos.empty())
    {
        autos.pop();
    }

    for (int i = 0; i < parsedString.size(); i++)
    {
        if (isAuto(parsedString[i]))
        {
            autos.push(parsedString[i].first.value());
        }
        else if (isOperation(parsedString[i].second.value()))
        {
            NondetermAutomaton one = autos.top();
            autos.pop();

            if (parsedString[i].second.value() == '*')
            {
                NondetermAutomaton result = calculateKleeneStar(one);
                autos.push(result);
            }
            else
            {
                NondetermAutomaton two = autos.top();
                autos.pop();
                NondetermAutomaton result = calculate(one, two, parsedString[i].second.value());
                autos.push(result);
            }
        }
    }

    return autos.top();
}

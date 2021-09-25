#ifndef TESTS_H
#define TESTS_H
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "../Deterministic/determAuto.h"
#include "../Nondeterministic/nondetermAuto.h"
#include "../Regular expressions to auto/regExpressionParser.h"

TEST_CASE("Test stream and Word from automaton")
{
    DetermAutomaton myAuto;
    myAuto.addFirstNode("A");
    myAuto.addTransition("A", '0', "A");
    myAuto.addTransition("A", '1', "B");
    myAuto.addTransition("B", '0', "C");
    myAuto.addTransition("C", '1', "C");
    myAuto.addTransition("C", '0', "B");
    myAuto.addTransition("B", '1', "A");
    myAuto.addFinals("A");
    std::stringstream in("01011110100");
    std::string f;
    myAuto.getWordFromStream(in) >> f;
    CHECK(myAuto.isWordFromAuto(f));
    std::stringstream in2("502");

    myAuto.getWordFromStream(in2) >> f;
    CHECK(!myAuto.isWordFromAuto(f));
}

TEST_CASE("Test Word from automaton")
{
    DetermAutomaton myAuto;
    myAuto.addFirstNode("A");
    myAuto.addTransition("A", '0', "A");
    myAuto.addTransition("A", '1', "B");
    myAuto.addTransition("B", '0', "C");
    myAuto.addTransition("C", '1', "C");
    myAuto.addTransition("C", '0', "B");
    myAuto.addTransition("B", '1', "A");
    myAuto.addFinals("A");

    std::string w = "0000";
    CHECK(myAuto.isWordFromAuto(w));
    w = "01011110100";
    CHECK(myAuto.isWordFromAuto(w));
    w = "0011000111";
    CHECK(!myAuto.isWordFromAuto(w));
    w = "01011110111";
    CHECK(myAuto.isWordFromAuto(w));
}

TEST_CASE("Test complementation of an automaton")
{
    DetermAutomaton myAuto;
    myAuto.addFirstNode("A");
    myAuto.addTransition("A", '0', "A");
    myAuto.addTransition("A", '1', "B");
    myAuto.addTransition("B", '0', "C");
    myAuto.addTransition("B", '1', "A");
    myAuto.addTransition("C", '1', "C");
    myAuto.addTransition("C", '0', "B");

    myAuto.addFinals("C");

    DetermAutomaton newAuto = myAuto.complementation();

    std::string w = "0000";

    CHECK(newAuto.isWordFromAuto(w));
    w = "00001";
    CHECK(newAuto.isWordFromAuto(w));
    w = "000101";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "51011110111";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "001011101100";
    CHECK(newAuto.isWordFromAuto(w));
}

TEST_CASE("Test intersection of an deterministic automaton")
{
    DetermAutomaton a, b;
    a.addFirstNode("X0");
    a.addTransition("X0", '0', "X0");
    a.addTransition("X0", '1', "X1");
    a.addTransition("X1", '1', "X0");
    a.addTransition("X1", '0', "X1");
    a.addFinals("X1");

    b.addFirstNode("Y0");
    b.addTransition("Y0", '0', "Y1");
    b.addTransition("Y0", '1', "Y1");
    b.addTransition("Y1", '0', "Y0");
    b.addTransition("Y1", '1', "Y0");
    b.addFinals("Y0");
    DetermAutomaton c(DetermAutomaton::intersection(a, b));

    std::string w = "100000";
    CHECK(c.isWordFromAuto(w));
    w = "0101";
    CHECK(!c.isWordFromAuto(w));
    w = "00101001";
    CHECK(c.isWordFromAuto(w));
    w = "0000";
    CHECK(!c.isWordFromAuto(w));
}

TEST_CASE("Test union of deterministic automatons")
{
    DetermAutomaton a, b;
    a.addFirstNode("X0");
    a.addTransition("X0", '0', "X0");
    a.addTransition("X0", '1', "X1");
    a.addTransition("X1", '1', "X0");
    a.addTransition("X1", '0', "X1");
    a.addFinals("X1");

    b.addFirstNode("Y0");
    b.addTransition("Y0", '0', "Y1");
    b.addTransition("Y0", '1', "Y1");
    b.addTransition("Y1", '0', "Y0");
    b.addTransition("Y1", '1', "Y0");
    b.addFinals("Y0");

    DetermAutomaton newAuto;
    newAuto = DetermAutomaton::unions(a, b);

    std::string w = "0011100";
    CHECK(newAuto.isWordFromAuto(w));
    w = "0000110011000100";
    CHECK(newAuto.isWordFromAuto(w));
    w = "1111000111";
    CHECK(newAuto.isWordFromAuto(w));
    w = "111110000111";
    CHECK(newAuto.isWordFromAuto(w));
    w = "11110";
    CHECK(!newAuto.isWordFromAuto(w));
}

TEST_CASE("Test Word from nondetermnistic automaton")
{
    NondetermAutomaton myAuto;
    myAuto.addFirstNode("0");
    myAuto.addTransition("0", '@', "1");
    myAuto.addTransition("0", '@', "4");
    myAuto.addTransition("1", 'a', "2");
    myAuto.addTransition("2", 'a', "2");
    myAuto.addTransition("2", 'b', "2");
    myAuto.addTransition("2", 'a', "3");
    myAuto.addTransition("4", 'b', "5");
    myAuto.addTransition("5", 'a', "5");
    myAuto.addTransition("5", 'b', "5");
    myAuto.addTransition("5", 'b', "6");
    myAuto.addTransition("6", '@', "7");
    myAuto.addTransition("3", '@', "7");
    myAuto.addFinals("7");

    std::string w = "aaaaa";
    CHECK(myAuto.isWordFromAuto(w));
    w = "a";
    CHECK(!myAuto.isWordFromAuto(w));
    w = "baaa";
    CHECK(!myAuto.isWordFromAuto(w));
    w = "aabbbba";
    CHECK(myAuto.isWordFromAuto(w));
}

TEST_CASE("Test complementation of a nondeterministic automaton")
{
    NondetermAutomaton myAuto;
    myAuto.addFirstNode("0");
    myAuto.addTransition("0", '@', "1");
    myAuto.addTransition("0", '@', "4");
    myAuto.addTransition("1", 'a', "2");
    myAuto.addTransition("2", 'a', "2");
    myAuto.addTransition("2", 'b', "2");
    myAuto.addTransition("2", 'a', "3");
    myAuto.addTransition("4", 'a', "5");
    myAuto.addTransition("5", 'a', "5");
    myAuto.addTransition("5", 'b', "5");
    myAuto.addTransition("5", 'a', "6");
    myAuto.addTransition("6", '@', "7");
    myAuto.addTransition("3", '@', "7");
    myAuto.addFinals("7");

    NondetermAutomaton newAuto = myAuto.complementation();

    std::string w = "c";

    CHECK(!newAuto.isWordFromAuto(w));
    w = "aa";
    CHECK(newAuto.isWordFromAuto(w));
    w = "a";
    CHECK(newAuto.isWordFromAuto(w));
    w = "ababab";
    CHECK(newAuto.isWordFromAuto(w));
    w = "aaabbaaab";
    CHECK(newAuto.isWordFromAuto(w));
}
TEST_CASE("Test union of nondeterministic automatons")
{
    NondetermAutomaton firstAuto;
    firstAuto.addFirstNode("1");
    firstAuto.addTransition("1", '@', "3");
    firstAuto.addTransition("1", 'a', "2");
    firstAuto.addTransition("2", 'c', "2");
    firstAuto.addTransition("2", 'b', "2");
    firstAuto.addTransition("2", 'd', "3");
    firstAuto.addFinals("3");
    NondetermAutomaton secondAuto;
    secondAuto.addFirstNode("4");
    secondAuto.addTransition("4", 'a', "4");
    secondAuto.addTransition("4", 'e', "5");
    secondAuto.addTransition("5", 'e', "4");
    secondAuto.addTransition("5", 'e', "6");
    secondAuto.addFinals("6");

    NondetermAutomaton newAuto;
    newAuto = NondetermAutomaton::unions(firstAuto, secondAuto);

    std::string w = "";
    CHECK(newAuto.isWordFromAuto(w));
    w = "ae";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "e";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aaaaaeeaaeee";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aaaaaaeeaaaeeee";
    CHECK(newAuto.isWordFromAuto(w));
    w = "aaaeeea";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "ad";
    CHECK(newAuto.isWordFromAuto(w));
    w = "abcccbbbcbd";
    CHECK(newAuto.isWordFromAuto(w));
}

TEST_CASE("Test concatenation of nondeterministic automatons")
{

    NondetermAutomaton firstAuto;
    firstAuto.addFirstNode("1");
    firstAuto.addTransition("1", 'a', "2");
    firstAuto.addTransition("2", 'c', "2");
    firstAuto.addTransition("2", 'b', "2");
    firstAuto.addTransition("1", '@', "3");
    firstAuto.addTransition("2", 'd', "3");
    firstAuto.addFinals("3");
    NondetermAutomaton secondAuto;
    secondAuto.addFirstNode("4");
    secondAuto.addTransition("4", 'a', "4");
    secondAuto.addTransition("4", 'e', "5");
    secondAuto.addTransition("5", 'e', "4");
    secondAuto.addTransition("5", 'e', "6");
    secondAuto.addFinals("6");

    NondetermAutomaton newAuto(NondetermAutomaton::concatenation(firstAuto, secondAuto));
    std::string w = "aaee";
    CHECK(newAuto.isWordFromAuto(w));
    w = "abdaaaeeee";

    CHECK(newAuto.isWordFromAuto(w));
    w = "";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aabcbcbccccbdee";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "adee";
    CHECK(newAuto.isWordFromAuto(w));
    w = "aaaeeea";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "adeee";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aaaeeee";
    CHECK(newAuto.isWordFromAuto(w));
}

TEST_CASE("Test concatenation of nondeterministic automatons with same names")
{

    NondetermAutomaton firstAuto;
    firstAuto.addFirstNode("1");
    firstAuto.addTransition("1", 'a', "2");
    firstAuto.addTransition("2", 'c', "2");
    firstAuto.addTransition("2", 'b', "2");
    firstAuto.addTransition("1", '@', "3");
    firstAuto.addTransition("2", 'd', "3");
    firstAuto.addFinals("3");
    NondetermAutomaton secondAuto;
    secondAuto.addFirstNode("1");
    secondAuto.addTransition("1", 'a', "1");
    secondAuto.addTransition("1", 'e', "2");
    secondAuto.addTransition("2", 'e', "1");
    secondAuto.addTransition("2", 'e', "3");
    secondAuto.addFinals("3");

    NondetermAutomaton newAuto(NondetermAutomaton::concatenation(firstAuto, secondAuto));
    std::string w = "aaee";
    CHECK(newAuto.isWordFromAuto(w));
    w = "abdaaaeeee";

    CHECK(newAuto.isWordFromAuto(w));
    w = "";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aabcbcbccccbdee";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "adee";
    CHECK(newAuto.isWordFromAuto(w));
    w = "aaaeeea";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "adeee";
    CHECK(!newAuto.isWordFromAuto(w));
    w = "aaaeeee";
    CHECK(newAuto.isWordFromAuto(w));
}

TEST_CASE("Test intersection of a nondeterministic automaton")
{
    NondetermAutomaton a, b;
    a.addFirstNode("X0");
    a.addTransition("X0", 'a', "X1");
    a.addTransition("X1", 'b', "X2");
    a.addTransition("X2", 'b', "X1");
    a.addTransition("X1", '@', "X3");
    a.addTransition("X3", 'c', "X4");
    a.addFinals("X4");

    b.addFirstNode("Y0");
    b.addTransition("Y0", 'a', "Y1");
    b.addTransition("Y1", 'a', "Y0");
    b.addTransition("Y1", '@', "Y2");
    b.addTransition("Y0", '@', "Y2");
    b.addTransition("Y2", 'b', "Y3");
    b.addTransition("Y3", 'b', "Y4");
    b.addTransition("Y4", 'c', "Y5");
    b.addFinals("Y5");

    NondetermAutomaton c(NondetermAutomaton::intersection(a, b));
    std::string w = "abbc";
    CHECK(c.isWordFromAuto(w));
    w = "aaabbc";
    CHECK(!c.isWordFromAuto(w));
    w = "abbbbc";
    CHECK(!c.isWordFromAuto(w));
}

TEST_CASE("Test building a nondeterministic automaton from a word")
{
    NondetermAutomaton myAuto("abc");
    std::string w = "abc";
    CHECK(myAuto.isWordFromAuto(w));
    w = "abcc";
    CHECK(!myAuto.isWordFromAuto(w));
}

TEST_CASE("Test parsing regular expression to an auto")
{

    std::string str = "ab.(abc*+bc)*.ca";

    Parser parser{str};
    NondetermAutomaton myAuto(parser.solve());

    CHECK(myAuto.isWordFromAuto("ababcca"));
    CHECK(myAuto.isWordFromAuto("ababca"));
    CHECK(myAuto.isWordFromAuto("ababcccccca"));
    CHECK(myAuto.isWordFromAuto("abbcbcabcbcca"));
    CHECK(myAuto.isWordFromAuto("abca"));
    CHECK(myAuto.isWordFromAuto("ababcabcbcabcccabca"));
    CHECK(!myAuto.isWordFromAuto("abcabcbcabcccabca"));
    CHECK(!myAuto.isWordFromAuto("abacca"));
    CHECK(!myAuto.isWordFromAuto("caca"));
    CHECK(!myAuto.isWordFromAuto("abcc"));
}

TEST_CASE("Test parsing regular expression with intersection to an auto")
{

    std::string str = "(ab+ba)&(ab+b)";

    Parser parser{str};
    NondetermAutomaton myAuto(parser.solve());

    CHECK(myAuto.isWordFromAuto("ab"));
    CHECK(!myAuto.isWordFromAuto("ba"));
    CHECK(!myAuto.isWordFromAuto("b"));
    CHECK(!myAuto.isWordFromAuto("ac"));
}

TEST_CASE("Test parsing regular expression to an auto")
{

    std::string str = "ab.(ab*&a*)*.ca";

    Parser parser{str};
    NondetermAutomaton myAuto(parser.solve());

    CHECK(myAuto.isWordFromAuto("abca"));
    CHECK(myAuto.isWordFromAuto("abaaaaaca"));
    CHECK(myAuto.isWordFromAuto("abaca"));
    CHECK(!myAuto.isWordFromAuto("ababca"));
    CHECK(!myAuto.isWordFromAuto("abaabbca"));
}

TEST_CASE("Test parsing regular expression with intersection to an auto")
{

    std::string str = "ab.(ab*.c*&a*.b*.c)*.ca";

    Parser parser{str};
    NondetermAutomaton myAuto(parser.solve());

    CHECK(myAuto.isWordFromAuto("ababbbbbcca"));
    CHECK(myAuto.isWordFromAuto("abacca"));
    CHECK(myAuto.isWordFromAuto("ababbcacabcca"));
    CHECK(myAuto.isWordFromAuto("ababcca"));
    CHECK(myAuto.isWordFromAuto("abca"));
    CHECK(!myAuto.isWordFromAuto("abaaabbbca"));
    CHECK(!myAuto.isWordFromAuto("caca"));
    CHECK(!myAuto.isWordFromAuto("abcc"));
}

TEST_CASE("Test parsing an auto to a regular expression ")
{

    DetermAutomaton a;
    a.addFirstNode("A");
    a.addTransition("A", 'a', "B");
    a.addTransition("A", 'b', "C");
    a.addTransition("B", 'a', "C");
    a.addTransition("B", 'b', "D");
    a.addTransition("C", 'b', "B");
    a.addFinals("D");
    a.addFinals("C");
    AutoParser c(a);
    c.parse();
    CHECK(c.getRegExpression().compare("((a(ab)*)b+ab+(b(ba)*b)b)+(b+(a(ab)*a)+(b(ba)*))") == 0);
}

void test()
{
    doctest::Context().run();
}
#endif

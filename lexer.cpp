
// Name: Fanny Dolisy
// CS 4386.501
// Assignment 1, DFA for ASN1

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// class used to simulate a DFA for this lexical analyzer
class DFA
{

private:
    // strings to represent certain character classes
    std::string DIGITS_NO_ZERO = "123456789";
    std::string LETTERS_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string LETTERS_LOWER = "abcdefghijklmnopqrstuvwxyz";

    // int to represent current position if the token if a keyword or requires more than one character analysis
    int keywordPost = 0;

public:

    // the current char the DFA is reading
    char currentChar;
    // the current state the DFA is in
    std::string officialState = "Start";
    // vectors for both states that require a lookahead and tokens that have been accepted
    std::vector<std::string> lookAheadStates, acceptedStates;
    // the lexeme that may be printed out with a token
    std::string lexeme = "";
    // are we at the end of a line
    bool EOL = false;

    //helper functions
    bool isWhiteSpace(char);
    bool containsChar(std::string, char);
    void findWhichLookAhead();
    bool checkSymbol(char);
    void findNextState();
    void acceptKeyWordState(std::string);
    void rejectKeywordState();

    // general states
    void start(char);
    void typeReference(char);
    void identifier(char);
    void assign(char);
    void rangeSep(char);
    void number(char);

    // keyword states
    void BEGIN(char);
    void DATE(char);
    void TAGS(char);
    void END(char);
    void INTEGER(char);
    void SEQUENCE(char);

    // lookahead states (other than keywords)
    void hyphen(char);
    void zero(char);
};

// determines if the current character is a white space
bool DFA::isWhiteSpace(char c)
{
    if ((c >= 9 && c <= 13) | c == 32)
    {
        return true;
    }
    else
    {
        return false;
    }
}
// determines if the current character is present in a string
bool DFA::containsChar(std::string name, char c)
{
    return(name.find(c) != std::string::npos);
}
// determines if current char is a valid symbol in the characterset
bool DFA::checkSymbol(char currentChar)
{
    bool found = true;
    switch (currentChar)
    {
    case '0':
        lookAheadStates.push_back("Zero");
        break;
    case ':':
        officialState = "toAssign";
        break;
    case '.':
        officialState = "toRangeSep";
        break;
    case '{':
        acceptedStates.push_back("LCURLY");
        break;
    case '}':
        acceptedStates.push_back("RCURLY");
        break;
    case ',':
        acceptedStates.push_back("COMMA");
        break;
    case '(':
        acceptedStates.push_back("LPAREN");
        break;
    case ')':
        acceptedStates.push_back("RPAREN");
        break;
    case '|':
        acceptedStates.push_back("BAR");
        break;
    case '\"':
        acceptedStates.push_back("QUOTE");
        break;
    default:
        found = false;
    }
    return found;
}

void DFA::acceptKeyWordState(std::string keyword)
{
    acceptedStates.push_back(keyword);
    lookAheadStates.clear();
    keywordPost = 0;
    officialState = "Start";
    lexeme = "";
}

void DFA::rejectKeywordState()
{
    lookAheadStates.clear();
    lexeme += currentChar;
    keywordPost = 0;
}
// Start state for the DFA, looks at all possibilities for basic char
void DFA::start(char currentChar)
{
    //Starting with a capital letter: could be typeref o
    if (containsChar(LETTERS_UPPER, currentChar))
    {
        // KEY WORDS
        switch (currentChar)
        {
        case 'B':
            lookAheadStates.push_back("BEGIN");
            keywordPost++;
            break;
        case 'D':
            lookAheadStates.push_back("DATE");
            keywordPost++;
            break;
        case 'E':
            lookAheadStates.push_back("END");
            keywordPost++;
            break;
        case 'I':
            lookAheadStates.push_back("INTEGER");
            keywordPost++;
            break;
        case 'S':
            lookAheadStates.push_back("SEQUENCE");
            keywordPost++;
            break;
        case 'T':
            lookAheadStates.push_back("TAGS");
            keywordPost++;
            break;
        }

        // if not a keyword, it is  a typereference
        officialState = "toTypeRef";
    }
    else if (containsChar(LETTERS_LOWER, currentChar))
    {
        // starts with a lower case letter
        officialState = "toIdentifier";

    }
    else if (containsChar(DIGITS_NO_ZERO, currentChar))
    {
        // starts with a nonzero number
        officialState = "toNumber";
    }
    else if (isWhiteSpace(currentChar))
    {
        // is a whitespace, ignore
        officialState = "Start";
    }
    else
    {
        // it is a symbol or bad input
        if (!checkSymbol(currentChar))
        {
            officialState = "fail";
        }
    }
}

// BEGIN keyword state for DFA
void DFA::BEGIN(char currentChar)
{
    // word is accepted
    if ((isWhiteSpace(currentChar) && keywordPost == 5) || (EOL && currentChar == 'N' && keywordPost == 4))
    {
        acceptKeyWordState("BEGIN");
        checkSymbol(currentChar);
    }
    else if (currentChar == 'E' && keywordPost == 1 || currentChar == 'G' && keywordPost == 2 || currentChar == 'I' && keywordPost == 3 || currentChar == 'N' && keywordPost == 4)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}
// DATE keyword state for DFA
void DFA::DATE(char currentChar)
{
    // word is accepted  
    if ((isWhiteSpace(currentChar) && keywordPost == 4) || (EOL && currentChar == 'E' && keywordPost == 3) || lexeme == "DATE")
    {
        acceptKeyWordState("DATE");
        checkSymbol(currentChar);
    }
    else if (currentChar == 'A' && keywordPost == 1 || currentChar == 'T' && keywordPost == 2 || currentChar == 'E' && keywordPost == 3)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}
// TAGS keyword state for DFA
void DFA::TAGS(char currentChar)
{
    // word is accepted   
    if (isWhiteSpace(currentChar) && keywordPost == 4 || (EOL && currentChar == 'S' && keywordPost == 3))
    {
        //accept, no other possibility
        acceptKeyWordState("TAGS");
        checkSymbol(currentChar);
    }

    else if (currentChar == 'A' && keywordPost == 1 || currentChar == 'G' && keywordPost == 2 || currentChar == 'S' && keywordPost == 3)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}
// END keyword state for DFA
void DFA::END(char currentChar)
{
    // word is accepted
    if (isWhiteSpace(currentChar) && keywordPost == 3 || (EOL && currentChar == 'D' && keywordPost == 2))
    {
        acceptKeyWordState("END");
        checkSymbol(currentChar);
    }
    else if (currentChar == 'N' && keywordPost == 1 || currentChar == 'D' && keywordPost == 2)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}
// INTEGER keyword state for DFA
void DFA::INTEGER(char currentChar)
{

    // word is accepted
    if (isWhiteSpace(currentChar) && keywordPost == 7 || (EOL && currentChar == 'R' && keywordPost == 6))
    {
        //accept, no other possibility
        acceptKeyWordState("INTEGER");
        checkSymbol(currentChar);
    }
    else if (currentChar == 'N' && keywordPost == 1 || currentChar == 'T' && keywordPost == 2 || currentChar == 'E' && keywordPost == 3 ||
        currentChar == 'G' && keywordPost == 4 || currentChar == 'E' && keywordPost == 5 || currentChar == 'R' && keywordPost == 6)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}
// SEQUENCE keyword state for DFA
void DFA::SEQUENCE(char currentChar)
{

    // word is accepted
    if (isWhiteSpace(currentChar) && keywordPost == 8 || (EOL && currentChar == 'E' && keywordPost == 7))
    {
        //accept, no other possibility
        acceptKeyWordState("SEQUENCE");
        checkSymbol(currentChar);
    }
    else if (currentChar == 'E' && keywordPost == 1 || currentChar == 'Q' && keywordPost == 2 || currentChar == 'U' && keywordPost == 3 ||
        currentChar == 'E' && keywordPost == 4 || currentChar == 'N' && keywordPost == 5 || currentChar == 'C' && keywordPost == 6 ||
        currentChar == 'E' && keywordPost == 7)
    {
        // continue processing word
        keywordPost++;
        lexeme += currentChar;
    }
    else
    {
        // not actually a keyword, it is a typeReference
        rejectKeywordState();
        if (currentChar == '-')
        {
            hyphen(currentChar);
        }
    }
}

// State if zero is the first character in a lexeme
void DFA::zero(char currentChar)
{
    if (isWhiteSpace(currentChar))
    {
        acceptedStates.push_back("Number");
    }
    else
    {
        officialState = "fail";
    }
}
// State if nonzero digit is the first character in a lexeme
void DFA::number(char currentChar)
{
    // continue accepting number
    if (isdigit(currentChar))
    {
        officialState = "toNumber";
        lexeme += currentChar;
    }
    else if (!isdigit(currentChar))
    {
        acceptedStates.push_back("Number");
        if (currentChar == '.')
        {
            officialState = "toRangeSep";
        }
        else
        {
            checkSymbol(currentChar);
            officialState = "Start";
        }

    }
    else
    {
        officialState = "fail";
    }
}
// state if hyphen is detected
void DFA::hyphen(char currentChar)
{
    if (isWhiteSpace(currentChar) || EOL)
    {
        officialState = "fail";
    }
    else
    {
        lookAheadStates.clear();
    }
}
// state for a typeReference
void DFA::typeReference(char currentChar)
{
    if (currentChar == '-')
    {
        // ensure the hyphen is acceptable
        lookAheadStates.push_back("hyphen");
    }
    // if end of line, accept the type reference
    else if (EOL)
    {
        acceptedStates.push_back("typereference");
        if (isalnum(currentChar))
        {
            lexeme += currentChar;
        }
        checkSymbol(currentChar);
        officialState = "Start";

    }
    else if (isalnum(currentChar))
    {
        // continue processing typereference
        officialState = "toTypeRef";
        lexeme += currentChar;
    }
    else if (isWhiteSpace(currentChar))
    {
        //accept type reference
        acceptedStates.push_back("typereference");
        officialState = "Start";

    }
    else
    {
        officialState = "fail";
    }


}
// state for an identifier
void DFA::identifier(char currentChar)
{
    // still a gucci type ref
    if (EOL)
    {
        acceptedStates.push_back("identifier");
        if (isalnum(currentChar))
        {
            lexeme += currentChar;
        }
        checkSymbol(currentChar);
        officialState = "Start";

    }
    else if (isalnum(currentChar))
    {
        // continue processing typereference
        officialState = "toIdentifier";
        lexeme += currentChar;
    }
    else if (currentChar == '-')
    {
        // ensure the hyphen is acceptable
        lookAheadStates.push_back("hyphen");
    }
    else if (isWhiteSpace(currentChar))
    {
        // accept
        acceptedStates.push_back("identifier");
        officialState = "Start";

    }
    else
    {
        // reject
        officialState = "fail";
    }


}
// state for an assign
void DFA::assign(char currentChar)
{
    if (currentChar == ':' && keywordPost == 0 || currentChar == ':' && keywordPost == 1 || currentChar == '=' && keywordPost == 2)
    {
        // continue processing input
        keywordPost++;
    }
    else if (isWhiteSpace(currentChar) && keywordPost == 3)
    {
        //accept if next char is white space
        acceptedStates.push_back("assign");
        keywordPost = 0;
        officialState = "Start";
    }
    else if ((containsChar(LETTERS_UPPER, currentChar) || containsChar(LETTERS_LOWER, currentChar)) && keywordPost == 3)
    {
        // next char is non white space
        lexeme += currentChar;
        acceptedStates.push_back("assign");
        keywordPost = 0;
        officialState = "Start";
    }
    else
    {
        officialState = "fail";
    }
}
// state for a range seperator
void DFA::rangeSep(char currentChar)
{

    if (currentChar == '.' && keywordPost == 0 || currentChar == '.' && keywordPost == 1)
    {
        // continue processing input
        keywordPost++;
    }
    else if (isdigit(currentChar) && keywordPost == 2)
    {
        //accept if next char is a digit
        lexeme += currentChar;
        acceptedStates.push_back("RangeSep");
        keywordPost = 0;
        officialState = "toNumber";
    }
    else if (isWhiteSpace(currentChar) && keywordPost == 2)
    {
        // accept if next char is white space
        acceptedStates.push_back("RangeSep");
        keywordPost = 0;
        officialState = "Start";
    }
    else
    {
        officialState = "fail";
    }
}

// helper function to ensure all lookahead states are analyzed before continuing
void DFA::findWhichLookAhead()
{
    // process all possible lookahead states before continuing
    for (int z = 0; z < lookAheadStates.size(); z++)
    {

        if (lookAheadStates[z] == "BEGIN")
        {
            BEGIN(currentChar);
        }
        else if (lookAheadStates[z] == "DATE")
        {
            DATE(currentChar);
        }
        else if (lookAheadStates[z] == "TAGS")
        {
            TAGS(currentChar);
        }
        else if (lookAheadStates[z] == "END")
        {
            END(currentChar);
        }
        else if (lookAheadStates[z] == "SEQUENCE")
        {
            SEQUENCE(currentChar);
        }
        else if (lookAheadStates[z] == "INTEGER")
        {
            INTEGER(currentChar);
        }
        else if (lookAheadStates[z] == "hyphen")
        {
            hyphen(currentChar);
        }
        else if (lookAheadStates[z] == "Zero")
        {
            zero(currentChar);
        }
        else if (lookAheadStates[z] == "Start")
        {
            start(currentChar);
        }
    }
}
// helper function to process next state
void DFA::findNextState()
{
    if (officialState == "Start")
    {
        start(currentChar);
    }
    if (officialState == "toTypeRef")
    {
        typeReference(currentChar);
    }
    if (officialState == "toIdentifier")
    {
        identifier(currentChar);
    }
    if (officialState == "toAssign")
    {
        assign(currentChar);
    }
    if (officialState == "toNumber")
    {
        number(currentChar);
    }
    if (officialState == "toRangeSep")
    {
        rangeSep(currentChar);
    }
}

int main()
{
    DFA dfa;
    std::ifstream MyReadFile("input.txt");
    std::string input = "";
    while (std::getline(MyReadFile, input))
    {
        // Output the text from the file
        for (int i = 0; i < input.length(); i++)
        {
            dfa.currentChar = input[i];

            //if end of line
            if (i == input.length() - 1)
            {
                dfa.EOL = true;
            }
            else
            {
                dfa.EOL = false;
            }


            // if no look ahead states, go to official ones
            if (!dfa.lookAheadStates.empty())
            {
                dfa.findWhichLookAhead();
            }
            else
            {
                dfa.findNextState();
            }

            // print out results if any accepted states
            if (!dfa.acceptedStates.empty())
            {
                for (int i = 0; i < dfa.acceptedStates.size(); i++)
                {
                    if (dfa.acceptedStates[i] == "typereference" || dfa.acceptedStates[i] == "identifier" || dfa.acceptedStates[i] == "Number")
                    {
                        std::cout << "<" << dfa.acceptedStates[i] << "> " << "  <" << dfa.lexeme << "> " << std::endl;
                        dfa.lexeme = "";
                    }
                    else
                    {
                        std::cout << "<" << dfa.acceptedStates[i] << "> " << std::endl;
                    }

                }
                dfa.acceptedStates.clear();
            }
            if (dfa.officialState == "fail")
            {
                std::cout << "Input not accepted" << std::endl;
                return 0;
            }


        }
    }
}





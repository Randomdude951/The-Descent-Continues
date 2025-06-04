//
//  main.cpp
//  The Descent Continues
//
//  Created by Tyler Franklin on 6/2/25.
//

#include <regex>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

using namespace std;

enum TokenType {
    NUMBER,
    MINUS,
    PLUS,
    MULTIPLY,
    DIVIDE,
    EXPONENT,
    WHITESPACE,
    LPAREN,
    RPAREN,
    NO_MORE_TOKENS,
    LEXICAL_ERROR
};

struct Token {
    TokenType type;
    string value;
    Token() {;}
    Token(TokenType type, string value) : type(type), value(value){;}

    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
};

class Lexer {
private:
    string input;
    string remaining;
    
public:
    Lexer(string input) : input(input) {
        remaining = input.c_str();
    }
    
    Token getNextToken(){
        Token token;
        do {
            if(remaining.size() == 0) {
                token = Token(NO_MORE_TOKENS, "");
                break;
            }
            
            smatch match;
            if (regex_search(remaining, match, regex("^([0-9]+)"))) {
                token = Token(NUMBER, match[1].str());
            } else if (regex_search(remaining, match, regex("^(\\-)"))) {
                token = Token(MINUS, match[1].str());
            } else if (regex_search(remaining, match, regex("^(\\+)"))) {
                token = Token(PLUS, match[1].str());
            } else if(regex_search(remaining, match, regex("^(\\*)"))){
                token = Token(MULTIPLY, match[1].str());
            } else if (regex_search(remaining, match, regex("^(\\/)"))) {
                token = Token(DIVIDE, match[1].str());
            } else if (regex_search(remaining, match, regex("^(\\^)"))) {
                token = Token(EXPONENT, match[1].str());
            } else if(regex_search(remaining, match, regex("^(\\s+)"))){
                token = Token(WHITESPACE, match[1].str());
                remaining = remaining.substr(token.value.size());
            } else if(regex_search(remaining, match, regex("^(\\()"))){
                token = Token(LPAREN, match[1].str());
            } else if(regex_search(remaining, match, regex("^(\\))"))){
                token = Token(RPAREN, match[1].str());
            } else {
                token = Token(LEXICAL_ERROR, match[1].str());
            }
        } while(token.type == WHITESPACE);
        
        return token;
    }

    bool getNextToken(Token& token) {
        token = getNextToken();
        return token.type != LEXICAL_ERROR;
    }

    void removeToken(Token token){
        remaining = remaining.substr(token.value.size());
    }
};

class ASTNode {
public:
    virtual int process() = 0;
    virtual string toString() = 0;
    virtual ~ASTNode() {;}
};

class ASTNodeSubtract : public ASTNode {
public:
    ASTNode* pLeft;
    ASTNode* pRight;

public:
    ASTNodeSubtract(ASTNode* pLeft, ASTNode* pRight)
        : pLeft(pLeft), pRight(pRight) {;}
    ~ASTNodeSubtract() {
        delete pLeft;
        delete pRight;
    }

    int process() override {
        return pLeft->process() - pRight->process();
    }

    string toString() override {
        string strOut = "(";
        strOut += pLeft->toString();
        strOut += "-";
        strOut += pRight->toString();
        strOut += ")";
        return strOut;
    }
};

class ASTNodeAdd : public ASTNode {
public:
    ASTNode* pLeft;
    ASTNode* pRight;

public:
    ASTNodeAdd(ASTNode* pLeft, ASTNode* pRight)
        : pLeft(pLeft), pRight(pRight) {;}
    ~ASTNodeAdd() {
        delete pLeft;
        delete pRight;
    }

    int process() override {
        return pLeft->process() + pRight->process();
    }

    string toString() override {
        string strOut = "(";
        strOut += pLeft->toString();
        strOut += "+";
        strOut += pRight->toString();
        strOut += ")";
        return strOut;
    }
};

class ASTNodeMultiply : public ASTNode {
public:
    ASTNode* pLeft;
    ASTNode* pRight;

public:
    ASTNodeMultiply(ASTNode* pLeft, ASTNode* pRight)
        : pLeft(pLeft), pRight(pRight) {;}
    ~ASTNodeMultiply() {
        delete pLeft;
        delete pRight;
    }
    
    int process() override {
        return pLeft->process() * pRight->process();
    }

    string toString() override {
        string strOut = "(";
        strOut += pLeft->toString();
        strOut += "*";
        strOut += pRight->toString();
        strOut += ")";
        return strOut;
    }
};

class ASTNodeDivide : public ASTNode {
public:
    ASTNode* pLeft;
    ASTNode* pRight;

public:
    ASTNodeDivide(ASTNode* pLeft, ASTNode* pRight)
        : pLeft(pLeft), pRight(pRight) {;}
    ~ASTNodeDivide() {
        delete pLeft;
        delete pRight;
    }
    
    int process() override {
        return pLeft->process() / pRight->process();
    }

    string toString() override {
        string strOut = "(";
        strOut += pLeft->toString();
        strOut += "/";
        strOut += pRight->toString();
        strOut += ")";
        return strOut;
    }
};

class ASTNodeExponent : public ASTNode {
public:
    ASTNode* pLeft;
    ASTNode* pRight;

public:
    ASTNodeExponent(ASTNode* pLeft, ASTNode* pRight)
        : pLeft(pLeft), pRight(pRight) {;}
    ~ASTNodeExponent() {
        delete pLeft;
        delete pRight;
    }
    
    int process() override {
        return pow(pLeft->process(), pRight->process());
    }

    string toString() override {
        string strOut = "(";
        strOut += pLeft->toString();
        strOut += "^";
        strOut += pRight->toString();
        strOut += ")";
        return strOut;
    }
};

class ASTNodeNumber : public ASTNode {
private:
    int number;

public:
    ASTNodeNumber(int number) : number(number) {;}
    int process() override {
        return number;
    }

    string toString() override {
        return std::to_string(number);
    }
};

class ASTParser {
private:
    ASTNode* expr(Lexer& lexer) {
        ASTNode* pLeft = term(lexer);
        Token token;
        while(lexer.getNextToken(token) && (token.type == MINUS || token.type == PLUS)) {
            lexer.removeToken(token);
            ASTNode* pRight = term(lexer);
            if(token.type == MINUS){
                pLeft = new ASTNodeSubtract(pLeft, pRight);
            } else {
                pLeft = new ASTNodeAdd(pLeft, pRight);
            }
        }
        return pLeft;
    }
    
    ASTNode* term(Lexer& lexer){
        ASTNode* pLeft = exponent(lexer);
        Token token;
        
        while(lexer.getNextToken(token) && (token.type == MULTIPLY || token.type == DIVIDE)) {
            lexer.removeToken(token);
            ASTNode* pRight = exponent(lexer);
            if(token.type == MULTIPLY){
                pLeft = new ASTNodeMultiply(pLeft, pRight);
            } else {
                pLeft = new ASTNodeDivide(pLeft, pRight);
            }
        }
        return pLeft;
    }
    
    ASTNode* exponent(Lexer& lexer){
        ASTNode* pLeft = factor(lexer);
        Token token;
        
        while(lexer.getNextToken(token) && token.type == EXPONENT) {
            lexer.removeToken(token);
            ASTNode* pRight = exponent(lexer);
            
            pLeft = new ASTNodeExponent(pLeft, pRight);
        }
        return pLeft;
    }

    ASTNode* factor(Lexer& lexer) {
        Token token;
        lexer.getNextToken(token);
        ASTNode* pNode;
        if (token.type == NUMBER) {
            lexer.removeToken(token);
            pNode = new ASTNodeNumber(stoi(token.value));
        }
        else if(token.type == LPAREN){
            lexer.removeToken(token);
            pNode = expr(lexer);
            lexer.getNextToken(token);
            if(token.type!= RPAREN){
                throw new std::runtime_error("Missing closing parenthesis");
            }
            lexer.removeToken(token);
        } else {
                throw new std::runtime_error("Parse error");
            }
        return pNode;
    }

public:
    ASTParser() {;};
    ASTNode* parse(string statement){
        Lexer lexer(statement);
        return expr(lexer);
    }
};

class EvalParser {
private:
    int expr(Lexer& lexer) {
        int left = term(lexer);
        Token token;
        while(lexer.getNextToken(token) && (token.type == MINUS || token.type == PLUS)) {
            lexer.removeToken(token);
            int right = term(lexer);
            if(token.type == MINUS){
                left = left - right;
            } else {
                left = left + right;
            }
        }
        return left;
    }
    
    int term(Lexer& lexer){
        int left = exponent(lexer);
        Token token;
        while(lexer.getNextToken(token) && (token.type == MULTIPLY || token.type == DIVIDE)) {
            lexer.removeToken(token);
            int right = exponent(lexer);
            if(token.type == MULTIPLY){
                left = left * right;
            } else {
                left = left / right;
            }
        }
        return left;
    }
    
    int exponent(Lexer& lexer){
        int left = factor(lexer);
        Token token;
        while(lexer.getNextToken(token) && token.type == EXPONENT) {
            lexer.removeToken(token);
            int right = exponent(lexer);
            left = pow(left, right);
        }
        return left;
    }

    int factor(Lexer& lexer) {
        Token token;
        lexer.getNextToken(token);
        int ret;
        if (token.type == NUMBER) {
            lexer.removeToken(token);
            ret = stoi(token.value);
        }
        else if(token.type == LPAREN){
            lexer.removeToken(token);
            ret = expr(lexer);
            lexer.getNextToken(token);
            if(token.type != RPAREN){
                throw new std::runtime_error("Missing closing parenthesis");
            }
            lexer.removeToken(token);
        } else {
            throw new std::runtime_error("parse error");
        }
        return ret;
    }
    
public:
    EvalParser()    {;};
    int parse(string statement){
        Lexer lexer(statement);
        return expr(lexer);
    }
};

void tests(){
    ASTParser parser;
    vector<string> testStatements = {
        "2+2", "(2-3-4)", "2+3*4",  // 4, -5, 14
        "((2+3)*(4+1))/5", "6/2*3", "2+3+4+5+6/2",  // 5, 9, 17
        "2^2^3", "((3+4)^2)", "3^(9-7)-8" // 256, 49, 1
    };
    vector<int> expected = {
        4, -5, 14,
        5, 9, 17,
        256, 49, 1
    };
    
    int i = 0;
    bool allTestPassed = true;
    for(string stmt : testStatements) {
        cout << "TESTING: " << stmt << endl;
        EvalParser evalParser;
        int result = evalParser.parse(stmt);
        cout << "Eval parser result: " << result << "\n";
        
        ASTNode* ast = parser.parse(stmt);
        string strTree = ast->toString();
        cout << "AST: " << strTree << "\n";
        
        int val = ast->process();
        cout << "stmt: " << stmt << " result: " << val << "\n";
        
        delete ast;
        
        if(expected[i] == val){
            cout << "TEST PASSED! Expected: " << expected[i] << ", returned: " << val << "\n";
        } else {
            cout << "TEST FAILED! Expected: " << expected[i] << " , returned: " << val << "\n";
            allTestPassed = false;
        }
        cout << "\n";
        i++;
        
    }
    cout << "TESTING COMPLETE\n";
    if(allTestPassed == true){
        cout << "PASSED ALL TEST CASES\n";
    }
    else {
        cout << "DID NOT PASS ALL TESTS\n";
    }
}

int main() {
    tests();
    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

class Error
{
    string msg;

public:
    Error(string message) : msg(message) {}

    string what()
    {
        return msg;
    }
};

class Token
{
public:
    enum Kind
    {
        none = 0,
        id,
        num,
        txt,
        plus,
        minus,
        mul,
        div,
        mod,
        exclimation,
        question,
        colon,
        semicolon,
        eos,
        eof,
        newline,
        r_par,
        l_par,
        l_cur,
        r_cur,
        l_ang,
        r_ang
    };
    Kind kind = none;
    double value = 0.0;
    string text = "";
    Token() {}

    friend ostream &operator<<(ostream &os, const Token &t);

    Token(double d) : value(d), kind(num) {}

    Token(Kind k) : kind(k) {}

    Token(string t) : text(t), kind(txt) {}
};

ostream &operator<<(ostream &os, const Token &t)
{
    switch (t.kind)
    {
    case Token::none:
        os << "<none>";
        break;
    case Token::plus:
        os << "+";
        break;
    case Token::minus:
        os << "-";
        break;
    case Token::div:
        os << "/";
        break;
    case Token::mul:
        os << "*";
        break;
    case Token::mod:
        os << "%";
        break;
    default:
        os << "Some lexeme...";
    }
    return os;
}

class TokenStream
{
    vector<Token> buffer;

public:
    void putback(Token &t) { buffer.push_back(t); }

    Token get()
    {
        if (buffer.size() > 0)
        {
            Token t = buffer.back();
            buffer.pop_back();
            return t;
        }

        char ch;
        cin.get(ch);

        switch (ch)
        {

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
        {
            cin.putback(ch);
            double d;
            cin >> d;
            return Token(d);
        }
        case '+':
            return Token(Token::plus);
        case '-':
            return Token(Token::minus);
        case '/':
            return Token(Token::div);
        case '*':
            return Token(Token::mul);
        case '%':
            return Token(Token::mod);
        case '(':
            return Token(Token::l_par);
        case ')':
            return Token(Token::r_par);
        case ';':
            return Token(Token::semicolon);
        case '\n':
            return Token(Token::newline);
        case '\0':
            return Token(Token::eos);
        default:
            throw Error("Lexer error: Unknown lexeme.");
        }
    }
};

TokenStream ts;

double expression();

double primary()
{
    Token t = ts.get();

    switch (t.kind)
    {
    case Token::num:
        return t.value;
    case Token::plus:
        return +primary();
    case Token::minus:
        return -primary();
    case Token::l_par:
    {
        double exp = expression();
        Token rb = ts.get();
        if (rb.kind != Token::r_par)
            throw Error("Primary error: closing round bracket expected.");
        return exp;
    }
    default:
        throw Error("Primary error: " + t.kind);
    }
}

double term()
{
    double left = primary();
    Token t = ts.get();

    while (true)
        switch (t.kind)
        {
        case Token::mul:
            left *= primary();
            t = ts.get();
            break;

        case Token::div:
        {
            double right = primary();
            if (right == 0)
            {
                return numeric_limits<double>::infinity();
                //throw Error("Division by zero.");
            }
            else
            {
                left /= right;
                t = ts.get();
                break;
            }
        }

        default:
            ts.putback(t);
            return left;
        }
}

double expression()
{
    double left = term();
    Token t = ts.get();

    while (true)
        switch (t.kind)
        {
        case Token::plus:
            left += term();
            t = ts.get();
            break;

        case Token::minus:
            left -= term();
            t = ts.get();
            break;

        default:
            ts.putback(t);
            return left;
        }
}

int main()
{

    cout << "> ";
    while (true)
        try
        {

            Token t = ts.get();
            while (t.kind == Token::semicolon || t.kind == Token::newline || t.kind == Token::eos)
            {
                t = ts.get();
            }

            ts.putback(t);

            double res = expression();

            cout << ": " << res << endl;
            cout << "> ";
        }
        catch (Error &e)
        {
            cerr << endl
                 << "Catch: " << e.what() << endl;
            break;
        };
    return 0;
}

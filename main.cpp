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
    str,
    plus,
    minus,
    asterisk,
    div,
    mod,
    exclimation_mark,
    question_mark,
    colon,
    semicolon,
    bracket_round_left,
    bracket_round_right,
    bracket_curly_left,
    bracket_curly_right,
    bracket_angle_left,
    bracket_angle_right,
    end_of_string,
    end_of_line,
    end_of_file
  };
  Kind kind = none;
  double value = 0.0;
  string text = "";

  Token() {}
  Token(double d) : value(d), kind(num) {}
  Token(Kind k) : kind(k) {}
  Token(string t) : text(t), kind(str) {}

  friend ostream &operator<<(ostream &out_stream, const Token &t);
};

ostream &operator<<(ostream &out, const Token &t)
{
  switch (t.kind)
  {
  case Token::none:
    out << "<none>";
    break;
  case Token::plus:
    out << "+";
    break;
  case Token::minus:
    out << "-";
    break;
  case Token::div:
    out << "/";
    break;
  case Token::asterisk:
    out << "*";
    break;
  case Token::mod:
    out << "%";
    break;
  default:
    out << "Some lexeme...";
  }
  return out;
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
      return Token(Token::asterisk);
    case '%':
      return Token(Token::mod);
    case '(':
      return Token(Token::bracket_round_left);
    case ')':
      return Token(Token::bracket_round_right);
    case ';':
      return Token(Token::semicolon);
    case '\n':
      return Token(Token::end_of_line);
    case '\0':
      return Token(Token::end_of_string);
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
  case Token::bracket_round_left:
  {
    double expr = expression();
    Token rb = ts.get();
    if (rb.kind != Token::bracket_round_right)
      throw Error("Primary error: closing round bracket expected.");
    return expr;
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
    case Token::asterisk:
      left *= primary();
      t = ts.get();
      break;

    case Token::div:
    {
      double right = primary();
      if (right == 0)
      {
        return numeric_limits<double>::infinity();
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
      while (t.kind == Token::semicolon || t.kind == Token::end_of_line || t.kind == Token::end_of_string)
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

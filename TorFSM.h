#pragma once

//#include <ams/torparser.h>

// This class implements a lexical analyzer in the form of a state machine for analyzing
// .torrent files. The torrent file format is called bencoding.
// See http://en.wikipedia.org/wiki/Bencode

#include <stack>
// Note boost.function provides a "Functor" type without templates:
#include <boost/function.hpp>


namespace torrent
{

#define LEXERROR(e) throw LexicalError(e, __LINE__, __FILE__, __FUNCTION__)
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))


class LexicalError {
public:
    enum Errors {unexpectedSymbol};
    LexicalError(Errors e, int line = 0, char* file="", char*function = ""):
    e_(e), line_(line), file_(file), function_(function) {};


    template <class T> Report(T& str)
    {
        char *s = "unknown error";
        if (e_ == unexpectedSymbol)
        {
            s = "unexpected symbol";
        }

        str << "Error: " << s << " occurred at line "
            << line_ << " in file " << file_ << " in function " << function_;
    }
private:
    Errors e_;
    int line_;
    char* file_;
    char* function_;

};


class TorFSM : public boost::noncopyable
{
    typedef void (TorFSM::*State)(char);

    // Functions that received tokens to form a parser:
    typedef boost::function<void (std::string)> ParseSymbolString;     // see boost.org
    typedef boost::function<void (int)> ParseSymbolInt;                // see boost.org
    typedef boost::function<void (char)> ParseSymbol;                  // see boost.org
    typedef std::deque<State> Stacktype;

    // Functions or function objects. These are initialised in the constructor:
    ParseSymbol parseSymbol_;
    ParseSymbolInt parseSymbolInt_;
    ParseSymbolString parseSymbolString_;

public:
    TorFSM(ParseSymbol parseSymbol, ParseSymbolInt parseSymbolInt, ParseSymbolString parseSymbolString) : 
           parseSymbol_(parseSymbol),
           parseSymbolInt_(parseSymbolInt),
           parseSymbolString_(parseSymbolString),
           dlevel_(0),
           llevel_(0),
           state_(Idle)
    {
    }

    void Idle(char c)
    {
        if (!StandardTest(c))
        {
            LEXERROR(LexicalError::unexpectedSymbol);
        }
    }

    void Dictionary(char c)
    {
        if (!StandardTest(c))
        {
            if (c == 'e')
            {
                parseSymbol_('e');
                Unstack();
            }
            else
            {
                LEXERROR(LexicalError::unexpectedSymbol);
            }
        }
    }

    void Integer(char c)
    {
        if (isdigit(c))
        {
            iresult_ = iresult_ * 10 + c - '0';
        }
        else if (c == 'e')
        {
//            intfunc_(iresult_);
            parseSymbolInt_(iresult_);
            Unstack();
        }
    }

    void List(char c)
    {
        if (!StandardTest(c))
        {
            if (c == 'e')
            {
                parseSymbol_('e'); // m is end of list
                Unstack();
            }
            else
            {
                LEXERROR(LexicalError::unexpectedSymbol);
            }
        }
    }

    void String(char c)
    {
        sresultLength_ --;
        sresult_ += c;

        if (sresultLength_ == 0)
        {
            parseSymbolString_(sresult_);
            Unstack();
        }
    }

    void WaitingForColon(char c)
    {
        if (c == ':')
        {
            if (sresultLength_ > 0)
            {
                sresult_ = "";
                state_ = String;
            }
            else
            {
                // Ignore zero length strings:
                Unstack();
            }
        }
        else if (isdigit(c))
        {
            sresultLength_ = 10 * sresultLength_ + c - '0';
        }
    }

    bool StandardTest(char c)
    {
        bool result = true;
        if (c == 'i')
        {
            statestack_.push_back(state_);
            iresult_ = 0;
            state_ = Integer;
        }
        else if (c == 'd')
        {
            statestack_.push_back(state_);
            dlevel_++;
            state_ = Dictionary;
            parseSymbol_('d');
        }
        else if (c == 'l')
        {   
            statestack_.push_back(state_);
            llevel_++;
            state_= List;
            parseSymbol_('l');
        }
        else if (isdigit(c))
        {
            statestack_.push_back(state_);
            sresultLength_ = c - '0';
            state_= WaitingForColon;
        }
        else
        {
            result = false;
        }
        return result;
    }


    // Function to remove two states from the stack in the case of dictionary/dvalue
    // at the top of the stack, or just one state otherwise:
    void Unstack()
    {
        state_ = statestack_.back();
        statestack_.pop_back();
    }


    // Crank the state machine round:
    template <class it> void Crank(it begin, it end)
    {
        // DEBUG
        std::string ss = std::string(begin, end);
        while (begin != end)
        {
            // DEBUG:
            State pp = state_;
            char c = *begin;

            CALL_MEMBER_FN(*this, state_)(*begin);
            begin++;
        }
    }

private:
    State state_;
    Stacktype statestack_; // we can't use std::stack as we need access to more than just the top
    std::deque<char> parseStack_;
    int dlevel_, llevel_;
    int iresult_;
    std::string sresult_;
    int sresultLength_;
};

} // namespace torrent
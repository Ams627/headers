#pragma once
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace torrent
{
#if 0
}
#endif

struct ParseStack
{
    struct StackEntry
    {
        char symbol;
        int i;

        StackEntry(char symbol) : symbol(symbol){}
        StackEntry(char symbol, int i) : symbol(symbol), i(i){}
    };
    std::vector<std::string> strings_;

    typedef  std::deque<StackEntry> Stacktype;

    Stacktype stack_;

    void Push(char symbol)
    {
        stack_.push_back(StackEntry(symbol));
    }

    void PushString(const string& s)
    {
        strings_.push_back(s);
        stack_.push_back(StackEntry('s', static_cast<int>(strings_.size() - 1)));
    }

    void PushInt(int i)
    {
        stack_.push_back(StackEntry('i', i));
    }

    void Pop()
    {
        stack_.pop_back();
    }

    char Top()
    {
        return stack_.back().symbol;
    }

    int TopInt()
    {
        return stack_.back().i;
    }

    std::string TopString()
    {
        if (stack_.back().i < strings_.size())
        {
            return strings_[stack_.back().i];
        }
        else
        {
            return "";
        }
    }

    bool TopIs(char symbol)
    {
        return (!stack_.empty() && stack_.back().symbol == symbol);
    }

    void SetTop(char symbol)
    {
        stack_.back().symbol = symbol;
    }

    bool IsEmpty()
    {
        return stack_.empty();
    }

    bool operator==(std::string s)
    {
        bool result;

        if (stack_.empty() || stack_.size() != s.size())
        {
            result = false;
        }
        else
        {
            std::string::iterator i = s.begin();
            Stacktype::iterator j = stack_.begin();
            result = true;
            while (i != s.end() && result)
            {
                if (*i != j->symbol)
                {
                    result = false;
                }
                i++;j++;
            }
        }
        return result;
    }

};


struct Parse //: public boost::noncopyable
{
    ParseStack stack_;
    std::vector<std::string> paths;
    std::string path;
    typedef void (Parse::*State)(char);
    State state_;
    int level_;

    Parse() {
        state_ = StateIdle;
        level_ = 0;
    }

    void SetLevel(char symbol)
    {
        if (symbol == 'd' || symbol == 'l')
        {
            level_ ++;
        }
        else if (symbol == 'e')
        {
            level_ --;
        }
        else
        {
            cerr << "error: symbol unexpected " << symbol << endl;
        }
    }

    void StateIdle(char symbol)
    {
        if (level_ == 0 && symbol == 'd')
        {
            state_ = StateD;
        }
        else 
        {
            SetLevel(symbol);
        }
    }

    void StateD(char symbol)
    {
        // we have found the file level dictionary, now we're expecting the info dictionary:
        if (level_ == 0)
        {
            if (symbol == 'd' && stack_.TopString() == "info")
            {
                state_ = StateDD;
            }
            else if (symbol == 'e')
            {
                state_ = StateIdle;
            }
            else
            {
                SetLevel(symbol);
            }
        }
        else
        {
            SetLevel(symbol);
        }
    }

    void StateDD(char symbol)
    {
        // In this state we have detected info=dictionary(...
        // we're now expecting files=list(... for the multifile case
        if (level_ == 0)
        {
            if (symbol == 'l' && stack_.TopString() == "files")
            {
                state_ = StateDDL;
            }
            else if (symbol == 'e')
            {
                state_ = StateD;
            }
            else
            {
                SetLevel(symbol);
            }
        }
        else
        {
            SetLevel(symbol);
        }
    }

    void StateDDL(char symbol)
    {
        // In this state we have detected "files=list(..." - we're now expecting a number
        // of dictionaries, one for each file:
        if (level_ == 0)
        {
            if (symbol == 'd')
            {
                state_ = StateDDLD;
            }
            else if (symbol == 'e')
            {
                state_ = StateDD;
            }
            else
            {
                SetLevel(symbol);
            }
        }
        else
        {
            SetLevel(symbol);
        }
    }

    void StateDDLD(char symbol)
    {
        // in this state we have detected a per-file dictionary
        // next we should detect a list of directory components that make up the files path
        if (level_ == 0)
        {
            if (symbol == 'l' && stack_.TopString() == "path")
            {
                state_ = StateDDLDL;
            }
            else if (symbol == 'e')
            {
                state_ = StateDDL;
            }
            else
            {
                SetLevel(symbol);
            }
        }
        else
        {
            SetLevel(symbol);
        }
    }

    void StateDDLDL(char symbol)
    {
        if (level_ == 0 && symbol == 'e')
        {
            state_ = StateDDLD;
        }
        else
        {
            SetLevel(symbol);
        }
    }

    void operator()(char symbol)
    {
        // DEBUG:
        char bol='*';
        if(!stack_.IsEmpty()) bol = stack_.Top();

        if ((symbol == 'd' || symbol == 'l') && stack_.TopIs('d'))
        {
            cout << "ERROR: dictionary key must be a string\n";
        }

        if (symbol == 'd')
        {
            if (stack_.TopIs('s'))
            {
                // top of stack holds the key, with a dictionary value:
                if (stack_ == "ds")
                {
                    // remove the string and replace with a dictionary indicator:
                    stack_.SetTop('d');
                }
            }
            else
            {
                stack_.Push('d');
            }
        }
        else if (symbol == 'l')
        {
            if (stack_.TopIs('s'))
            {
                // top of stack holds the key, with a list value:
                // remove the key string from the stack and replace with a list indicator:
                stack_.SetTop('l');
            }
            else
            {
                stack_.Push('l');
            }
        }
        else if (symbol == 'e') // end of dictionary or list
        {
            if (stack_.IsEmpty())
            {
                cout << "end of list or dictionary without start\n";
            }
            else
            {
                stack_.Pop();
            }
        }

        // Crank the state machine round:
        CALL_MEMBER_FN(*this, state_)(symbol);
    }

    void operator()(int i)
    {
        if (stack_.IsEmpty())
        {
            cout << "Warning: Free integer (not in dictionary) detected:";
            cout << i << endl;
        }
        else
        {
            if (stack_.TopIs('d'))
            {
                cout << "Error: integer as dictionary key (string expected)\n";
            }
            else if (stack_.TopIs('s'))
            {
                if (state_ == StateD && stack_.TopString() == "creation date")
                {
                    cout << "date is " << i << endl;
                }
                else if (state_ == StateDDLD && stack_.TopString() == "length")
                {
                    cout << "length is " << i << endl;
                }
                stack_.Pop();
            }
        }
    }

    void operator()(string s)
    {
//        FiddleString(s);

        if (stack_.IsEmpty())
        {
            cout << "Warning: Free string (not in dictionary) detected:";
            cout << s << endl;
        }
        else
        {
            if (stack_.TopIs('d'))
            {
                stack_.PushString(s);
            }
            else if (stack_.TopIs('s')) // keyvalue entry - string = string
            {
                if (state_ == StateD && stack_.TopString() == "announce")
                {
                    cout << "announce = " << s << endl;
                }
                stack_.Pop();
            }
            else if (stack_.TopIs('l')) // string entry in a list
            {
                if (state_ == StateDDLDL)
                {
                    cout << "path component " << s << endl;
                }
            }
        }
    }
};

}
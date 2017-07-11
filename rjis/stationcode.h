#pragma once

class RJISError
{
    std::string message;
    std::string filename;
    int line;
public:
    RJISError(std::string message, std::string filename, int line) : message(message), filename(filename), line(line) {}
};

#pragma pack(push, 1)
union UNLC
{
    std::array<char, 4> aCode;
	friend std::ostream& operator<<(std::ostream &str, const UNLC& unlc);
    uint32_t uCode;

    size_t GetSize() const { return aCode.size(); }
    std::string GetString() const { return std::string(aCode.rbegin(), aCode.rend()); }

    char& operator[](size_t n) { return aCode[n]; }
    inline bool operator<(const UNLC& other) const {return uCode < other.uCode; }
    inline operator uint32_t() { return uCode; };
    UNLC() = default;
    UNLC(uint32_t u) : uCode(u) {}
	UNLC(std::string s, size_t offset = 0) { for (size_t i = 0; i < aCode.size(); ++i){ aCode[aCode.size() - i - 1] = s[i + offset]; } }
	UNLC(char *s) { for (size_t i = 0; i < aCode.size(); ++i){ aCode[aCode.size() - i - 1] = s[i]; } }
    void Set(const std::string& str, size_t offset = 0) { for (size_t i = 0; i < aCode.size(); ++i) { aCode[aCode.size() - i - 1] = str[i + offset]; } }
    void SetCountyCode(const std::string s, size_t offset) { aCode[3] = aCode[2] = 'C'; aCode[1] = s[offset]; aCode[0] = s[offset + 1]; }
    void SwapEndian() { auto a = aCode[0]; aCode[0] = aCode[3]; aCode[3] = a; a = aCode[1]; aCode[1] = aCode[2]; aCode[2] = a; }
    UNLC& operator=(std::string &s) { for (size_t i = 0; i < aCode.size(); ++i){ aCode[aCode.size() - i - 1] = s[i]; } return *this; }
	bool operator==(std::string s)
	{
		bool result;
		if (s.length() != 4)
		{
			result = false;
		} 
		else
		{ 
			UNLC other(s);
			result = this->uCode == other.uCode;
		}
		return result;
	}
	bool operator!=(std::string s) {
		return !(*this == s);
	}
	bool operator==(const UNLC& other) const
	{
		return uCode == other.uCode;
	}
	bool operator!=(const UNLC& other) const
	{
		return uCode != other.uCode;
	}

};

inline std::ostream& operator<<(std::ostream &str, const UNLC& unlc)
{
	for (auto p = unlc.aCode.crbegin(); p != unlc.aCode.crend(); ++p)
	{
		str.put(*p);
	}
	return str;
}

union UFlow
{
    std::array<char, 8> aFlow;
    uint64_t u64Flow;
    struct {
        uint32_t a;
        uint32_t b;
    } u32Flow;
    struct {
        UNLC destination;
        UNLC origin;
    };
    UFlow() = default;
    UFlow(uint64_t u) : u64Flow(u) {}
    UFlow(std::string s, size_t offset = 0) { Set(s, offset); }
    UFlow(UNLC o, UNLC d) { origin = o; destination = d; }
    inline size_t GetSize() const { return aFlow.size(); }
    inline std::string GetString() const { return std::string(aFlow.rbegin(), aFlow.rend()); }

    std::string GetJSON() const
    {
        return std::string("\"orig\": \"") + origin.GetString() + "\", \"dest\": \"" + destination.GetString() + "\"";
    }

    inline char& operator[](size_t n)  { return aFlow[n]; }
    inline bool operator<(const UFlow& other) const { return u64Flow < other.u64Flow; }
	inline bool operator==(const UFlow& other) const { return u64Flow == other.u64Flow; }

    bool operator==(std::string s)
    {
        bool result;
        if (s.length() != 8)
        {
            result = false;
        }
        else
        {
            UFlow other(s);
            result = this->u64Flow == other.u64Flow;
        }
        return result;
    }



//    inline operator uint64_t() { return u64Flow; };
    void Reverse() { uint32_t t = u32Flow.a; u32Flow.a = u32Flow.b; u32Flow.b = t; }
    void Set(const std::string& str, size_t offset) { for (size_t i = 0; i < aFlow.size(); ++i) {
        aFlow[aFlow.size() - i - 1] = str[i + offset]; } }

	friend std::ostream& operator<<(std::ostream& str, const UFlow& flow);
};
#pragma pack(pop)   

inline std::ostream& operator<<(std::ostream& str, const UFlow& flow) { str << flow.origin << flow.destination; return str; }


class StationCode
{
    UNLC code;
public:
    StationCode() {}
    StationCode(const std::string& s)
    {
        for (size_t i = 0; i < code.GetSize(); ++i)
        {
            code[i] = s[i];
        }
    }
    StationCode(const char*p)
    {
        for (size_t i = 0; i < code.GetSize(); ++i)
        {
            code[i] = p[i];
        }
    }

    inline size_t GetSize() const { return code.GetSize(); }
    operator uint32_t() const { return code.uCode; }

    template <class T> StationCode(T& t, size_t n)
    {
        std::copy(t.begin() + n, t.begin() + code.GetSize() + n, code.aCode.begin());
    }

    //friend bool operator<(const StationCode& c1, const StationCode& c2)
    //{
    //    return c1 < c2;
    //}

    //friend bool operator==(const StationCode& c1, const StationCode& c2)
    //{
    //    bool result = true;
    //    size_t i = 0;
    //    while (i < c1.code.size() && result)
    //    {
    //        result = c1.code[i] == c2.code[i];
    //        i++;
    //    }
    //    return result;
    //}

    //friend bool operator!=(const StationCode& c1, const StationCode& c2)
    //{
    //    return !(c1 == c2);
    //}

    char operator[](size_t i)
    {
        return code[i];
    }

    void SetInvalid()
    {
        code.uCode = -1;
    }

    bool IsInvalid() const
    {
        return code.uCode == -1;
    }

    //bool operator<(const StationCode& c) const
    //{
    //    int result = 0;
    //    size_t i = 0;
    //    while (i < code.size() && result)
    //    {
    //        result = code[i] - c.code[i];
    //        if (!result)
    //        {
    //            i++;
    //        }
    //    }
    //    if (result)
    //    {
    //        result = !result;
    //    }
    //    else
    //    {
    //        result = code[i] < c.code[i];
    //    }
    //    return result;
    //}

    char *GetData() { return code.aCode.data(); }
    std::string GetString() { return std::string(code.aCode.begin(), code.aCode.end()); }
    friend std::ostream& operator<<(std::ostream& stream, StationCode sc)
    {
        for (auto c: sc.code.aCode)
        {
            stream.put(c);
        }
        return stream;
    }
};

class RJISFlow
{
public:
    UFlow flow_;
    RJISFlow(std::string s)
    {
        assert(s.length() >= sizeof(flow_)); // there must be a minimum of eight chars
        for (size_t i = 0; i <  flow_.GetSize(); ++i)
        {
            assert(isupper(s[i]) || isdigit(s[i])); // flow chars must be upper case or digit
            flow_[i] = s[i];
        }
    }

    RJISFlow(std::string s, size_t offset)
    {
        assert(s.size() - offset >= flow_.GetSize()); // there must be enough characters from the offset to the end
        for (size_t i = 0; i < flow_.GetSize(); i++)
        {
            assert(isupper(s[i + offset]) || isdigit(s[i + offset])); // flow chars must be upper case or digit
            flow_[i] = s[i + offset];
        }
    }

    RJISFlow(char *p)
    {
        for (size_t i = 0; i < flow_.GetSize(); ++i)
        {
            assert(p[i] != 0); // no null terminators allowed in a char* string
            flow_[i] = p[i];
        }
    }

    RJISFlow(const StationCode & s1, const StationCode &s2)
    {
        flow_.u32Flow.a = s1;
        flow_.u32Flow.b = s2;
    }

    std::string GetOrigin()
    {
        std::string result(flow_.GetSize() / 2, ' ');
        for (size_t i = 0; i < flow_.GetSize() / 2; ++i)
        {
            result[i] = flow_[i];
        }
        return result;
    }

    std::string GetDestination()
    {
        std::string result(flow_.GetSize() / 2, ' ');
        for (size_t i = 0; i < flow_.GetSize() / 2; ++i)
        {
            result[i] = flow_[i + flow_.GetSize() / 2];
        }
        return result;
    }

    void Reverse()
    {
        flow_.Reverse();
    }

    friend bool operator<(const RJISFlow & f1, const RJISFlow& f2)
    {
        return f1.flow_.u64Flow < f2.flow_.u64Flow;
    }
};

template <int L> class RJISGeneral
{
    std::array<char, L> type;
public:
    RJISGeneral(const std::string& s, size_t offset)
    {
        assert(s.size() - offset >= L); // there must be enough characters from the offset to the end
        for (size_t i = 0; i < L; i++)
        {
            type[i] = s[i + offset];
        }
    }
    RJISGeneral(char *p)
    {
        for (size_t i = 0; i < L; i++)
        {
            assert(p[i] != 0); // null terminator not allowed in string
            type[i] = p[i];
        }
    }
    std::string GetString()
    {
        std::string result;
        for (auto p : type)
        {
            result.push_back(p);
        }
        return result;
    }
};

inline int __fastcall GetNum(const std::string s, size_t offset, size_t length)
{
    assert(offset + length <= s.length());
    int result = 0;
    for (size_t i = 0; i < length; ++i)
    {
        result = result * 10 + s[i + offset] - '0';
    }
    return result;
}

//struct RJISTicketType
//{
//    std::array<char, 3> type;
//    RJISTicketType(std::string s, size_t offset)
//    {
//        assert(s.size() - offset >= type.size()); // there must be enough characters from the offset to the end
//        for (size_t i = 0; i < type.size(); i++)
//        {
//            type[i] = s[i];
//        }
//    }
//    RJISTicketType(char *p)
//    {
//        for (size_t i = 0; i < type.size(); i++)
//        {
//            assert(p[i] != 0); // null terminator not allowed in string
//            type[i] = p[i];
//        }
//    }
//};
//
//struct RJISTicketType
//{
//    std::array<char, 3> type;
//    RJISTicketType(std::string s, size_t offset)
//    {
//        assert(s.size() - offset >= type.size()); // there must be enough characters from the offset to the end
//        for (size_t i = 0; i < type.size(); i++)
//        {
//            type[i] = s[i];
//        }
//    }
//    RJISTicketType(char *p)
//    {
//        for (size_t i = 0; i < type.size(); i++)
//        {
//            assert(p[i] != 0); // null terminator not allowed in string
//            type[i] = p[i];
//        }
//    }
//};

typedef RJISGeneral<3> RJISTicketType;
typedef RJISGeneral<2> RJISRestrictionCode;

// a fare record from the flow file:
class RJISFare
{
    int fare; // fare in pence
    RJISTicketType ticketType;
    RJISRestrictionCode restrictionCode;
};


// given a list of origins and destinations, form the list of all possible combinations of (origin, destination) where
// origin and destination are not equal
template <class Cont1, class Cont2, class Cont3> void PermuteNLCs(Cont1& flows, const Cont2& origins, const Cont3& destinations)
{
    for (auto p : origins)
    {
        for (auto q : destinations)
        {
			if (p != q)
			{
				flows.push_back(Cont1::value_type(p, q));
			}
        }
    }
}
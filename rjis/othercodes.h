#pragma once

template <size_t N, bool space = false> class AlNum
{
    std::array<char, N> data_;

	friend std::ostream& operator<<(std::ostream &str, const AlNum<N, space>& alnum)
	{
		for (auto i = 0; i < N; i++)
		{
			str.put(alnum.data_[i]);
		}
		return str;
	}

public:
    AlNum() = default;
    AlNum(const char* s)
    {
        Set(s, 0);
    }
    AlNum(const std::string& str, size_t offset = 0)
    {
        Set(str, offset);
    }
    void Set(const std::string& str, size_t offset)
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            data_[i] = str[offset + i];
        }
    }

    void Clear()
    {
        for (auto& p : data_)
        {
            p = 0;
        }
    }

    bool operator<(const AlNum<N, space>& other) const
    {
        int result = 0;
        for (size_t i = 0; i < N && result == 0; ++i)
        {
            if (data_[i] != other.data_[i])
            {
                result = data_[i] - other.data_[i];
            }
        }
        return result < 0;
    }

    bool IsEmpty() const
    {
        uint8_t orResult = 0;
        for (const auto& p : data_)
        {
            orResult |= static_cast<uint8_t>(p);
        }
        return orResult == 0;
    }

	bool operator==(const AlNum<N, space>& other) const
	{
		return (data_ == other.data_);
	}

	bool operator==(const std::string s) const
	{
		bool result;
		if (s.length() != N)
		{
			result = false;
		}
		else
		{
			result = true;
			for (size_t i = 0; i < N && result; ++i)
			{
				if (data_[i] != s[i])
				{
					result = false;
				}
			}
		}
		return result;
	}

	bool operator!=(const std::string s) const
	{
		return !(*this == s);
	}

    char operator[](size_t n)
    {
        return data_[n];
    }
    std::string GetString() const { return std::string(data_.begin(), data_.end()); }
//    operator LPCSTR() const { return data_.data(); }
};

// template <size_t N, bool space = false> std::ostream& operator<<(std::ostream &str, const AlNum<N, space>& alnum)
// {
// 	for (auto i = 0; i < N; i++)
// 	{
// 		str.put(alnum.data_[i]);
// 	}
// 	return str;
// }

template <size_t N> class FixedInt
{
    uint32_t n;
    friend std::ostream& operator<<(std::ostream& str, FixedInt p)
    {
        str << p.n;
        return str;
    }

public:
    FixedInt() = default;
    FixedInt(int n) : n(n) {}
    FixedInt(std::string s, size_t offset = 0)
    {
        Set(s, offset);
    }
    inline void Set(std::string s, size_t offset = 0)
    {
        int result = 0;
        for (auto i = 0U; i < N; ++i)
        {
            if (!isdigit(s[i + offset]))
            {
                throw QException("digit expected in FixedInt.");
            }
            result = 10 * result + s[i + offset] - '0';
        }
        n = result;
    }
    bool operator<(const FixedInt<N>& other) const
    {
        return n < other.n;
    }
    operator uint32_t() const { return n; }
};

template <size_t N> class FixedShort
{
    uint16_t n;
    friend std::ostream& operator<<(std::ostream& str, FixedShort p)
    {
        str << p.n;
        return str;
    }

public:
    FixedShort() = default;
    FixedShort(int n) : n(n) {}
    FixedShort(std::string s, size_t offset = 0)
    {
        Set(s, offset);
    }
    inline void Set(std::string s, size_t offset = 0)
    {
        int result = 0;
        for (auto i = 0U; i < N; ++i)
        {
            if (!isdigit(s[i + offset]))
            {
                throw QException("digit expected in FixedInt.");
            }
            result = 10 * result + s[i + offset] - '0';
        }
        n = result;
    }
    bool operator<(const FixedShort<N>& other) const
    {
        return n < other.n;
    }
    operator uint16_t() const { return n; }
    operator int() const { return n; }
};



class MinMaxPassengers
{
    static const int s_passengerLength = 3;
    uint32_t passengers_;
    friend std::ostream& operator<<(std::ostream& str, MinMaxPassengers p);
public:
    inline void Set(const std::string str, size_t offset)
    {
        int passengers = 0;
        for (int i = 0; i < s_passengerLength; ++i)
        {
            passengers = passengers * 10 + str[i + offset] - '0';
        }
        passengers_ = passengers;
    }
    inline int Get() const { return passengers_; }
};

inline std::ostream& operator<<(std::ostream& str, MinMaxPassengers p)
{
    str << p.passengers_;
    return str;
}



class Fare
{
    static const int s_farelength = 8;
    uint32_t fare_;
    friend std::ostream& operator<<(std::ostream& str, Fare f);
public:
    inline void Set(const std::string str, size_t offset)
    {
        int fare = 0;
        for (int i = 0; i < s_farelength; ++i)
        {
            fare = fare * 10 + str[i + offset] - '0';
        }
        fare_ = fare;
    }
    inline int Get() const { return fare_; }
    bool operator==(const Fare other) const
    {
        return fare_ == other.fare_;
    }
    operator uint32_t() const { return fare_; }
};

inline std::ostream& operator<<(std::ostream& str, Fare f)
{
    str << f.fare_;
    return str;
}

class Indicator
{
    bool indicated_;
    friend std::ostream& operator<<(std::ostream& str, Indicator ind)
    {
        str << (ind.indicated_ ? "Y" : "N");
        return str;
    }
public:
    inline void Set(const std::string& line, size_t offset)
    {
        if (line[offset] == 'Y')
        {
            indicated_ = true;
        }
        else if (line[offset] == 'N')
        {
            indicated_ = false;
        }
    }
    inline bool Get() const { return indicated_; }
	inline bool IsIndicated() const { return indicated_; }
	inline std::string GetString() const { return indicated_ ? "Y" : "N"; }
    bool operator==(const Indicator& other) const
    {
        return indicated_ == other.indicated_;
    }
};

// Current future marker for restrictions file:
class CFMarker
{
    bool future_;
    friend std::ostream& operator<<(std::ostream& str, CFMarker ind)
    {
        str << (ind.future_ ? "F" : "C");
        return str;
    }
public:
    CFMarker() = default;
    CFMarker(char c)
    {
        if (c == 'F')
        {
            future_ = true;
        }
        else if (c == 'C')
        {
            future_ = false;
        }
        else
        {
            throw QException(std::string("") + "Invalid CF marker '" + c + "' - must be C or F");
        }
    }
    inline void Set(const std::string& line, size_t offset)
    {
        if (line[offset] == 'C')
        {
            future_ = false;
        }
        else if (line[offset] == 'F')
        {
            future_ = true;
        }
    }
    inline bool IsFuture() const { return future_; }
    inline bool IsCurrent() const { return !future_; }
    inline std::string GetString() const { return future_ ? "F" : "C"; }
    bool operator==(const CFMarker& other) const
    {
        return future_ == other.future_;
    }
    // need to compare for std::map - remember bools get promoted to ints according to the C++ standard
    // with false becoming zero and true become one
    bool operator<(const CFMarker& other) const { return future_ < other.future_; }
};


class NDRecordType
{
	bool override_;
public:
	inline void Set(const std::string& line, size_t offset)
	{
		if (line[offset] == 'O')
		{
			override_ = true;
		}
		else if (line[offset] == 'N')
		{
			override_ = false;
		}
	}
	inline bool Get() const { return override_; }
	void SetOverride() { override_ = true; }
	void SetNDF() { override_ = false; }
};

// various RJIS fixed width codes:
using RouteCode = AlNum<5>;
using RailcardCode = AlNum<3, true>;	// railcard can have spaces
using TicketCode = AlNum<3>;
using RestrictionCode = AlNum<2>;
using StatusCode = AlNum<3>;
using CRSCode = AlNum<3>;
using NSDiscFlag = AlNum<1, true>; 	// flag can have a space
using AddonAmount = AlNum<8, true>; 	// for non-standard discounts
using ValidityCode = AlNum<2>;
using CapriCode = AlNum<3>;
using TOCCode = AlNum<3>;
using UTSCode1 = AlNum<1>;
using UTSCode2 = AlNum<2>;
using UTSCode3 = AlNum<2>;
using AdminAreaCode = AlNum<2>;
using ERSCountry = AlNum<2>;
using ErsCode = AlNum<3>;
using CountyCode = AlNum<2>;
using PTECode = AlNum<2>;
using ValidityPeriod = AlNum<4, true>;


typedef FixedInt<1> TicketClass;
typedef FixedInt<3> NumPassengers;
typedef FixedInt<3> Percentage;
typedef FixedInt<3> Multiplier;
typedef FixedInt<2> DiscountCategory;
typedef FixedInt<1> GatePasses;
typedef FixedInt<1> GateTimeCode;
typedef FixedInt<4> RSequence;
#pragma once

namespace  RJISDate {

const int firstYear = 2013; // 1st January 2013 is day 0, however, we use zero to represent always valid, so any date before 2013 is zero
const int lastYear = 2158;  // 65536 days
const int errorFirstYear = 1980;
const int errorLastYear = 3000;

struct DateError : public std::exception
{
    DateError(std::string s) : std::exception(s.c_str()) {}
};

class DateRange;

#pragma pack(push, 1)
union UDateRange
{
    unsigned int range_;
    struct  
    {
        unsigned short rsecond;
        unsigned short rfirst;
    } r;
};
#pragma pack(pop)

// from 'The explanatory supplement to the astronomical almanac'
inline void GetYMD(int&y, int&m, int&d, int n)
{
    int j = n + 693901 + 29220 + 12054;
    y = (4 * j - 1) / 146097;
    j = (4 * j - 1) % 146097;
    d = j / 4;
    j = (4 * d + 3) / 1461;
    d = ((4 * d + 3) % 1461) / 4 + 1;
    m = (5 * d - 3) / 153;
    d = ((5 * d - 3) % 153) / 5 + 1;
    y = 100 * y + j;
    if (m < 10)
    {
        m = m + 3;
    }
    else
    {
        m = m - 9;
        y = y + 1;
    }
}

inline unsigned int Serial(int y, int m, int d)
{
    unsigned int result;
    if (y >= lastYear)
    {
        result = 0xFFFF;
    }
    else if (y < firstYear)
    {
        result = 0;
    }
    else
    {
        result = 367 * y - 7 * (y + (m + 9) / 12) / 4 - 3 * ((y + (m - 9) / 7) / 100 + 1) / 4 + 275 * m / 9 + d - 735265;
    }
    return result;
}

// Get the number of days in a month - this is only designed to work until 2199 as we only support 65535 days
// from our "Epoch". It should be faster as we are not checking divisibility by 100
inline unsigned int __fastcall GetDaysInMonth(int y, int m)
{
    unsigned int result = 30;
    if (m == 2)
    {
        if ((y & 3) && y != 2100)
        {
            result ^= 2;
        }
        else 
        {
            result ^= 3;
        }
    }
    else if ((1 << m) & 0x15AA)
    {
        result |= 1;
    }
    return result;
}

inline void CheckDateValid(int y, int m, int d)
{
    if (m < 1 || m > 12)
    {
        throw DateError("Invalid month in Date");
    }
    else if (y < errorFirstYear || y > errorLastYear)
    {
        throw DateError("Invalid year in date");
    }
    else if (static_cast<unsigned>(d) > GetDaysInMonth(y, m))
    {
        throw DateError("Invalid day number in date");
    }
}

inline unsigned int SetSerialFromDDMMYYYYString(const std::string &s, size_t offset)
{
    assert(s.length() - offset == 8 && "String too short for DDMMYYYY");
    for (size_t i = 0; i < 8; i++)
    {
        if (!isdigit(s[i + offset]))
        {
            throw DateError("Bad character in date");
        }
    }
    int d = 0, m = 0, y = 0;
    d = 10 * (s[offset] - '0') + s[offset + 1] - '0';
    m = 10 * (s[offset + 2] - '0') + s[offset + 3] - '0';
    y = 1000 * (s[offset + 4] - '0') + 100 * (s[offset + 5] - '0') + 10 * (s[offset + 6] - '0') + s[offset + 7] - '0';
    return Serial(y, m, d);
}

inline unsigned int SetSerialFromDoubleDDMMYYYYString(std::string s, int offset)
{
    assert(sizeof(UDateRange) == 4);
    assert(s.length() >= 16 && "String must be 16 chars DDMMYYYY");

    UDateRange u;
    int d1 = 0, m1 = 0, y1 = 0;
    d1 = 10 * (s[offset + 0] - '0') + s[offset + 1] - '0';
    m1 = 10 * (s[offset + 2] - '0') + s[offset + 3] - '0';
    y1 = 1000 * (s[offset + 4] - '0') + 100 * (s[offset + 5] - '0') + 10 * (s[offset + 6] - '0') + s[offset + 7] - '0';

    int d2 = 0, m2 = 0, y2 = 0;
    d2 = 10 * (s[offset + 8] - '0') + s[offset + 9] - '0';
    m2 = 10 * (s[offset + 10] - '0') + s[offset + 11] - '0';
    y2 = 1000 * (s[offset + 12] - '0') + 100 * (s[offset + 13] - '0') + 10 * (s[offset + 14] - '0') + s[offset + 15] - '0';
    CheckDateValid(y1, m1, d1);
    CheckDateValid(y2, m2, d2);
    u.r.rfirst = Serial(y1, m1, d1);
    u.r.rsecond = Serial(y2, m2, d2);
    return u.range_;
}


class Date
{
    unsigned short serial_;
public:
    Date() {}
    Date(int y, int m, int d) : serial_(Serial(y, m, d)) {}
    int GetSerial() {return serial_;}
    void SetYMD(int y, int m, int d)
    {
        if (y > lastYear)
        {
            serial_ = 0xFFFF;
        }
        else
        {
            CheckDateValid(y, m, d);
            serial_ = Serial(y, m, d);
        }
    }
    void GetYMD(int&y, int&m, int&d) const
    {
        RJISDate::GetYMD(y, m, d, serial_);
    }
    
    bool operator<(const Date& d2) {return serial_ < d2.serial_;}
    bool operator<=(const Date& d2) {return serial_ <= d2.serial_;}
    bool operator>(const Date& d2) {return serial_ > d2.serial_;}
    bool operator>=(const Date& d2) {return serial_ >= d2.serial_;}
    void operator+=(int n) {serial_ += n;} // add days
    Date& operator+(int n) {return *this;}
};

class DateRange
{
#pragma pack(push, 1)
    union
    {
        unsigned int range_;
        struct
        {
            unsigned short rsecond;
            unsigned short rfirst;
        } r;
    };
#pragma pack(pop)
public:
    // default constructor - set both dates to 1 Jan 1980:
    DateRange() : r({0xFFFF, 0}) {}

    // set a range based on two dates without checking validity:
    DateRange(int y1, int m1, int d1, int y2, int m2, int d2)
    {
        r.rfirst = Serial(y1, m1, d1);
        r.rsecond = Serial(y2, m2, d2);
    }

    DateRange(std::string s, size_t offset)
    {
        SetSerial(SetSerialFromDoubleDDMMYYYYString(s, offset));
    }

    void SetRange(int y1, int m1, int d1, int y2, int m2, int d2)
    {
        r.rfirst = Serial(y1, m1, d1);
        r.rsecond = Serial(y2, m2, d2);
    }

    int GetDays()
    {
        return r.rsecond - r.rfirst;
    }

    inline bool IsDateInRange(int y, int m, int d)
    {
        unsigned int serial = Serial(y, m, d);
        return r.rfirst <= serial && serial <= r.rsecond;
    }

    void GetYMDYMD(int&y1, int&m1, int&d1, int&y2, int&m2, int&d2) const
    {
        GetYMD(y1, m1, d1, r.rfirst);
        GetYMD(y2, m2, d2, r.rsecond);
    }

    inline void __fastcall SetSerials(int serial1, int serial2)
    {
        assert(serial1 >= 0 && serial1 <= 0xFFFF && serial2 >=0 && serial2 <= 0xFFFF);
        r.rfirst = serial1;
        r.rsecond = serial2;
    }

    inline void __fastcall SetSerial(unsigned int serial)
    {
        range_ = serial;
    }
};

inline void SetDateRangeFrom16String(DateRange & dr, std::string s, int offset)
{
    assert(s.length() - offset >= 16 && "String at offset must be at least 16 more chars DDMMYYYYDDMMYYYY");
    dr.SetSerial(SetSerialFromDoubleDDMMYYYYString(s, offset));
}

}
#pragma once
#undef min
#undef max
// RJIS date class - all dates are stored as the number of days since 1-Jan-1970
// so 1-Jan-1970 itself is stored as zero. We use 1970 as the day since Parkeon TLV
// uses 1970. In practice there is very little need for dates more than a couple of years
// before fares updates are released.
// As far as day numbers are concerned, if they are ever used then zero is Monday and six is Sunday


namespace RJISDate
{
#if 0 // fool Visual Assist
}
#endif

// namespace scope definitions and functions:

const uint32_t firstyear = 1970;
const uint32_t lastyear = 2999;

struct DateException : public std::exception
{
	DateException(std::string msg) : std::exception(msg.c_str()) {}
	DateException(char* msg) : std::exception(msg) {}
	virtual ~DateException() {}
};


inline uint32_t GetSerial(const int y, const int m, const int d)
{
	int result = 367 * y - 7 * (y + (m + 9) / 12) / 4 - 3 * ((y + (m - 9) / 7) / 100 + 1) / 4 + 275 * m / 9 + d - 736359;
	return (uint32_t)result;
}

// deserialise a date from a number of days - from 'The explanatory supplement to the astronomical almanac'
inline void GetYMD(int&y, int&m, int&d, int n)
{
	int j = n + 702669;
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

inline bool IsDateValid(const uint32_t y, const uint32_t m, const uint32_t d)
{
	bool valid = true;
	if (y < firstyear || y > lastyear)
	{
		valid = false;
	}
	else if (m < 1 || m > 12)
	{
		valid = false;
	}
	else if (d < 1)
	{
		valid = false;
	}
	else if (d > 30 && (m == 4 || m == 6 || m == 9 || m == 11))
	{
		valid = false;
	}
	else if (m == 2)
	{
		// if a leap year:
		if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
		{
			if (d > 29)
			{
				valid = false;
			}
		}
		else if (d > 28)
		{
			valid = false;
		}
	}
	else if (d > 31)
	{
		valid = false;
	}
	return valid;
}

inline void GetYMDFromString(int&y, int&m, int&d, const std::string& line, size_t offset)
{
	static const int dateLength = 8;
	for (size_t i = 0; i < dateLength; ++i)
	{
		if (!isdigit(line[offset + i]))
		{
			throw DateException("invalid character in RJIS date.");
		}
		if (i > 3)
		{
			y = 10 * y + line[offset + i] - '0';
		}
		else if (i > 1)
		{
			m = 10 * m + line[offset + i] - '0';
		}
		else
		{
			d = 10 * d + line[offset + i] - '0';
		}
	}
}


// a single date:
class Date
{
	uint32_t days_;
	friend std::ostream& operator<<(std::ostream &str, const Date& date);
    friend class Dayset;
public:
	Date() : days_(0) {}                    // set to earliest possible date
	Date(uint32_t days) : days_(days) {}    // set to number of days since earliest possible date
	Date(int y, int m, int d)               // set to specified date
	{
		if (!IsDateValid(y, m, d))
		{
			throw DateException("Invalid date");
		}
		days_ = GetSerial(y, m, d);
	}
    Date(const std::string &s, size_t offset)
    {
        Set(s, offset);
    }
	static Date Today()
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		return Date(GetSerial(st.wYear, st.wMonth, st.wDay));
	}

	static Date ISO8601(std::string iso)
	{
		if (iso.length() != 10)
		{
			throw DateException("ISO8601 date must be 10 characters long");
		}

		if (iso[4] != '-' && iso[7] != '-')
		{
			throw DateException("'-' expected in ISO8601 date string.");
		}
		for (size_t i = 0; i < iso.length(); i++)
		{
			if (i != 4 && i != 7)
			{
				if (!isdigit(iso[i]))
				{
					throw DateException("invalid characted in ISO8601 date.");
				}
			}
		}

		int y = 1000 * (iso[0] - '0') + 100 * (iso[1] - '0') + 10 * (iso[2] - '0') + iso[3] - '0';
		int m = 10 * (iso[5] - '0') + iso[6] - '0';
		int d = 10 * (iso[8] - '0') + iso[9] - '0';
		Date date(y, m, d);
		return date;
	}


	void GetYMD(int&y, int&m, int&d) const
	{
		RJISDate::GetYMD(y, m, d, days_);
	}

	Date& operator++() {
		days_++;
		return *this;
	}
	Date operator++(int) {
		return Date(days_++);
	}
	Date& operator--() {
		days_--;
	}
	Date operator--(int) {
		return Date(days_--);
	}

	int GetDayOfWeek() const {
		return (days_ + 4) % 7;
	}

	bool operator==(const Date& other) const { return days_ == other.days_; }
	bool operator!=(const Date& other) const { return days_ != other.days_; }
	bool operator<(const Date& other) const { return days_ < other.days_; }
	bool operator<=(const Date& other) const { return days_ <= other.days_; }
	bool operator>(const Date& other) const { return days_ > other.days_; }
	bool operator>=(const Date& other) const { return days_ >= other.days_; }

	Date& operator+=(int i)
	{
		days_ += i;
		return *this;
	}

	Date& operator-=(int i)
	{
		days_ -= i;
		return *this;
	}

	const Date operator+(int i) const
	{
		Date result = *this;
		result += i;
		return result;
	}

	const Date operator-(int i) const
	{
		Date result = *this;
		result -= i;
		return result;
	}

	void Set(const std::string& line, const size_t offset)
	{
		int y = 0, m = 0, d = 0;
		GetYMDFromString(y, m, d, line, offset);
		if (!IsDateValid(y, m, d))
		{
			throw DateException("Invalid end date");
		}
		days_ = GetSerial(y, m, d);
	}

	void SetToday()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		days_ = GetSerial(st.wYear, st.wMonth, st.wDay);
	}

	int GetDays() const { return days_; }

	template<class T> void DumpDate(T& str) const
	{
		int y, m, d;
		RJISDate::GetYMD(y, m, d, days_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << " ";
	}

	template<class T> void DumpDateText(T& str, bool space = false) const
	{
		std::string months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		int y, m, d;
		RJISDate::GetYMD(y, m, d, days_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << '-' << months[m - 1] << '-' << std::setw(4) << y << (space ? " " : "");
	}

	template<class T> void DumpDateRJISFormat(T& str) const
	{
		int y, m, d;
		RJISDate::GetYMD(y, m, d, days_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	}

	friend class Range;
	friend class Triple;
};

// start and end date:
class Range
{
	uint32_t daysStart_;
	uint32_t daysEnd_;
	friend std::ostream& operator<<(std::ostream &str, const Range& r);
public:
	Range() : daysStart_(0), daysEnd_(0) {}

	Range(int y1, int m1, int d1, int y2, int m2, int d2)
	{
		if (!IsDateValid(y1, m1, d1) || !IsDateValid(y2, m2, d2))
		{
			throw DateException("an invalid date was supplied\n");
		}
		daysStart_ = GetSerial(y1, m1, d1);
		daysEnd_ = GetSerial(y2, m2, d2);
	}

	Range(const Date& dend, const Date& dstart)
	{
		daysEnd_ = dend.days_;
		daysStart_ = dstart.days_;
	}

    Range(const std::string& line, size_t offset, bool backwards = false)
    {
        Set(line, offset, backwards);
    }

    void Set(int y1, int m1, int d1, int y2, int m2, int d2)
    {
        if (!IsDateValid(y1, m1, d1) || !IsDateValid(y2, m2, d2))
        {
            throw DateException("an invalid date was supplied\n");
        }
        daysStart_ = GetSerial(y1, m1, d1);
        daysEnd_ = GetSerial(y2, m2, d2);
    }


	inline void Set(std::string line, size_t offset, bool backwards = false)
	{
		int ys = 0, ms = 0, ds = 0; // start date
		int ye = 0, me = 0, de = 0; // end date
        if (backwards)
        {
            GetYMDFromString(ye, me, de, line, offset + 8);
            GetYMDFromString(ys, ms, ds, line, offset);
        }
        else
        {
            GetYMDFromString(ye, me, de, line, offset);
            GetYMDFromString(ys, ms, ds, line, offset + 8);
        }
		if (!IsDateValid(ys, ms, ds))
		{
			throw DateException("Invalid end date");
		}
		if (!IsDateValid(ye, me, de))
		{
			throw DateException("Invalid start date");
		}

		daysEnd_ = GetSerial(ye, me, de);
		daysStart_ = GetSerial(ys, ms, ds);
	}

	inline bool IsDateInRange(int y, int m, int d) const
	{
		if (!IsDateValid(y, m, d))
		{
			throw DateException("an invalid date was supplied\n");
		}
		uint32_t serial = GetSerial(y, m, d);
		return serial >= daysStart_ && serial <= daysEnd_;
	}

    void SetStartDate(const Date& date)
    {
        daysStart_ = date.days_;
    }

    void SetEndDate(const Date& date)
    {
        daysEnd_ = date.days_;
    }

	Date GetEndDate() const
	{
		return Date(daysEnd_);
	}

	Date GetStartDate() const
	{
		return Date(daysStart_);
	}

	inline bool IsDateInRange(const Date& d) const
	{
		return d.days_ >= daysStart_ && d.days_ <= daysEnd_;
	}

	virtual ~Range(){}


	template<class T> void DumpDates(T& str) const
	{
		int y, m, d;
		GetYMD(y, m, d, daysEnd_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << " ";
		GetYMD(y, m, d, daysStart_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << " ";
	}
};

inline std::ostream& operator<<(std::ostream &str, const Range& range)
{
	int y, m, d;
	GetYMD(y, m, d, range.daysEnd_);
	str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	GetYMD(y, m, d, range.daysStart_);
	str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	return str;
}

// start, end and quote dates:
class Triple
{
	static const size_t TripleDateLength = 24;
	uint32_t daysStart_;
	uint32_t daysEnd_;
	uint32_t daysQuote_;
	friend std::ostream& operator<<(std::ostream &str, const Triple& triple);
public:
	Triple() {}
	Triple(const Date& dend, const Date& dstart, const Date& dquote)
	{
		daysEnd_ = dend.days_;
		daysStart_ = dstart.days_;
		daysQuote_ = dquote.days_;
	}

    Triple(std::string line, size_t offset)
    {
        Set(line, offset);
    }

	virtual ~Triple(){}

	inline void Set(std::string line, size_t offset)
	{
		int ys = 0, ms = 0, ds = 0; // start date
		int ye = 0, me = 0, de = 0; // end date
		int yq = 0, mq = 0, dq = 0; // quote date
		GetYMDFromString(ye, me, de, line, offset);
		GetYMDFromString(ys, ms, ds, line, offset + 8);
		GetYMDFromString(yq, mq, dq, line, offset + 16);
		if (!IsDateValid(yq, mq, dq))
		{
			throw DateException("Invalid quote date");
		}
		if (!IsDateValid(ys, ms, ds))
		{
			throw DateException("Invalid end date");
		}
		if (!IsDateValid(ye, me, de))
		{
			throw DateException("Invalid start date");
		}

		daysEnd_ = GetSerial(ye, me, de);
		daysStart_ = GetSerial(ys, ms, ds);
		daysQuote_ = GetSerial(yq, mq, dq);
	}

	void SetStart(const Date& date)
	{
		daysStart_ = date.GetDays();
	}

	void SetEnd(const Date& date)
	{
		daysEnd_ = date.GetDays();
	}

	void SetQuote(const Date& date)
	{
		daysQuote_ = date.GetDays();
	}

	void SetRange(Range r)
	{
		SetEnd(r.GetEndDate());
		SetStart(r.GetStartDate());
	}


	int GetEndDays() const { return daysEnd_; }
	int GetStartDays() const { return daysStart_; }
	int GetQuoteDays() const { return daysQuote_; }

	Date GetEndDate() const { return Date(daysEnd_); }
	Date GetStartDate() const { return Date(daysStart_); }
	Date GetQuoteDate() const { return Date(daysQuote_); }

    // if we can query a record on this date and the travel date is between the start
    // and end date then return true - otherwise return false:
    bool AreDatesValid(RJISDate::Date queryDate, RJISDate::Date travelDate)
    {
        return queryDate.days_ >= daysQuote_ &&
            travelDate.days_ >= daysStart_ &&
            travelDate.days_ <= daysEnd_;
    }

	template<class T> void DumpDatesRJISFormat(T& str)
	{
		int y, m, d;
		GetYMD(y, m, d, daysEnd_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
		GetYMD(y, m, d, daysStart_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
		GetYMD(y, m, d, daysQuote_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	}

	template<class T> void DumpDates(T& str)
	{
		int y, m, d;
		GetYMD(y, m, d, daysEnd_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << " ";
		GetYMD(y, m, d, daysStart_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << " ";
		GetYMD(y, m, d, daysQuote_);
		str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y << std::endl;
	}
};

// function returns true if the start-end ranges from the two dates are next to each other or they overlap
// this works for Ranges and Triples (start-end must be "well-formed")
template <class D> bool AreRangesMergeable(D r1, D r2)
{
	bool result;
	int x1 = r1.GetStartDate().GetDays();
	int x2 = r1.GetEndDate().GetDays();
	int y1 = r2.GetStartDate().GetDays();
	int y2 = r2.GetEndDate().GetDays();

	// consider them "overlapping" if they are adjacent - lower the "late" start date by a day:
	if (x1 < y1)
	{
		y1--;
	}
	else
	{
		x1--;
	}

	result = x1 <= y2 && y1 <= x2;
	return result;
}

// this works for Ranges and Triples (start-end must be "well-formed")
template <class D> bool AreRangesOverlapping(D r1, D r2)
{
	bool result;
	int x1 = r1.GetStartDate().GetDays();
	int x2 = r1.GetEndDate().GetDays();
	int y1 = r2.GetStartDate().GetDays();
	int y2 = r2.GetEndDate().GetDays();

	result = x1 <= y2 && y1 <= x2;
	return result;
}

inline bool AreRangesOverlapping(const Date& r1end,
								 const Date& r1start,
								 const Date& r2end,
								 const Date& r2start)
{
	return r1start < r2end && r2start <= r1end;
}



template <class D> D MergeRanges(D r1, D r2)
{
	int x1 = r1.GetStartDate().GetDays();
	int x2 = r1.GetEndDate().GetDays();
	int y1 = r2.GetStartDate().GetDays();
	int y2 = r2.GetEndDate().GetDays();

	D result(std::max(x2, y2), std::min(x1, y1));
	return result;
}

inline Triple MergeRanges(Triple r1, Triple r2)
{
	Range r = MergeRanges(Range(r1.GetEndDate(), r1.GetStartDate()), Range(r2.GetEndDate(), r2.GetStartDate()));
	Date qd1 = r1.GetQuoteDate();
	Date qd2 = r2.GetQuoteDate();
	Triple result(r.GetEndDate(), r.GetStartDate(), std::min(qd1, qd2));
	return result;
}

template <class D>std::pair<Range, Range> MakeHoleInRange(D outer, D inner)
{
	assert(inner.GetStartDate() > outer.GetStartDate() && inner.GetEndDate() < outer.GetEndDate());
	std::pair<Range, Range> result = std::make_pair(
		Range(inner.GetStartDate() - 1, outer.GetStartDate()),
		Range(inner.GetEndDate() + 1, outer.GetEndDate()));

	return result;
}

// stream insertion operator - prints a triple date in RJIS format DDMMYYYYDDMMYYYYDDMMYYYY with no 
// separators
inline std::ostream& operator<<(std::ostream &str, const Triple& triple)
{
	int y, m, d;
	GetYMD(y, m, d, triple.daysEnd_);
	str << std::dec << std::right << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	GetYMD(y, m, d, triple.daysStart_);
    str << std::dec << std::right << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	GetYMD(y, m, d, triple.daysQuote_);
    str << std::dec << std::right << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	return str;
}

inline std::ostream& operator<<(std::ostream &str, const Date& date)
{
	int y, m, d;
	GetYMD(y, m, d, date.days_);
	str << std::dec << std::setfill('0') << std::setw(2) << d << std::setw(2) << m << std::setw(4) << y;
	return str;
}

// a class representing a set of days of the week when a journey is valid. It stores a single 8 bit value
// with bit 0 meaning monday is including in the set, bit 1 meaning tuesday is included and so on - so 127 means
// that a journey is valid on every day of the week. Bit 7 is not used.
class Dayset
{
    uint8_t dayset_; 
public:
    Dayset() : dayset_(0) {}
    void Set(std::string days, size_t offset = 0, bool yn = false)
    {
        if (days.length() - offset < 7)
        {
            throw DateException("Invalid running days string");
        }

        char onComparator, offComparator;
        if (yn)
        {
            onComparator = 'Y';
            offComparator = 'N';
        }
        else
        {
            onComparator = '1';
            offComparator = '0';
        }

        uint8_t d = 0;
        uint8_t mask = 1;
        for (auto i = 0u; i < 7; i++, mask <<= 1)
        {
            if (days[i + offset] == onComparator)
            {
                d |= mask;
            }
            else if (days[i + offset] != offComparator)
            {
                throw DateException("Invalid character in running days");
            }
        }
        dayset_ = d;
    }

    Dayset(std::string days, size_t offset = 0, bool yn = false)
    {
        Set(days, offset, yn);
    }

    bool IsDateInDayset(const Date d) const
    {
        uint8_t dayOfWeek = static_cast<uint8_t>((d.days_ + 3) % 7);
        bool result = ((1 << dayOfWeek) & dayset_) != 0;
        return result;
    }

    template <class T> void DumpDays(T& str)
    {
        for (uint8_t p = 1; p; p <<= 1)
        {
            if (dayset_ & p)
            {
                str.put('Y');
            }
            else
            {
                str.put('N');
            }
        }
    }
};

}

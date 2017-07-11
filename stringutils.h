#pragma once
namespace ams{

// case insensitive strings:
struct ci_char_traits : public std::char_traits<char>
            // just inherit all the other functions
            //  that we don't need to override
{
	static bool eq(char c1, char c2)  { return toupper(c1) == toupper(c2); }

	static bool ne( char c1, char c2 )  { return toupper(c1) != toupper(c2); }

	static bool lt( char c1, char c2 ) { return toupper(c1) <  toupper(c2); }

	static int compare( const char* s1, const char* s2, size_t n ) {return _memicmp( s1, s2, n ); }

static const char*	find( const char* s, int n, char a )
{
    while( n-- > 0 && toupper(*s) != toupper(a) )
	{
        ++s;
    }
    return s;
}
};

typedef std::basic_string<char, ci_char_traits> ci_string;


inline void MakeLower(std::string&s)
{
	std::transform(s.begin(), s.end(), s.begin(), tolower);
}

inline void MakeUpper(std::string&s)
{
	std::transform(s.begin(), s.end(), s.begin(), toupper);
}

}

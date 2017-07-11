#pragma once

class CADate
{
public:
    int GetDayNumber();
    int GetWeekNumber();
    int SetPrevious(int m);
    static char * GetMonthName(int m);
    inline int GetDayOfWeek() {return m_serial % 7;}
    void SetDMY(int d, int m, int y);
    void GetDMY(int &d, int &m, int &y);

    inline int GetYear()
        {if (!m_cached) GetDMY(m_cachedD, m_cachedM, m_cachedY); return m_cachedY;}
    inline int GetMonth()
        {if (!m_cached) GetDMY(m_cachedD, m_cachedM, m_cachedY); return m_cachedM;}
    inline int GetDay()
        {
        if (!m_cached) GetDMY(m_cachedD, m_cachedM, m_cachedY); return m_cachedD;}

    inline int GetSerial() { return m_serial;}
    void SetToday();
    void SetEaster(int y);
	bool IsLastDayOfMonth();

    CADate();
    CADate(int serial);
    CADate(int d, int m, int y);
    virtual ~CADate();

    CADate operator ++();
    CADate operator --();
    CADate operator ++(int x);
    CADate operator --(int x);

    CADate operator +(int x);
    CADate operator +=(int x);
    CADate operator -(int x);
    CADate operator -=(int x);
    inline int operator -(CADate x) {return m_serial - x.m_serial;}
    inline bool operator==(CADate &x) {return m_serial == x.m_serial;}
    inline bool operator>(CADate &x) {return m_serial > x.m_serial;}
    inline bool operator<(CADate &x) {return m_serial < x.m_serial;}
    inline bool operator>=(CADate &x) {return m_serial >= x.m_serial;}
    inline bool operator<=(CADate &x) {return m_serial <= x.m_serial;}
    inline bool operator!=(int x) {return x != m_serial;}
    inline bool operator==(int x) {return x == m_serial;}

private:
    int m_serial;
    bool m_cached;
    int m_cachedD, m_cachedM, m_cachedY;
public:
        bool InRange(CADate &d1, CADate &d2);
    void Cache();
    bool IsInEUDST();
    int SetJan1(int year);
    char* GetShortMonthName();
    void SetFromJD(int jd);
    double GetJulianDate();
    int m_marker;
};

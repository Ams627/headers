#pragma once
#include <ams/smallinstancenumber.h>



namespace sin=ams::SmallInstanceNumber; //namespace alias

// This include file is need for the IRCOperations class.
// In order to implement an IRC Client you need to pass raw socket data to an instance of the IRCOperations
// class by calling the ProcessRawData member. 
// You can control all other behaviour by calling Join, Part and by overriding OnConnect, OnJoin
// OnChannelMessage, OnUserMessage etc.

// link to the AMS IRC library:
#pragma comment(lib, "AMSIRCLib.lib")

namespace IRC
{
#if 0
} // fool visual assist
#endif

// This type definition is required for the writeback function. A writeback function must be passed to the
// IRCOperations constructor. It is called when the IRC library wishes to send raw data to the socket.
// The other parameter of the IRCOperations constructor is a socket identifier. Each instance of the
// IRCOperations class is thus associated with a single socket. The socket identifier passed to the
// IRCOperations constructor is passed as the first parameter to the writeback function whenever it
// is called. It is up to the implementer to map the socket identifier to a windows SOCKET handle.
typedef boost::function<void (int, const char*, int) > WRITER;

// Prototype for the raw data processor in the IRC library. This function should not be called by 
// implementers but is called by the IRCProcessor class.
void ProcessRawData(int instancenumber, char* buf, int len);

// Typedef and prototype for internally used function. Do not call this function
// yourself.
typedef void (*CommandCallBack)(int param,
                                std::string prefix,
                                int numeric,
                                std::string command,
                                std::vector<std::string> params,
                                std::string suffix);
void Init(CommandCallBack cbk);

struct User
{
    std::string nick_;
    char type_;
};

struct Channel
{
    std::string topic_;
    std::string setby_;
    std::deque<User> members_;
};

// Main IRC operations class - this class has an instance number per instance
// provided by inheriting from ams::HasInstanceNumber. This instance number is used
// as a completion port key, so that when Read operations complete the raw socket data
// can be passed back to the appropriate instance of this class
class IRCOperations : public boost::noncopyable, public sin::HasSmallInstanceNumber<IRCOperations>
{
public:
    static void CommandCallBack(int param,
        std::string prefix,
        int numeric,
        std::string command,
        std::vector<std::string> params,
        std::string suffix);
    typedef std::map<std::string, Channel> CHANNELLIST;
    typedef CHANNELLIST::iterator CHANNELLISTITER;
    CHANNELLIST channelList_;
    WRITER writer_;				// callback function to write raw data
    int writebackKey_;			// key to pass to the callback function
public:
    IRCOperations(WRITER writer, int key): writer_(writer), writebackKey_(key) {}

    void ProcessRawData(char*data, DWORD len); // this function processes raw socket data from an IRC server
    void Join(std::string);
    void Names(std::string channel, std::vector<std::string> names);
    void SendUser(std::string username, std::string hostname, std::string servername, std::string realname);
    void SendNick(std::string nick);
    void SendPong(std::string pingstring);
    void SendS(std::string s);
    void Send(const char* buf, int len);

   void ProcessJoin(std::string user, std::string channel);
   void ProcessNamesReply(std::string channel, std::string memberlist);

    // overridables:
    virtual void OnConnect();
    virtual void OnConnectNumeric(const int numeric, std::vector<std::string>& params, const std::string suffix);
    virtual void OnPrivMsg(std::string from, std::string to, std::string msg);
    void OnJoin(std::string user, std::string channel);

    static void Init();
};

}
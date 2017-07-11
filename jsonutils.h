#pragma once

namespace ams
{
    namespace JSON
    {
        inline std::string Key(std::string name)
        {
            return "\"" + name + "\":";
        }
        inline std::string NVPair(std::string name, std::string value, bool comma = false)
        {
            std::string result;
            result = "\"" + name + "\": \"" + value + "\"" + (comma ? "," : "");
            return result;
        }

        inline std::string NVPair(std::string name, int value, bool comma = false)
        {
            std::string result;
            result = "\"" + name + "\": " + std::to_string(value) + (comma ? "," : "");
            return result;
        }

        inline std::string NVPair(std::string name, char value, bool comma = false)
        {
            std::string result;
            result = "\"" + name + "\": \"" + value + "\"" + (comma ? "," : "");
            return result;
        }


        inline std::string NVPair(std::string name, uint64_t value, bool comma = false)
        {
            std::string result;
            result = "\"" + name + "\": " + std::to_string(value) + (comma ? "," : "");
            return result;
        }

        inline std::string NVPair(std::string name, GUID guid, bool comma = false)
        {
            std::string result;
            std::ostringstream ss;
            ss << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << guid.Data1 << '-' <<
                std::setw(4) << guid.Data2 << '-' <<
                std::setw(4) << guid.Data3 << '-';

            for (int i = 0; i < 8; i++)
            {
                ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)guid.Data4[i];
                if (i == 1)
                {
                    ss << '-';
                }
            }

            result = "\"" + name + "\": " + "\"" + ss.str() + "\"" + (comma ? "," : "");
            return result;
        }
    }
}
#pragma once

namespace TLVUtils
{
    // convert an integer, either 64 bits or 32 bits to its TLV representation
    // for details, see the document SFI-01-DTT.doc, but the representation
    // resembles UTF-8. The number of bytes needed is returned by this function and
    // you should use this return value to indicate the number of bytes to be written
    // to the TLV file:
    template <typename T, typename U> size_t Convert(T& out, const U in)
    {
        size_t result = 0;
        out = 0;
        if (sizeof U > sizeof uint32_t || sizeof T > sizeof uint32_t)
        {
            if (in > 0x1FFFFFFF && in <= 0x1FFFFFFFFFLL) // one and SEVEN Fs one and NINE Fs
            {
                result = 5;
                out = in;
                out |= 0xE000000000LL;
            }
        }
        if (out == 0)
        {
            if (in > 0x1FFFFFFF)
            {
                result = 0;
            }
            else if (in > 0x1FFFFF)
            {
                result = 4;
                out = in | 0xC0000000;
            }
            else if (in > 0x1FFF)
            {
                result = 3;
                out = in | 0xA00000;
            }
            else if (in > 0x7F)
            {
                result = 2;
                out = in | 0x8000;
            }
            else
            {
                result = 1;
                out = in & 0x7F;
            }
        }
        return result;
    }
//     template <> size_t Convert(uint32_t&, const uint64_t)
//     {
//         std::cout << "special\n";
//         return 0;
//     }

#pragma warning(disable : 4293)
    // "enbuffer" an integer - TLV is BigEndian or "Motorola" format and we may need
    // to write up to 5 bytes to the buffer:
    template <typename T> void EnBuffer(BYTE*buf, const T in, const size_t length)
    {
        if (length > 0)
        {
            buf[length - 1] = in & 0xFF;
        }
        if (length > 1)
        {
            buf[length - 2] = (in >> 8) & 0xFF;
        }
        if (length > 2)
        {
            buf[length - 3] = (in >> 16) & 0xFF;
        }
        if (length > 3)
        {
            buf[length - 4] = (in >> 24) & 0xFF;
        }
        if (length > 4)
        {
            buf[length - 5] = (in >> 32) & 0xFF;
        }
    }

    // "enbuffer" an integer to a vector or deque or anything that implements push_back
    template <typename T, class BufferType> void EnBuffer(BufferType& buf, const T in, const size_t length)
    {
        if (length > 4)
        {
            buf.push_back((in >> 32) & 0xFF);
        }
        if (length > 3)
        {
            buf.push_back((in >> 24) & 0xFF);
        }
        if (length > 2)
        {
            buf.push_back((in >> 16) & 0xFF);
        }
        if (length > 1)
        {
            buf.push_back((in >> 8) & 0xFF);
        }
        if (length > 0)
        {
            buf.push_back(in & 0xFF);
        }
    }


}

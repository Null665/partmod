#ifndef CRC_H_CODEFROMCPPDOTCOMBYDISCH
#define CRC_H_CODEFROMCPPDOTCOMBYDISCH
//
// CRC32 checksum class
// Original source: http://www.cplusplus.com/forum/lounge/27570/
//
// Usage:
/*
    CRC32 crc;
    crc.Hash(&addr_of_stuff,length);
    uint32_t hash=crc.Get();
    crc.Reset();
    // crc.Hash() again

    or

    uint32_t hash=CRC32(&addr_of_stuff,length);
*/

#include <stdint.h>

class CRC32
{
public:
    //=========================================
    //  ctors
    inline CRC32()                                  { Reset();                  }
    inline CRC32(const void* buf,uint32_t siz)        { Reset(); Hash(buf,siz);   }

    //=========================================
    // implicit cast, so that you can do something like foo = CRC(dat,siz);
    inline operator uint32_t () const                    { return Get();             }

    //=========================================
    // getting the crc
    inline uint32_t          Get() const                 { return ~mCrc;             }

    //=========================================
    // HashBase stuff
    void        Reset()                     { mCrc = ~0;                }
    void        Hash(const void* buf,uint32_t siz);

private:
    uint32_t         mCrc;
    static bool mTableBuilt;
    static uint32_t  mTable[0x100];

    static const uint32_t        POLYNOMIAL = 0x04C11DB7;

private:
    //=========================================
    // internal support
    static void         BuildTable();
    static uint32_t     Reflect(uint32_t v,int bits);
};


#endif

#include "crc.hpp"


//=====================================================
bool    CRC32::mTableBuilt = false;
uint32_t     CRC32::mTable[0x100];

//=====================================================

void CRC32::BuildTable()
{
    int i, j;
    for(i = 0; i < 0x100; ++i)
    {
        mTable[i] = Reflect(static_cast<uint32_t>(i),8) << 24;

        for(j = 0; j < 8; ++j)
            mTable[i] = (mTable[i] << 1) ^ ( (mTable[i] & (1<<31))  ? POLYNOMIAL : 0);

        mTable[i] = Reflect(mTable[i],32);
    }
    mTableBuilt = true;
}

uint32_t CRC32::Reflect(uint32_t v,int bits)
{
    uint32_t ret = 0;
    int i;

    --bits;
    for(i = 0; i <= bits; ++i)
    {
        if(v & (1<<i))
            ret |= 1 << (bits-i);
    }
    return ret;
}

//=====================================================

void CRC32::Hash(const void* buf,uint32_t siz)
{
    //=============================
    const uint8_t* p = reinterpret_cast<const uint8_t*>(buf);

    //=============================
    if(!mTableBuilt)
        BuildTable();

    uint32_t i;
    for(i = 0; i < siz; ++i)
        mCrc = (mCrc >> 8) ^ mTable[ (mCrc & 0xFF) ^ p[i] ];
}

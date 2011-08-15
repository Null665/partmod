#include "guid_manager.h"
#include "disk.h"
#include "disk_exception.h"
using namespace std;

uint64_t invert_endianess(uint64_t val)
{
    return ((((val) & 0xff00000000000000ull) >> 56) |
            (((val) & 0x00ff000000000000ull) >> 40) |
            (((val) & 0x0000ff0000000000ull) >> 24) |
            (((val) & 0x000000ff00000000ull) >> 8 ) |
            (((val) & 0x00000000ff000000ull) << 8 ) |
            (((val) & 0x0000000000ff0000ull) << 24) |
            (((val) & 0x000000000000ff00ull) << 40) |
            (((val) & 0x00000000000000ffull) << 56));
}



GuidManager::GuidManager()
{
    add_default_guids();
}

void GuidManager::Add(string guid_str,uint32_t uid,string description)
{
    __GUID guid=str_to_guid(guid_str);
    Add(guid,uid,description);
}

void GuidManager::Add(string guid_str,uint32_t uid)
{
    Add(guid_str,uid,"");
}

void GuidManager::Add(__GUID guid,uint32_t id,string description)
{
    GUID_DATA data;
    data.description=description;
    data.guid=guid;

    guid_data.push_back(make_pair(data,id));

}

void GuidManager::Add(__GUID guid,uint32_t id)
{
    Add(guid,id,"");
}

uint32_t GuidManager::Count()
{
    return guid_data.size();
}

__GUID GuidManager::Get(uint32_t which)
{
    if(which>=Count())
        throw DiskException(ERR_OUT_OF_BOUNDS);
    return guid_data[which].first.guid;

}



uint32_t GuidManager::GetUID(uint32_t which)
{
    if(which>Count())
        throw DiskException(ERR_OUT_OF_BOUNDS);
    return guid_data[which].second;
}

__GUID GuidManager::GetByUID(uint32_t uid)
{
    for(unsigned int i=0; i<Count(); i++)
        if(GetUID(i)==uid)
            return guid_data[i].first.guid;

    throw(DiskException(ERR_UNKNOWN_ERROR));
}

__GUID GuidManager::str_to_guid(std::string guid_str)
{
    __GUID guid;
    string guid_parts[4];

    if(guid_str[8]!='-' || guid_str[13]!='-' || guid_str[18]!='-' ||guid_str[23]!='-')
        throw DiskException("Invalid symbol in GPT string");

    guid_parts[0]=guid_str.substr(0,8);
    guid_parts[1]=guid_str.substr(9,4);
    guid_parts[2]=guid_str.substr(14,4);
    guid_parts[3]=guid_str.substr(19,4);
    guid_parts[3]+=guid_str.substr(24,35);

    guid.one=StrToU64(guid_parts[0],16);
    guid.two=StrToU64(guid_parts[1],16);
    guid.three=StrToU64(guid_parts[2],16);
    guid.four=StrToU64(guid_parts[3],16);
    guid.four=invert_endianess(guid.four);

    return guid;

}

string GuidManager::GetDescription(uint32_t which)
{
    return guid_data[which].first.description;
}

string GuidManager::GetDescription(const __GUID &guid)
{
    for(int i=0; i<Count(); i++)
        if(
            guid.one==guid_data[i].first.guid.one &&
            guid.two==guid_data[i].first.guid.two &&
            guid.three==guid_data[i].first.guid.three &&
            guid.four==guid_data[i].first.guid.four )
            return guid_data[i].first.description;

    throw(DiskException("GUID not found"));
}


string GuidManager::GetGuidAsString(uint32_t which)
{
  const __GUID &guid=Get(which);

  string parts[5];
  parts[0]=U64ToStr(guid.one,16);
  parts[1]=U64ToStr(guid.two,16);
  parts[2]=U64ToStr(guid.three,16);
  parts[3]=U64ToStr(invert_endianess(guid.four)>>48,16);
  parts[4]=U64ToStr(invert_endianess(guid.four)&0xFFFFFFFFFFFF,16);

  string str;
  str=parts[0]+"-"+parts[1]+"-"+parts[2]+"-"+parts[3]+"-"+parts[4];
  return str;

}


void GuidManager::add_default_guids()
{
    int i=0;
// OS independent
    Add("00000000-0000-0000-0000-000000000000",i,      "Unused"); // 0
    Add("024DEE41-33E7-11D3-9D69-0008C781F39F",++i,    "MBR partition scheme");
    Add("C12A7328-F81F-11D2-BA4B-00A0C93EC93B",++i,    "EFIsystem partition");
    Add("21686148-6449-6E6F-744E-656564454649",++i,    "BIOS boot partition");

// Windows
    Add("E3C9E316-0B5C-4DB8-817D-F92DF00215AE",++i,    "Microsoft reserved partition");
    Add("EBD0A0A2-B9E5-4433-87C0-68B6B72699C7",++i,    "Basic data partition");  // 5
    Add("5808C8AA-7E8F-42E0-85D2-E1E90434CFB3",++i,    "LDM metadata partition");
    Add("AF9B60A0-1431-4F62-BC68-3311714A69AD",++i,    "LDM data partition");
    Add("DE94BBA4-06D1-4D40-A16A-BFD50179D6AC",++i,    "Windows recovery enviroment partition");
    Add("37AFFC90-EF7D-4E96-91C3-2D7AE055B174",++i,    "IBM GPFS partition");

// HP-UX
    Add("75894C1E-3AEB-11D3-B7C1-7B03A0000000",++i,    "HP-UX data partition"); // 10
    Add("E2A1E728-32E3-11D6-A682-7B03A0000000",++i,    "HP-UX service partition");

// Linux
    Add("EBD0A0A2-B9E5-4433-87C0-68B6B72699C7",++i,    "Linux data partition"); // The same as basic data partition
    Add("A19D880F-05FC-4D3B-A006-743F0F84911E",++i,    "Linux RAID partition");
    Add("0657FD6D-A4AB-43C4-84E5-0933C84B4F4F",++i,    "Linux swap partition");
    Add("E6D6D379-F507-44C2-A23C-238F2A3DF928",++i,    "LVM partition"); // 15
    Add("8DA63339-0007-60C0-C436-083AC8230908",++i,    "Linux reserved");

// FreeBSD
    Add("83BD6B9D-7F41-11DC-BE0B-001560B84F0F",++i,    "FreeBSD boot partition");
    Add("516E7CB4-6ECF-11D6-8FF8-00022D09712B",++i,    "FreeBSD data partition");
    Add("516E7CB5-6ECF-11D6-8FF8-00022D09712B",++i,    "FreeBSD swap partition");
    Add("516E7CB6-6ECF-11D6-8FF8-00022D09712B",++i,    "UFS partition");            // 20
    Add("516E7CB8-6ECF-11D6-8FF8-00022D09712B",++i,    "Vinum volume manager partition");
    Add("516E7CBA-6ECF-11D6-8FF8-00022D09712B",++i,    "ZFS partition");

// Mac OS X
    Add("48465300-0000-11AA-AA11-00306543ECAC",++i,    "HFS partition");
    Add("55465300-0000-11AA-AA11-00306543ECAC",++i,    "Apple UFS");
    Add("6A898CC3-1DD2-11B2-99A6-080020736631",++i,    "ZFS"); // 25
    Add("52414944-0000-11AA-AA11-00306543ECAC",++i,    "Apple RAID");
    Add("52414944-5F4F-11AA-AA11-00306543ECAC",++i,    "Apple RAID offline");
    Add("426F6F74-0000-11AA-AA11-00306543ECAC",++i,    "Apple boot");
    Add("4C616265-6C00-11AA-AA11-00306543ECAC",++i,    "Apple label");
    Add("5265636F-7665-11AA-AA11-00306543ECAC",++i,    "Apple TV recovery"); // 30

// Solaris
    Add("6A82CB45-1DD2-11B2-99A6-080020736631",++i,    "Solaris boot partition");
    Add("6A85CF4D-1DD2-11B2-99A6-080020736631",++i,    "Solaris root");
    Add("6A87C46F-1DD2-11B2-99A6-080020736631",++i,    "Solaris swap");
    Add("6A8B642B-1DD2-11B2-99A6-080020736631",++i,    "Solaris backup");
    Add("6A898CC3-1DD2-11B2-99A6-080020736631",++i,    "Solaris /usr"); // 35
    Add("6A8EF2E9-1DD2-11B2-99A6-080020736631",++i,    "Solaris /var");
    Add("6A90BA39-1DD2-11B2-99A6-080020736631",++i,    "Soalris /home");
    Add("6A9283A5-1DD2-11B2-99A6-080020736631",++i,    "Solaris alternate sector");
    Add("6A945A3B-1DD2-11B2-99A6-080020736631",++i,    "Solaris reserved");
    Add("6A9630D1-1DD2-11B2-99A6-080020736631",++i,    "Solaris reserved"); // 40
    Add("6A980767-1DD2-11B2-99A6-080020736631",++i,    "Solaris reserved");
    Add("6A96237F-1DD2-11B2-99A6-080020736631",++i,    "Solaris reserved");
    Add("6A8D2AC7-1DD2-11B2-99A6-080020736631",++i,    "Solaris reserved");

// NetBSD
    Add("49F48D32-B10E-11DC-B99B-0019D1879648",++i,    "NetBSD swap");
    Add("49F48D5A-B10E-11DC-B99B-0019D1879648",++i,    "NetBSD FFS"); //45
    Add("49F48D82-B10E-11DC-B99B-0019D1879648",++i,    "NetBSD LFS");
    Add("49F48DAA-B10E-11DC-B99B-0019D1879648",++i,    "NetBSD RAID");
    Add("2DB519C4-B10F-11DC-B99B-0019D1879648",++i,    "NetBSD Concatenated partition");
    Add("2DB519EC-B10F-11DC-B99B-0019D1879648",++i,    "NetBSD encrypted");

// ChromeOS
    Add("FE3A2A5D-4F32-41A7-B725-ACCC3285A309",++i,    "ChromeOS kernel"); // 50
    Add("3CB8E202-3B7E-47DD-8A3C-7FF2A13CFCEC",++i,    "ChromeOS rootfs");
    Add("2E0A753D-9E48-43B0-8337-B15192CB1B5E",++i,    "ChromeOS reserved");

}

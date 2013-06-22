#ifndef GUID_MANAGER_H
#define GUID_MANAGER_H

#include "definitions.h"
#include <vector>
#include <string>


struct GUID_DATA
{
    __GUID guid;
    std::string description;
};



class GuidManager
{
protected:
    std::vector<std::pair<GUID_DATA,uint32_t> > guid_data;

    void add_default_guids();

public:
    static std::string GuidToString(const __GUID &guid);
    static __GUID StringToGuid(std::string);

    GuidManager();

    void Add(__GUID,uint32_t);
    void Add(__GUID,uint32_t,std::string);
    void Add(std::string guid_str,uint32_t,std::string);
    void Add(std::string guid_str,uint32_t);


    __GUID   Get(uint32_t which);
    __GUID   GetByUID(uint32_t uid);
    uint32_t Count();

    void AddDescription(uint32_t which,std::string description);
    std::string GetDescription(uint32_t which);
    std::string GetDescription(const __GUID &guid);

    uint32_t GetUID(uint32_t which);

    std::string GetGuidAsString(uint32_t which);
};

#endif

#include "dllcall.hpp"


DllCall::DllCall()
{
    hInst=0;
    is_open=false;
}

DllCall::~DllCall()
{
    Close();
}

#if defined (_WIN32) || defined (WIN32)
bool DllCall::Open(const char *dll, bool no_errmsg)
{
   hInst=LoadLibrary(dll);
   if(hInst==0)
     {
         if(!no_errmsg)
             MessageBox(0,"Failed to load a DLL","Error",16);
         return false;
     }
   else
     {
        is_open=true;
        return true;
     }
}


void DllCall::Close()
{
    if(is_open)
        FreeLibrary(hInst);
}
#else // Linux

bool DllCall::Open(const char *dll, bool no_errmsg)
{

}


void DllCall::Close()
{

}


#endif

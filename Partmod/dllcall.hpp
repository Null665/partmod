#ifndef DLLCALL_HPP
#define DLLCALL_HPP

// TODO (Admin#1#): TODO: Write code for linux

#if defined (_WIN32) || defined (WIN32)
  #include <windows.h>
#else
  #define HINSTANCE int
#endif


class DllCall
{
HINSTANCE hInst;
bool is_open;

public:
  DllCall();
  ~DllCall();

  bool Open(const char *dll,bool no_errmsg=false);
  template<typename T> T GetProc(const char* function,bool no_errmsg=false);
  void Close();
};


#if defined (_WIN32) || defined (WIN32)
    template<typename T> T DllCall::GetProc(const char* function,bool no_errmsg)
    {
        T fp=(T)GetProcAddress(hInst,function);
        if(fp==0 && !no_errmsg)
            MessageBox(0,"Function not found in DLL","Error",16);

        return fp;
    }
#else


#endif


#endif // DLLCALL_HPP

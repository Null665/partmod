#ifndef GUI_H
#define GUI_H

#include <string>

#define WINVER    0x0600
#define _WIN32_IE 0x0600

#include <windows.h>
#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

//
// ListView columns
//

// For partition list
#define LVC_TYPE            1
#define LVC_FS_TYPE         2
#define LVC_SIZE            3
#define LVC_FREE            4
#define LVC_BEGIN           5
#define LVC_END             6
#define LVC_MOUNTPOINT      7

// For disk list
#define LVC_DISK_TYPE       1
#define LVC_DISK_SIZE       2
#define LVC_DISK_FREE       3
#define LVC_DISK_CYLINDERS  4
#define LVC_DISK_BPS        5
#define LVC_DISK_SPT        6
#define LVC_DISK_TPC        7
#define LVC_DISK_SECTORS    8
#define LVC_DISK_SIGNATURE  9




// Type of selection

#define S_PARTITION  1
#define S_FREE_SPACE 2





#define LV_InsertItem(wnd,str)\
{\
LVITEM tmp={LVIF_TEXT, 0,0, 0,0, (CHAR*)str,0, 0,0,0,0,0,0 };\
ListView_InsertItem(wnd, &tmp);\
}

#define LV_SetItem(wnd,str,item,column)\
{\
LVITEM tmp={LVIF_TEXT, item,column, 0,0, (CHAR*)str,0, 0,0,0,0,0,0 };\
ListView_SetItem(wnd, &tmp);\
}


struct sel_type
{
  int      type;
  uint32_t flags;
};

struct lvlist
{
std::string partition,
            type,
            fs_type,
            size,
            free,
            begin_sect,
            last_sect,
            mountpoint;

int num;
int pos;
sel_type selection_type;

};


void EnableButtons(HWND hwnd,bool enable,UINT buttons,...);
std::string GetFileName(bool open,HWND hwnd=0,const char *str_filter="All files\0*.*\0\0");
bool cmp_lv(lvlist a,lvlist b);



template<typename T> T *get_this(HWND hwnd)
{
   LONG_PTR lptr=GetWindowLong(hwnd,DWL_USER);
   return reinterpret_cast<T*>(lptr);

}

template<typename T> void init_set_this(HWND hwnd,LPARAM lParam)
{
   T *p= reinterpret_cast<T*>(lParam);
   SetWindowLong(hwnd,DWL_USER,(LONG_PTR)p);

}




class Dialog
{
public:
  void InitDialog(HINSTANCE hinst,HWND hwnd);

protected:
  Disk *disk;

  void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  void click_physical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
  void click_logical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
  void msg_hotkeys(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  BOOL customdraw_handler(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
  static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

};



class DlgPartmod:public Dialog
{
public:
  DlgPartmod();
  void InitDialog(HINSTANCE hinst,HWND hwnd);
protected:
  Disk *disk;
  int part_selected,frs_selected;
  std::vector <lvlist> listview_list;

  void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  void click_physical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
  void click_logical(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

  int ListPartitions(HWND );
  void ListDisks(HWND);
  sel_type get_sel_type(HWND);

  BOOL customdraw_handler(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
  static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};

//
// Dialog for creating a new partition
//
class DlgNewPart:public Dialog
{
public:
void InitDialog(Disk *disk,int frs_selected ,HINSTANCE hinst,HWND hwnd);

private:
  int frs_selected;

// functions
void wm_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

void btn_idok_click(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};

//
// Dialog for formating disk
//
class DlgFormatDisk:public Dialog
{
public:
void InitDialog(Disk *disk,int part_selected,HINSTANCE hinst,HWND hwnd);

private:
int part_selected;
void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

// Dialog's callback function
static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);


};




class DlgDiskImage: public Dialog
{
public:
void InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd);

private:
LONG_PTR old_app_state;

void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void set_fields(const char*filename,HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

void btn_idok_click(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);



};


class DlgBackup: public Dialog
{
public:
void InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd, bool create_backup);
private:

bool create_backup;


void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};




class DlgSave: public Dialog
{
public:
void InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd);
static DWORD WINAPI start(LPVOID);

static DWORD WINAPI Callback(LPVOID);

private:
static HWND hwnd;
static bool finished;

void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};



struct lv_errors
{
    std::string error_code,
           partition,
           partition_relevant,
           error_description;
    DISK_ERROR de;

};

class DlgCheckDisk
{
public:
void InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd);
private:
Disk *disk;
std::vector<lv_errors> lv_list;

void wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
void msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

void refresh(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

static BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
};



#endif

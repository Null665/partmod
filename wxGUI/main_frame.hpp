#ifndef MAIN_FRAME_HPP
#define MAIN_FRAME_HPP

#include <wx/wx.h>
#include <wx/listctrl.h>

#include <string>
#include <vector>
#include <stdint.h>

#include "../Partmod/disk.h"

#define S_PARTITION    1
#define S_FREE_SPACE   2





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

int      selection_type;
uint32_t flags;



};



class MainFrame: public wxFrame
{
protected:
  void create_menus();

  void create_disk_listctrl();
  void create_partition_listctrl();

  void refresh_partition_list();
  void refresh_disk_list();

  Disk *disk;

  wxPanel *mainPanel;
  wxListCtrl *diskList;
  wxListCtrl *partitionList;

    wxMenu *menuActions;
    wxMenu *menuDisk;
    wxMenu *menuPartition;
    wxMenu *menuTools;
    wxMenu *menuHelp;

  std::vector<lvlist> disk_structure; // list of partition and free space slices

  int selected_partition,selected_frs;

  static const long
    ID_QUIT ,
    ID_ABOUT ,

    ID_SAVE_CHANGES ,
    ID_CLOSE_DISK  ,
    ID_OPEN_DISK_IMAGE ,
    ID_CREATE_BACKUP ,
    ID_RESTORE_BACKUP ,
    ID_CHECK_DISK ,

    ID_CREATE_PARTITION  ,
    ID_DELETE_PARTITION ,
    ID_SET_ACTIVE ,
    ID_SET_INACTIVE ,
    ID_EDIT_BOOTSECTOR ,
    ID_CHECK_FS ,
    ID_FORMAT ,
    ID_WIPE_PARTITION ,

    ID_DISK_LIST ,
    ID_PARTITION_LIST ,

    ID_LIST_GUID ;


public:

    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnDiskListClick(wxListEvent& event);
    void OnPartitionListClick(wxListEvent& event);

    void OnPartitionListDeselect(wxListEvent& event);

    void OnPartitionListRightClick(wxListEvent& event);
    void OnPartitionListPopupClick(wxListEvent& event);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnNewPartition(wxCommandEvent& event);
    void OnOpenDiskImage(wxCommandEvent& event);

    void OnDiskCloseClick(wxCommandEvent& event);
    void OnSaveChangesClick(wxCommandEvent& event);

    void OnListGuidClick(wxCommandEvent& event);

    void OnSetActiveClick(wxCommandEvent& event);
    void OnUnsetActiveClick(wxCommandEvent& event);

    void OnCreateBackupClick(wxCommandEvent& event);
    void OnRestoreBackupClick(wxCommandEvent& event);

    void OnWipePartitionClick(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif

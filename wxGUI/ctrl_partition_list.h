#ifndef WXLISTVIEWPARTITIONLIST_H
#define WXLISTVIEWPARTITIONLIST_H

#include <wx/listctrl.h>
#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

#define S_PARTITION    1
#define S_FREE_SPACE   2

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


class wxPartitionList : public wxListView
{
public:
  wxPartitionList(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxListCtrlNameStr);
  virtual ~wxPartitionList();

  void Refresh();
  void AssignDisk(Disk *disk);
  int GetSelectedPartition();
  int GetSelectedFreeSpace();

  void HideMountPoint(bool hide=true);

protected:
  Disk *disk;
  std::vector<lvlist> disk_structure; // list of partition and free space slices
  int selected_partition,selected_frs;
};

#endif // WXLISTVIEWPARTITIONLIST_H

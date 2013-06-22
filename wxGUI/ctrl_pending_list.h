#ifndef CTRL_PENDING_LIST_H
#define CTRL_PENDING_LIST_H

#include <wx/listctrl.h>
#include "../Partmod/disk.h"
//#include "../Partmod/disk_exception.h"

class wxPendingList : public wxListView
{
public:
  wxPendingList(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxListCtrlNameStr);
  virtual ~wxPendingList();

  void Refresh();
  void AssignDisk(Disk *disk);

protected:
  Disk *disk;
};


#endif // CTRL_PENDING_LIST_H

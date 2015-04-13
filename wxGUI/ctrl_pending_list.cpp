#include "ctrl_pending_list.h"
#include <sstream>
#include "../Partmod/numstr.h"
using std::stringstream;


wxPendingList::wxPendingList(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString &name) : wxListView(parent,winid,pos,size,style,validator,name)
{
   InsertColumn(0,_("#"),0,70);
   InsertColumn(1,_("Action"),0,100);
   InsertColumn(2,_("Description"),0,250);


   disk=NULL;
}

wxPendingList::~wxPendingList()
{
    //dtor
}


void wxPendingList::AssignDisk(Disk *disk)
{
  this->disk=disk;
}

void wxPendingList::Refresh()
{
    this->DeleteAllItems();

    if(disk==NULL)
        return;
    if(disk->IsOpen()==false || disk->CountPendingActions()==0)
        return;


    for(unsigned i=0;i<disk->CountPendingActions();i++)
    {
        PENDING p=disk->GetPending(i);

        InsertItem(i,to_string(i));

        stringstream ss;
        switch(p.action)
        {
          case PENDING_WIPE:
              SetItem(i,1,"Wipe");
              SetItemTextColour(i,wxColor(240,0,0));
              break;
          case PENDING_MOVE:
              SetItem(i,1,"Move");
              break;
          case PENDING_CREATE:
              SetItem(i,1,"Create");
              break;
          case PENDING_DELETE:
              SetItem(i,1,"Delete");
              break;
          case PENDING_FLAGS:
              SetItem(i,1,"Modify flags");
              break;
          default:
              SetItem(i,1,"#unknown");
              break;
        }

       if(p.action!=PENDING_FLAGS)
           ss<<"partition (begin: "<<p.gpart.begin_sector<<", size: "<<size_str(p.gpart.length,disk->GetDiskGeometry().bps)<<")";
       else
         {
           uint32_t flags_curr=p.flags;
           uint32_t flags_old=p.flags_old;

           if(flags_curr&PART_ACTIVE && !(flags_old&PART_ACTIVE))
                ss <<"+PART_ACTIVE ";
           else ss <<"-PART_ACTIVE ";

         }
       SetItem(i,2,ss.str());
       ss.clear();
    }
}

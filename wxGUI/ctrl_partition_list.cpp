#include "ctrl_partition_list.h"
#include <algorithm>
using std::sort;

bool cmp_lv(lvlist a,lvlist b)
{
  return StrToU64((char*)a.begin_sect.c_str())<StrToU64((char*)b.begin_sect.c_str());
}



wxPartitionList::wxPartitionList(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString &name) : wxListView(parent,winid,pos,size,style,validator,name)
{
   InsertColumn(0,_("Partition #"),0,70);
   InsertColumn(1,_("Type"),0,100);
   InsertColumn(2,_("FS type"),0,90);
   InsertColumn(3,_("Size"),0,70);
    //InsertColumn(4,_("Free"),0,70);
   InsertColumn(5,_("First sect."),0,80);
   InsertColumn(6,_("Last sect."),0,80);
   InsertColumn(7,_("Mount point"),0,100);

   disk=NULL;
}

wxPartitionList::~wxPartitionList()
{
    //dtor
}


void wxPartitionList::AssignDisk(Disk *disk)
{
  this->disk=disk;
}

int wxPartitionList::GetSelectedPartition()
{
    if(disk==NULL)
        return -1;

	if(disk_structure[this->GetFocusedItem()].selection_type==S_PARTITION)
	    return disk_structure[this->GetFocusedItem()].num;
	else return -1;
}
int wxPartitionList::GetSelectedFreeSpace()
{
    if(disk==NULL)
        return -1;

	if(disk_structure[this->GetFocusedItem()].selection_type==S_FREE_SPACE)
	    return disk_structure[this->GetFocusedItem()].num;
	else return -1;
}



void wxPartitionList::Refresh()
{
    this->DeleteAllItems();
    disk_structure.clear();

    if(disk==NULL)
        return;
    if(disk->IsOpen()==false)
        return;

    for(int i=0;i<disk->CountPartitions();i++)
      {
         GEN_PART gpart=disk->GetPartition(i);
         lvlist tmp;

         if(gpart.flags&PART_PRIMARY && gpart.flags&PART_ACTIVE)
             tmp.type="Primary (Active)";
         else if(gpart.flags&PART_PRIMARY)
             tmp.type="Primary";
         else if(gpart.flags&PART_EXTENDED)
             tmp.type="Extended MBR";
         else if(gpart.flags&PART_LOGICAL)
             tmp.type="Logical";
         else if(gpart.flags&PART_MBR_GPT)
             tmp.type="GUID part. table";
         else if(gpart.flags&PART_GPT)
             tmp.type="GPT partition";

         if(gpart.flags&PART_EXTENDED || gpart.flags&PART_MBR_GPT)
             tmp.mountpoint="";
         else
             tmp.mountpoint=get_mount_point(gpart,disk->GetDiskSignature());

         tmp.partition=U64ToStr(i+1);
         tmp.begin_sect=U64ToStr(gpart.begin_sector);
         tmp.last_sect=U64ToStr(gpart.begin_sector+gpart.length);
         tmp.free="N/A";
         if(gpart.flags&PART_PRIMARY || gpart.flags&PART_EXTENDED || gpart.flags&PART_MBR_GPT || gpart.flags&PART_LOGICAL)
             tmp.fs_type=disk->fsid_man->GetByFsid(disk->GetMBRSpecific(i).fsid).description;
         else if(gpart.flags&PART_GPT)
           {
             try{
                tmp.fs_type=disk->guid_man->GetDescription(disk->GetGPTSpecific(i).type_guid);
             }catch(...)
             {
                 tmp.fs_type="[unknown guid]";
             }
           }
         else tmp.fs_type="[unknown]";
         tmp.num=i;
         tmp.selection_type=S_PARTITION;
         tmp.flags=gpart.flags;
         tmp.size=size_str(gpart.length,disk->GetDiskGeometry().bps).c_str();

         disk_structure.push_back(tmp);
      }
    for(int i=0;i<disk->CountFreeSpaces();i++)
      {
         FREE_SPACE frs=disk->GetFreeSpace(i);
         lvlist tmp;

         tmp.partition="-";
         tmp.type="Free space";
         tmp.fs_type="";
         tmp.mountpoint="";
         tmp.begin_sect=U64ToStr(frs.begin_sector);
         tmp.last_sect=U64ToStr(frs.begin_sector+frs.length);
         tmp.flags=frs.type;
         tmp.selection_type=S_FREE_SPACE;
         tmp.num=i;
         tmp.size=size_str(frs.length,disk->GetDiskGeometry().bps).c_str();
         disk_structure.push_back(tmp);
      }

    sort(disk_structure.begin(),disk_structure.end(),cmp_lv);

    for(int i=0;i<disk_structure.size();i++)
      {
          lvlist tmp=disk_structure[i];

          this->InsertItem(i,tmp.partition.c_str(),-1);
          this->SetItem(i,1,tmp.type.c_str(),-1);
          this->SetItem(i,2,tmp.fs_type.c_str(),-1);
          this->SetItem(i,3,tmp.size.c_str(),-1);
         // this->SetItem(i,4,tmp.free.c_str(),-1);
          this->SetItem(i,4,tmp.begin_sect.c_str(),-1);
          this->SetItem(i,5,tmp.last_sect.c_str(),-1);
          this->SetItem(i,6,tmp.mountpoint.c_str(),-1);

          if(tmp.selection_type==S_FREE_SPACE)
            {
               if(tmp.flags!=FREE_UNALLOCATED)
                   this->SetItemTextColour(i,wxColor(50,130,50));
               else this->SetItemTextColour(i,wxColor(50,50,50));
            }

          else
            {
               if(tmp.flags&PART_PRIMARY)
                   this->SetItemTextColour(i,wxColor(1,6,203));
               else if(tmp.flags&PART_EXTENDED || tmp.flags&PART_MBR_GPT)
                   this->SetItemTextColour(i,wxColor(240,0,0));
               else if(tmp.flags&PART_LOGICAL || tmp.flags&PART_GPT)
                   this->SetItemTextColour(i,wxColor(140,90,140));
            }
      }

}

void wxPartitionList::HideMountPoint(bool hide)
{
  static bool col_hidden=false;
  if(hide && col_hidden==false)
    {
      this->DeleteColumn(this->GetColumnCount()-1);
      col_hidden=true;
    }
  else if( !hide && col_hidden==true)
  {
     InsertColumn(7,_("Mount point"),0,100);
     col_hidden=false;
  }
}

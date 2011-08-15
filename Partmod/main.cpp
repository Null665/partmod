
#define WINVER 0x600
#define _WIN32_IE 0x600

#include <iostream>

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>



#include <cstdlib>
#include <winioctl.h>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;
#include "Partition.h"
#include "resource.h"
void UI_ListPartitions(Partition *part);
void UI_ListFreeSpace(Partition *part);
void UI_DeletePartition(Partition *part);
void UI_SaveChanges(Partition *part);
void UI_SelectDisk(Partition *part);
void UI_CreatePartition(Partition *part);
void UI_Help();







#if 0
int main()
{
stringstream ss("\\\\.\\PhysicalDrive0");
int disk_num;
int choice;

select_disk:
cout <<"Enter a disk number : (for \\\\.\\PhysicalDrive0 enter 0)  ";
cin >>disk_num;

ss.str("");
ss<<"\\\\.\\PhysicalDrive"<<disk_num;
part=new  Partition;
if(!part->Open(ss.str().c_str()))
  {
    cout <<"Could not open this disk, try again."<<endl;
    delete part;
    goto select_disk;
   }
part->CreatePartition(PART_IN_EXTENDED,5*NSECT_GB);

InitCommonControls();
DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_PARTMOD),0,PartmodDlgProc);

UI_Help();

while(true)
  {

    cout <<"\nEnter your choice:  ";
    cin >>choice;
    clrscr();
    if(choice==0) return 0;
    else if(choice==1) UI_ListPartitions(part);
    else if(choice==2) UI_ListFreeSpace(part);
    else if(choice==3) UI_CreatePartition(part);
    else if(choice==5) UI_DeletePartition(part);

    else if(choice==7) UI_SaveChanges(part);
    else if(choice==8) {delete part; UI_SelectDisk(part);}
    else if(choice==9) UI_Help();
    else cout <<"Invalid choice, press 9 for command list\n\n";
  }

return 0;

}

#endif


void UI_ListPartitions(Partition *part)
{
cout <<"Partition list"<<endl;
GEN_PART gpart;

cout <<"#"<<setw(12);
cout <<"Type"<<setw(15);
cout <<"FS type"<<setw(10);
cout <<"Size"<<setw(10);
cout <<"Status"<<setw(15);
cout <<"First sect."<<setw(15);
cout <<"Last sect."<<endl;
cout <<"------------------------------------------------------------------------------"<<endl;
for(int i=0;i<part->CountPartitions();i++)
  {
     gpart=part->GetPartition(i);

     cout <<i+1<<setw(12);
     if(gpart.flags&PART_PRIMARY)
        cout <<"Primary"<<setw(15);
     else if(gpart.flags&PART_EXTENDED)
        cout <<"Extended"<<setw(15);
     else if(gpart.flags&PART_IN_EXTENDED)
       cout <<"Extended"<<setw(15);
     cout <<systype_str(gpart.fs_type)<<setw(10);
     cout <<size_str(gpart.length)<<setw(10);
     if(gpart.flags&PART_ACTIVE)      cout <<"Active"<<setw(15);
     else cout <<"Inactive"<<setw(15);


     cout <<gpart.begin_sector<<setw(15);
     cout <<gpart.length+gpart.begin_sector;


     cout <<endl;
  }


cout <<endl;



}

void UI_ListFreeSpace(Partition *part)
{


FREE_SPACE frs;
unsigned __int64 total=0;
int fsc=part->CountFreeSpaces();
if(part->CountFreeSpaces()==0)
  {
    cout <<"There's no free space on disk.\n";
    return;
  }
cout <<"Free space found on disk "<<endl;
cout <<"#"<<setw(10);
cout <<"Size   "<<setw(40);
cout <<"Notes";
cout <<"\n---------------------------------------------------"<<endl;

for(int i=0;i<fsc;i++)
  {
     frs=part->EnumFreeSpace(i);
     cout <<i+1<<setw(10);
     cout <<size_str(frs.length)<<setw(40);
     if(frs.in_extended==true) cout <<"In extended partition";
     cout <<endl;
     total +=frs.length;


  }
cout <<"--------------------------\nFree space total: "<<size_str(total)<<endl<<endl;


}

void UI_DeletePartition(Partition *part)
{
int part_num;
char confirm;
cout <<"Enter number of partition you want to delete: ";
cin >>part_num;
part_num-=1;
if(part_num>part->CountPartitions() || part_num<0)
  {
    cout <<"Partition doesn't exist"<<endl;
    return;
  }
if(part->GetPartition(part_num).flags&PART_EXTENDED)
    cout <<"Warning: This is extended lba partition and it may contain other partitions"<<endl;

cout <<"Are you sure want to delete a partition? [Y/N]"<<endl;
cin >>confirm;
if(confirm=='Y' || confirm=='y')
   part->DeletePartition(part_num);


}


void UI_SaveChanges(Partition *part)
{
char confirm;
cout <<"Are you sure? [Y/N] ";
cin >>confirm;
if(confirm=='y' || confirm=='Y')
  {
    cout <<"Saving changes...";
    if(!part->Save())
    cout <<"ERROR: "<<error_table[part->get_error()]<<endl;
    else cout << "Finished successfuly"<<endl;
  }



}


void UI_Help()
{

    cout <<"What to do?"<<endl;
    cout <<"0 - Quit"<<endl;
    cout <<"1 - List partitions"<<endl;
    cout <<"2 - List free space"<<endl;
    cout <<"3 - Create a partition"<<endl;
    cout <<"5 - Delete a partition"<<endl;
    cout <<"7 - Write partition tables to disk. (Saves changes)"<<endl;
    cout <<"8 - Select other disk"<<endl;
    cout <<"9 - Display this information"<<endl;

}

void UI_SelectDisk(Partition *part)
{

stringstream ss;
int disk_num;


cout <<"Enter a disk number : (for \\\\.\\PhysicalDrive0 enter 0)  ";
cin >>disk_num;

ss.str("");
ss<<"\\\\.\\PhysicalDrive"<<disk_num;
part=new  Partition;
if(!part->Open(ss.str().c_str()))
  {
    cout <<"Could not open this disk, try again."<<endl;
    delete part;
  }



}


void UI_CreatePartition(Partition *part)
{
int input,
    part_type,
    num_primary=0;
unsigned __int64 size;
num_primary=part->CountPartitions(PART_PRIMARY);


cout <<"Create a partition"<<endl;
if(part->CountFreeSpaces()<=0)
  {
    cerr <<"There's no free space on disk"<<endl;
    return;
  }
if(num_primary==4)
    cout <<"Note: there are 4 primary partitons so you cannot create any more of them"<<endl;

cout <<"Enter type of partition you want to create: 1=primary, 2=extended mbr, 3=extended ebr"<<endl;
cin>>input;
if(input==1) part_type=PART_PRIMARY;
else if(input==2) part_type=PART_EXTENDED;
else if(input==3) part_type=PART_IN_EXTENDED;

cout <<"Enter size of partition in Megabytes: "<<endl;
cin>>size;
if(!part->CreatePartition(part_type,size))
  {
     cerr<<"Error: "<<error_table[part->get_error()]<<endl;
  }

}

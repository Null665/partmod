#include "gui.h"
#include "resource.h"

void DlgBackup::InitDialog(Disk *disk,HINSTANCE hinst,HWND hwnd, bool create_backup)
{
   this->create_backup=create_backup;
   this->disk=disk;
   DialogBoxParam(hinst,MAKEINTRESOURCE(IDD_BACKUP),hwnd,DlgProc,(LPARAM)this);
}




BOOL CALLBACK DlgBackup::DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
DlgBackup *backup=get_this<DlgBackup>(hwnd);

switch(message)
  {
     case WM_INITDIALOG:

          init_set_this<DlgBackup>(hwnd,lParam);
          backup=get_this<DlgBackup>(hwnd);

          if(!backup->disk->IsOpen())
            {
                MessageBox(hwnd,"Select a disk first","Information",64);
                EndDialog(hwnd,IDD_BACKUP);
            }
          backup->wm_init(hwnd,message,wParam,lParam);
          return true;

     case WM_COMMAND:
          backup->msg_buttons(hwnd,message,wParam,lParam);
          break;


   default: return false;
  }


}


void DlgBackup::wm_init(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(create_backup==true)
        SetWindowText(hwnd,"Create a partition table backup");
    else
        SetWindowText(hwnd,"Restore the partition table");



}






void DlgBackup::msg_buttons(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(LOWORD(wParam))
  {

      case IDB_BROWSE:
      {

         char *filter="Partition table backups (*.ptb)\0*.ptb\0All files\0*.*\0\0";
         char filename[2048];
         filename[0]='\0';
         OPENFILENAME ofn;
         memset(&ofn,0,sizeof(OPENFILENAME));

         ofn.lStructSize=sizeof(OPENFILENAME);
         ofn.hwndOwner=hwnd;
         ofn.lpstrFilter=filter;
         ofn.lpstrFile=filename;
         ofn.nFileExtension=0;
         ofn.nMaxFile=2048;
         ofn.lpstrDefExt=".ptb";
         ofn.Flags= OFN_EXPLORER;

         int ret;
         if(create_backup==true)
            ret=GetSaveFileName(&ofn);
         else
            ret=GetOpenFileName(&ofn);


         SetDlgItemText(hwnd,EDT_FILENAME,filename);
         break;
      }
     case IDOK:
       {
           char path[2048];

           try
           {


           if(create_backup)
               disk->CreateBackup(path);
           else
               disk->LoadBackup(path);
           }
           catch(DiskException &de)
           {
               MessageBox(hwnd,de.what(),"Runtime exception",16);
               break;
           }
           catch(...)
           {
               MessageBox(hwnd,"Failed to create/restore partition tale backup","Runtime exception",16);
               break;
           }
          MessageBox(hwnd,"Finished successfuly","",64);
       }
     case IDCANCEL:
       EndDialog(hwnd,IDD_BACKUP);
       break;


  }

}

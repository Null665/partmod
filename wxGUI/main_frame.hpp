#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

//(*Headers(MainFrame)
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#include "ctrl_partition_list.h"


#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"



class MainFrame: public wxFrame
{
	public:

		MainFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MainFrame();

		//(*Declarations(MainFrame)
		wxMenuItem* MenuOpenDiskImage;
		wxMenuItem* MenuListFsid;
		wxMenuItem* MenuAbout;
		wxListView* diskList;
		wxMenu* menuPartition;
		wxMenuItem* MenuRestoreBackup;
		wxMenuItem* MenuSaveChanges;
		wxMenu* menuActions;
		wxMenuItem* MenuSetInactive;
		wxMenuItem* MenuListGuid;
		wxMenuItem* MenuDeletePartition;
		wxPanel* Panel1;
		wxMenuItem* MenuUndoLastPending;
		wxPartitionList* partitionList;
		wxMenuItem* MenuCloseDisk;
		wxMenu* Menu1;
		wxMenuItem* MenuPartitionProperties;
		wxMenuItem* MenuSetActive;
		wxButton* ButtonSaveChanges;
		wxMenuItem* MenuShowPending;
		wxMenu* menuHelp;
		wxMenuItem* MenuCreateBackup;
		wxStatusBar* StatusBar1;
		wxMenu* menuTools;
		wxButton* ButtonQuit;
		wxMenu* menuBackup;
		wxMenu* menuDisk;
		wxMenuItem* MenuWipePartition;
		wxMenuItem* MenuUndoAllPending;
		wxMenuItem* MenuCreatePartition;
		wxMenuItem* MenuQuit;
		wxMenuItem* MenuRefreshDiskList;
		wxMenuBar* menuBar;
		//*)

	protected:

		//(*Identifiers(MainFrame)
		static const long ID_LISTVIEW1;
		static const long ID_LISTVIEW2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_PANEL1;
		static const long ID_SAVE_CHANGES;
		static const long ID_REFRESH_DISK_LIST;
		static const long ID_QUIT;
		static const long ID_CLOSE_DISK;
		static const long ID_OPEN_DISK_IMAGE;
		static const long ID_CREATE_PARTITION;
		static const long ID_DELETE_PARTITION;
		static const long ID_SET_ACTIVE;
		static const long ID_SET_INACTIVE;
		static const long ID_PARTITION_PROPERTIES;
		static const long ID_WIPE_PARTITION;
		static const long ID_MENUITEM3;
		static const long ID_MENUITEM4;
		static const long ID_MENUITEM5;
		static const long ID_CREATE_BACKUP;
		static const long ID_RESTORE_BACKUP;
		static const long ID_LIST_GUID;
		static const long ID_MENUITEM1;
		static const long ID_ABOUT;
		static const long ID_STATUSBAR1;
		//*)

	private:
	    Disk *disk;

		//(*Handlers(MainFrame)
		void OndiskListItemActivated(wxListEvent& event);
		void OnMenuCloseDiskSelected(wxCommandEvent& event);
		void OnMenuSaveChangesSelected(wxCommandEvent& event);
		void OnMenuQuitSelected(wxCommandEvent& event);
		void OnMenuOpenDiskImageSelected(wxCommandEvent& event);
		void OnMenuCreateBackupSelected(wxCommandEvent& event);
		void OnMenuRestoreBackupSelected(wxCommandEvent& event);
		void OnMenuCreatePartitionSelected(wxCommandEvent& event);
		void OnMenuDeletePartitionSelected(wxCommandEvent& event);
		void OnMenuSetActiveSelected(wxCommandEvent& event);
		void OnMenuSetInactiveSelected(wxCommandEvent& event);
		void OnMenuWipePartitionSelected(wxCommandEvent& event);
		void OnMenuListGuidSelected(wxCommandEvent& event);
		void OnMenuAboutSelected(wxCommandEvent& event);
		void OnPartitionListItemActivated(wxListEvent& event);
		void OnPartitionListItemRClick(wxListEvent& event);
		void OnMenuPartitionPropertiesSelected(wxCommandEvent& event);
		void OnPartitionListItemDeselect(wxListEvent& event);
		void OnMenuRefreshDiskListSelected(wxCommandEvent& event);
		void OnMenuListFsidSelected(wxCommandEvent& event);
		void OnMenuUndoPendingSelected(wxCommandEvent& event);
		void OnMenuShowPendingSelected(wxCommandEvent& event);
		void OnMenuUndoLastPendingSelected(wxCommandEvent& event);
		void OnMenuUndoAllPendingSelected(wxCommandEvent& event);
		//*)
        void OnPartitionListPopupClick(wxListEvent& event);

        void refresh_disk_list();
        void btn_disable_readonly();

		DECLARE_EVENT_TABLE()
};

#endif

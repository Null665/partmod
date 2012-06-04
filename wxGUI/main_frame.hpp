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

#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

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
	public:

		MainFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MainFrame();

		//(*Declarations(MainFrame)
		wxMenuItem* MenuOpenDiskImage;
		wxMenuItem* MenuAbout;
		wxListView* partitionList;
		wxListView* diskList;
		wxMenu* menuPartition;
		wxMenuItem* MenuRestoreBackup;
		wxMenuItem* MenuSaveChanges;
		wxMenu* menuActions;
		wxMenuItem* MenuSetInactive;
		wxButton* Button1;
		wxMenuItem* MenuListGuid;
		wxMenuItem* MenuDeletePartition;
		wxPanel* Panel1;
		wxMenuItem* MenuCloseDisk;
		wxMenuItem* MenuPartitionProperties;
		wxButton* Button2;
		wxMenuItem* MenuSetActive;
		wxMenu* menuHelp;
		wxMenuItem* MenuCreateBackup;
		wxStatusBar* StatusBar1;
		wxMenu* menuTools;
		wxMenu* menuDisk;
		wxMenuItem* MenuWipePartition;
		wxMenuItem* MenuCreatePartition;
		wxMenuItem* MenuQuit;
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
		static const long ID_QUIT;
		static const long ID_CLOSE_DISK;
		static const long ID_OPEN_DISK_IMAGE;
		static const long ID_CREATE_BACKUP;
		static const long ID_RESTORE_BACKUP;
		static const long ID_CREATE_PARTITION;
		static const long ID_DELETE_PARTITION;
		static const long ID_SET_ACTIVE;
		static const long ID_SET_INACTIVE;
		static const long ID_PARTITION_PROPERTIES;
		static const long ID_WIPE_PARTITION;
		static const long ID_LIST_GUID;
		static const long ID_ABOUT;
		static const long ID_STATUSBAR1;
		//*)

	private:
	    Disk *disk;
        std::vector<lvlist> disk_structure; // list of partition and free space slices
        int selected_partition,selected_frs;

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
		//*)
        void OnPartitionListPopupClick(wxListEvent& event);


        void refresh_disk_list();
        void refresh_partition_list();

		DECLARE_EVENT_TABLE()
};

#endif

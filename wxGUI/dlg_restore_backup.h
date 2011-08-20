#ifndef DLG_RESTORE_BACKUP_H
#define DLG_RESTORE_BACKUP_H

//(*Headers(DlgRestoreBackup)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

class DlgRestoreBackup: public wxDialog
{
	public:

		DlgRestoreBackup(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgRestoreBackup();

		//(*Declarations(DlgRestoreBackup)
		wxTextCtrl* TextBackupFileName;
		wxButton* ButtonBrowse;
		wxStaticText* StaticText1;
		wxFileDialog* FileDialog1;
		wxButton* ButtonCancel;
		wxButton* ButtonRestore;
		//*)
        int ShowModal(Disk *disk);
	protected:
        Disk *disk;
		//(*Identifiers(DlgRestoreBackup)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(DlgRestoreBackup)
		void OnButtonBrowseClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonRestoreClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

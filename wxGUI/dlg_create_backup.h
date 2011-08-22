#ifndef DLG_CREATE_BACKUP_H
#define DLG_CREATE_BACKUP_H

//(*Headers(DlgCreateBackup)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

class DlgCreateBackup: public wxDialog
{
	public:

		DlgCreateBackup(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~DlgCreateBackup();

		//(*Declarations(DlgCreateBackup)
		wxButton* ButtonClose;
		wxTextCtrl* TextBackupFileName;
		wxButton* ButtonBrowse;
		wxStaticText* StaticText1;
		wxFileDialog* FileDialog1;
		wxButton* ButtonCreateBackup;
		//*)

        int ShowModal(Disk *disk);
	protected:

		//(*Identifiers(DlgCreateBackup)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BROWSE;
		static const long ID_CREATE_BACKUP;
		static const long ID_CANCEL;
		//*)
        Disk *disk;
	private:

		//(*Handlers(DlgCreateBackup)
		void OnButtonBrowseClick(wxCommandEvent& event);
		void OnButtonCloseClick(wxCommandEvent& event);
		void OnButtonCreateBackupClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
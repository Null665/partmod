#ifndef DLG_CREATE_BACKUP_H
#define DLG_CREATE_BACKUP_H

//(*Headers(DlgCreateBackup)
#include <wx/sizer.h>
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
		wxTextCtrl* TextBackupFileName;
		wxButton* ButtonBrowse;
		wxStaticText* StaticText1;
		wxFileDialog* FileDialog1;
		wxTextCtrl* TextDescription;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

        int ShowModal(Disk *disk);
	protected:

		//(*Identifiers(DlgCreateBackup)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
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

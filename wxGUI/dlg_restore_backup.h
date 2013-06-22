#ifndef DLG_RESTORE_BACKUP_H
#define DLG_RESTORE_BACKUP_H

//(*Headers(DlgRestoreBackup)
#include <wx/sizer.h>
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
		wxStaticText* StaticText2;
		wxTextCtrl* TextBackupFileName;
		wxButton* Button1;
		wxButton* ButtonBrowse;
		wxStaticText* StaticText1;
		wxFileDialog* FileDialog1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxTextCtrl* TextDescription;
		wxTextCtrl* TextChecksum;
		wxTextCtrl* TextNumPart;
		wxTextCtrl* TextNumberOfSectors;
		wxStaticText* StaticText4;
		//*)
        int ShowModal(Disk *disk);
	protected:
        Disk *disk;
        wxString file_path;
        void update_fields();


		//(*Identifiers(DlgRestoreBackup)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL5;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(DlgRestoreBackup)
		void OnButtonBrowseClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonRestoreClick(wxCommandEvent& event);
		void OnTextBackupFileNameTextEnter(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

#ifndef DLG_OPEN_DISKIMAGE_H
#define DLG_OPEN_DISKIMAGE_H

//(*Headers(DlgOpenDiskImage)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"
#include "../Partmod/chs.hpp"


class DlgOpenDiskImage: public wxDialog
{
	public:

		DlgOpenDiskImage(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgOpenDiskImage();

		int ShowModal(Disk *disk);

		//(*Declarations(DlgOpenDiskImage)
		wxStaticText* StaticText2;
		wxTextCtrl* TextTPC;
		wxButton* ButtonBrowse;
		wxTextCtrl* TextImagePath;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxTextCtrl* TextBPS;
		wxStaticText* StaticText4;
		wxTextCtrl* TextSPT;
		wxTextCtrl* TextCylinders;
		//*)

	protected:

		//(*Identifiers(DlgOpenDiskImage)
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
		//*)

	private:
        Disk *disk;
		//(*Handlers(DlgOpenDiskImage)
		void OnButtonOKClick(wxCommandEvent& event);
		void OnButtonBrowseClick(wxCommandEvent& event);
		void OnTextImagePathTextEnter(wxCommandEvent& event);
		//*)
		void set_values();



		DECLARE_EVENT_TABLE()
};

#endif

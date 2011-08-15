#ifndef DLG_NEWPART_H
#define DLG_NEWPART_H

#include "../Partmod/disk.h"

//(*Headers(DlgNewPart)
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class DlgNewPart: public wxDialog
{
	public:

		DlgNewPart(wxWindow* parent);
		virtual ~DlgNewPart();

		int ShowModal(Disk *disk, int);

		//(*Declarations(DlgNewPart)
		wxStaticText* StaticText2;
		wxChoice* ChoicePartitionType;
		wxButton* ButtonOK;
		wxStaticText* StaticText1;
		wxChoice* ChoiceFsType;
		wxButton* ButtonCancel;
		wxChoice* ChoiceSizeMul;
		wxSpinCtrl* SpinCtrlPartitionSize;
		//*)

	protected:

		//(*Identifiers(DlgNewPart)
		static const long IDA_HSHDGSDHUF;
		static const long ID_SIZE_MULTIPLIER;
		static const long ID_STATICTEXT1;
		static const long ID_OK;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE2;
		//*)

	private:
        int selected_frs;
        Disk *disk;

		//(*Handlers(DlgNewPart)
		void OnButtonOKClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnChoicePartitionTypeSelect(wxCommandEvent& event);
		//*)


		DECLARE_EVENT_TABLE()
};

#endif

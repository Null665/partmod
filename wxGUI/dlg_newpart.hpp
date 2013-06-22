#ifndef DLG_NEWPART_H
#define DLG_NEWPART_H

#include "../Partmod/disk.h"

//(*Headers(DlgNewPart)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include "ctrl_partition.h"
#include <wx/spinctrl.h>
#include <wx/choice.h>
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
		wxSpinCtrl* SpinCtrlFreeFollowing;
		wxPartition* partitionView;
		wxStaticText* StaticText6;
		wxChoice* ChoicePartitionType;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlFreePrepending;
		wxChoice* ChoiceFsType;
		wxChoice* ChoiceAlignTo;
		wxStaticText* StaticText5;
		wxStaticText* StaticText4;
		wxChoice* ChoiceSizeMul;
		wxSpinCtrl* SpinCtrlPartitionSize;
		//*)

	protected:

		//(*Identifiers(DlgNewPart)
		static const long ID_CUSTOM1;
		static const long ID_STATICTEXT4;
		static const long ID_PARTITION_SIZE;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT5;
		static const long ID_FREE_PREPENDING;
		static const long ID_STATICTEXT6;
		static const long ID_FREE_FOLLOWING;
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE2;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE3;
		static const long ID_STATICTEXT3;
		static const long ID_CHOICE4;
		//*)

	private:
        int selected_frs;
        Disk *disk;

		//(*Handlers(DlgNewPart)
		void OnButtonOKClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnChoicePartitionTypeSelect(wxCommandEvent& event);
		void OnChoice2Select(wxCommandEvent& event);
		void OnPartitionViewMouseMove(wxMouseEvent& event);
		void OnSpinCtrlFreePrependingChange(wxSpinEvent& event);
		void OnChoiceSizeMulSelect(wxCommandEvent& event);
		void OnSpinCtrlFreeFollowingChange(wxSpinEvent& event);
		void OnSpinCtrlPartitionSizeChange(wxSpinEvent& event);
		//*)


		DECLARE_EVENT_TABLE()
};

#endif

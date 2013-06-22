#ifndef DLG_SAVE_CHANGES_H
#define DLG_SAVE_CHANGES_H

// I know I really need to fix this multithreaded mess but hey, if it aint't broke, don't fix it



//(*Headers(DlgSaveChanges)
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
//*)
#include <wx/msgdlg.h>
#include <wx/thread.h>

#include "ctrl_pending_list.h"

#include "../Partmod/disk.h"
#include "../Partmod/disk_exception.h"

class DlgSaveChanges;



class DiskSaveThread: public wxThread
{
private:
  Disk *disk;
public:
  DiskSaveThread(Disk *disk);
  void *Entry();
};

class ProgressThread: public wxThread
{
private:
  Disk *disk;
  DlgSaveChanges *dlg;
public:
  ProgressThread(Disk *disk,DlgSaveChanges *dlg);

  void *Entry();

};


class DlgSaveChanges: public wxDialog
{
	public:

		DlgSaveChanges(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgSaveChanges();

		int ShowModal(Disk *disk);

		//(*Declarations(DlgSaveChanges)
		wxStaticText* StaticText2;
		wxPendingList* pendingList;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* ButtonCancel;
		wxButton* ButtonConfirm;
		wxTextCtrl* TextLog;
		wxGauge* GaugeProgress;
		//*)

	protected:

		//(*Identifiers(DlgSaveChanges)
		static const long ID_GAUGE1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_OK;
		static const long ID_LISTVIEW1;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON1;
		//*)

	private:
       Disk *disk;
       ProgressThread *thread_progress;

		//(*Handlers(DlgSaveChanges)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonConfirmClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

};

#endif

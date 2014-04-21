#ifndef CTRL_PARTITION_H
#define CTRL_PARTITION_H

#include <wx/panel.h>
//#include <wx/sizer.h>

enum CTRL_STATE
{
  S_IDLE,
  S_LEFT_SLIDER,
  S_RIGHT_SLIDER,
  S_MOVE
};


class wxPartition : public wxPanel
{
    static const int slider_width = 10;

    int slider_left_pos;
    int slider_right_pos;

    CTRL_STATE state;

    unsigned n_steps;

    wxMouseEvent mouse_old_pos;
    bool enabled;

public:

  wxPartition(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxPanelNameStr);

    void SetNumberOfSteps(unsigned n_steps){this->n_steps=n_steps;}
    unsigned GetNumberOfSteps(){return n_steps;}

    bool SetLeftSliderPos(unsigned pos);
    bool SetRightSliderPos(unsigned pos);

    unsigned GetLeftSliderPos();
    unsigned GetRightSliderPos();

    bool Enable(bool enable=true);


    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void render(wxDC& dc);

    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void mouseLeftDoubleClick(wxMouseEvent& event);
    DECLARE_EVENT_TABLE()
};




#endif

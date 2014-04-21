#include "ctrl_partition.h"
#include <wx/wx.h>


BEGIN_EVENT_TABLE(wxPartition, wxPanel)

    EVT_MOTION(wxPartition::mouseMoved)
    EVT_LEFT_DOWN(wxPartition::mouseDown)
    EVT_LEFT_UP(wxPartition::mouseReleased)
    EVT_LEAVE_WINDOW(wxPartition::mouseLeftWindow)
    EVT_LEFT_DCLICK(wxPartition::mouseLeftDoubleClick)

    // catch paint events
    EVT_PAINT(wxPartition::paintEvent)

END_EVENT_TABLE()

wxPartition::wxPartition(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString &name) : wxPanel(parent,winid,pos,size,style,name)
{
    enabled=true;
    SetMinSize(size );
    n_steps=10000;
    state=S_IDLE;

    slider_left_pos=0;
    SetRightSliderPos(n_steps);
}


void wxPartition::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void wxPartition::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void wxPartition::render(wxDC&  dc)
{
    // Draw the background
    dc.SetBrush( *wxLIGHT_GREY_BRUSH );
    dc.DrawRectangle( 0, 0, GetSize().GetWidth(), GetSize().GetHeight() );

    // Fill the partition rectangle
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( slider_left_pos, 0, slider_right_pos-slider_left_pos, GetSize().GetHeight() );

    wxBrush sliderBrush;

    sliderBrush.SetColour(wxColor(1,6,203));
    dc.SetBrush( sliderBrush );
    // Draw the left and right slider
    dc.DrawRectangle(slider_left_pos,0,slider_width,GetSize().GetHeight());
    dc.DrawRectangle(slider_right_pos,0,-slider_width,GetSize().GetHeight());

    // Draw rectangles at the top and the bottom of control between sliders
    dc.DrawRectangle(slider_left_pos,0,slider_right_pos-slider_left_pos,10);
    dc.DrawRectangle(slider_left_pos,GetSize().GetHeight()-slider_width,slider_right_pos-slider_left_pos,10);
}

void wxPartition::mouseDown(wxMouseEvent& event)
{
    if(!enabled)
        return;

    if(event.m_x>=slider_left_pos && event.m_x<=slider_width+slider_left_pos)
        state=S_LEFT_SLIDER;
    else if (event.m_x>=slider_right_pos-slider_width && event.m_x<=slider_right_pos)
        state=S_RIGHT_SLIDER;
    else if(event.m_x>slider_left_pos && event.m_x<slider_right_pos)
        state=S_MOVE;

    mouse_old_pos=event;
}

void wxPartition::mouseReleased(wxMouseEvent& event)
{
    state=S_IDLE;
}
void wxPartition::mouseLeftWindow(wxMouseEvent& event)
{
    state=S_IDLE;
}

void wxPartition::mouseLeftDoubleClick(wxMouseEvent& event)
{
    slider_left_pos=0;
    SetRightSliderPos(n_steps-1);
}

void wxPartition::mouseMoved(wxMouseEvent& event)
{
  if(!enabled)
      return;

  if(state==S_LEFT_SLIDER)
  {
     if(slider_right_pos<slider_left_pos+slider_width && slider_left_pos<event.m_x)
         return;
      else if(slider_left_pos<0)
      {
          slider_left_pos=0;
          return;
      }

      slider_left_pos=event.m_x;
      this->Refresh();
  }
  else if(state==S_RIGHT_SLIDER)
  {
    if(event.m_x>GetSize().GetWidth())
        return;

     if(slider_right_pos<slider_left_pos+slider_width && slider_right_pos>event.m_x)
         return;

      slider_right_pos=event.m_x;
      this->Refresh();
  }
  else if(state==S_MOVE)
  {
      if(slider_left_pos<=0 && event.m_x<mouse_old_pos.m_x)
      {
        slider_left_pos=0;
        return;
      }
      else if(slider_right_pos>=GetSize().GetWidth() && event.m_x>mouse_old_pos.m_x)
      {
        slider_right_pos=GetSize().GetWidth();
        return;
      }

      slider_left_pos=slider_left_pos-(mouse_old_pos.m_x-event.m_x);
      slider_right_pos=slider_right_pos-(mouse_old_pos.m_x-event.m_x);
      mouse_old_pos=event;
      this->Refresh();
  }


}

// ------------------------------------------------

bool wxPartition::Enable(bool enable)
{
  this->enabled=enable;
  Refresh();
  return enabled; // TODO: check what wxWindow::Enable must actually return
}


bool wxPartition::SetLeftSliderPos(unsigned pos)
{
  if(pos>n_steps)
      return false;
  slider_left_pos=pos*float(GetSize().GetWidth())/n_steps;
  this->Refresh();
  return true;
}


bool wxPartition::SetRightSliderPos(unsigned pos)
{
  if(pos>n_steps)
      return false;
  slider_right_pos=pos*float(GetSize().GetWidth())/n_steps;
  this->Refresh();
  return true;
}

unsigned wxPartition::GetLeftSliderPos()
{
  double pos=(double)n_steps / (double)GetSize().GetWidth() * (double)slider_left_pos;
  return pos;
}
unsigned wxPartition::GetRightSliderPos()
{
  double pos=(double)n_steps / (double)GetSize().GetWidth() * (double)slider_right_pos;
  return pos;
}

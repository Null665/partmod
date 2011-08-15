#include "resource.h"
#include "gui.h"
#include <commctrl.h>

int main()
{
InitCommonControls();

DlgPartmod partmod;
partmod.InitDialog(GetModuleHandle(0),HWND_DESKTOP);

return 0;
}

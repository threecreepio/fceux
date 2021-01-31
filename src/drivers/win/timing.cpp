#include "common.h"
#include "main.h"
#include "gui.h"
#include "resource.h"
#include "fceu.h"

char str[5];
extern int newppu;

/**
* This function is called when the dialog closes.
*
* @param hwndDlg Handle of the timing dialog.
**/
void CloseTimingDialog(HWND hwndDlg)
{
	if(IsDlgButtonChecked(hwndDlg, CB_SET_HIGH_PRIORITY) == BST_CHECKED)
	{
		eoptions |= EO_HIGHPRIO;
	}
	else
	{
		eoptions &= ~EO_HIGHPRIO;
	}

	if(IsDlgButtonChecked(hwndDlg, CB_DISABLE_SPEED_THROTTLING)==BST_CHECKED)
	{
		eoptions |= EO_NOTHROTTLE;
	}
	else
	{
		eoptions &= ~EO_NOTHROTTLE;
	}

	aggressiveskip_disabled = (IsDlgButtonChecked(hwndDlg, CB_AGGRESSIVESKIP) == BST_CHECKED);
	overclock_enabled = (IsDlgButtonChecked(hwndDlg, CB_OVERCLOCKING) == BST_CHECKED);
	skip_7bit_overclocking = (IsDlgButtonChecked(hwndDlg, CB_SKIP_7BIT) == BST_CHECKED);

	GetDlgItemText(hwndDlg, IDC_EXTRA_SCANLINES, str, 4);
	sscanf(str,"%d",&postrenderscanlines);

	GetDlgItemText(hwndDlg, IDC_VBLANK_SCANLINES, str, 4);
	sscanf(str,"%d",&vblankscanlines);

	if (postrenderscanlines < 0)
	{
		postrenderscanlines = 0;
		MessageBox(hwndDlg, "Overclocking is when you speed up your CPU, not slow it down!", "Error", MB_OK | MB_ICONERROR);
		sprintf(str,"%d",postrenderscanlines);
		SetDlgItemText(hwndDlg,IDC_EXTRA_SCANLINES,str);
		SetFocus(GetDlgItem(hwndDlg, IDC_EXTRA_SCANLINES));
	}
	else if (vblankscanlines < 0)
	{
		vblankscanlines = 0;
		MessageBox(hwndDlg, "Overclocking is when you speed up your CPU, not slow it down!", "Error", MB_OK | MB_ICONERROR);
		sprintf(str,"%d",vblankscanlines);
		SetDlgItemText(hwndDlg,IDC_VBLANK_SCANLINES,str);
		SetFocus(GetDlgItem(hwndDlg, IDC_VBLANK_SCANLINES));
	}
	else if (overclock_enabled && newppu)
	{
		MessageBox(hwndDlg, "Overclocking doesn't work with new PPU!", "Error", MB_OK | MB_ICONERROR);
		SetFocus(GetDlgItem(hwndDlg, CB_OVERCLOCKING));
	}
	else
		EndDialog(hwndDlg, 0);

	totalscanlines = normalscanlines + (overclock_enabled ? postrenderscanlines : 0);
}

/**
* Callback function of the Timing configuration dialog.
**/
INT_PTR CALLBACK TimingConCallB(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:            

			// Update controls to the current settings.
			if(eoptions & EO_HIGHPRIO)
			{
				CheckDlgButton(hwndDlg, CB_SET_HIGH_PRIORITY, BST_CHECKED);
			}

			if(eoptions & EO_NOTHROTTLE)
			{
				CheckDlgButton(hwndDlg, CB_DISABLE_SPEED_THROTTLING, BST_CHECKED);
			}

			if(newppu)
			{
				EnableWindow(GetDlgItem(hwndDlg, CB_OVERCLOCKING), false);
			}

			if (aggressiveskip_disabled)
			{
				CheckDlgButton(hwndDlg, CB_AGGRESSIVESKIP, BST_CHECKED);
			}

			if(overclock_enabled)
			{
				CheckDlgButton(hwndDlg, CB_OVERCLOCKING, BST_CHECKED);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EXTRA_SCANLINES), true);
				EnableWindow(GetDlgItem(hwndDlg, CB_SKIP_7BIT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_VBLANK_SCANLINES), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_VBLANK_SCANLINES_TEXT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EXTRA_SCANLINES_TEXT), true);
			}
			if(skip_7bit_overclocking)
				CheckDlgButton(hwndDlg, CB_SKIP_7BIT, BST_CHECKED);

			SendDlgItemMessage(hwndDlg,IDC_EXTRA_SCANLINES, EM_SETLIMITTEXT,3,0);
			SendDlgItemMessage(hwndDlg,IDC_VBLANK_SCANLINES,EM_SETLIMITTEXT,3,0);

			sprintf(str,"%d",postrenderscanlines);
			SetDlgItemText(hwndDlg,IDC_EXTRA_SCANLINES,str);

			sprintf(str,"%d",vblankscanlines);
			SetDlgItemText(hwndDlg,IDC_VBLANK_SCANLINES,str);
			
			CenterWindowOnScreen(hwndDlg);

			break;

		case WM_CLOSE:
		case WM_QUIT:
			CloseTimingDialog(hwndDlg);
			break;

		case WM_COMMAND:

			if(!(wParam >> 16))
			{
				switch(wParam & 0xFFFF)
				{
					case 1:
						CloseTimingDialog(hwndDlg);
						break;
					case CB_OVERCLOCKING:
						bool chk = IsDlgButtonChecked(hwndDlg, CB_OVERCLOCKING) == BST_CHECKED;
						
						EnableWindow(GetDlgItem(hwndDlg, IDC_EXTRA_SCANLINES), chk);
						EnableWindow(GetDlgItem(hwndDlg, CB_SKIP_7BIT), chk);
						EnableWindow(GetDlgItem(hwndDlg, IDC_VBLANK_SCANLINES), chk);
						EnableWindow(GetDlgItem(hwndDlg, IDC_VBLANK_SCANLINES_TEXT), chk);
						EnableWindow(GetDlgItem(hwndDlg, IDC_EXTRA_SCANLINES_TEXT), chk);
						
						break;
				}
			}
	}

	return 0;
}

void DoTimingConfigFix()
{
	DoPriority();
}

/**
* Shows the Timing configuration dialog.
**/
void ConfigTiming()
{
	DialogBox(fceu_hInstance, "TIMINGCONFIG", hAppWnd, TimingConCallB);  
	DoTimingConfigFix();
}


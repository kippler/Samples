/******************************************************************
 *
 * Project: Helper
 * File: ComboBox.cpp
 * License: GPL - See LICENSE in the top level directory
 *
 * Wraps a standard combo box control. Provides additional
 * autocomplete functionality.
 *
 * Notes:
 * The combobox control is not ideal when it comes to
 * implementing autocomplete. It is possible to show
 * a dropdown list of matched terms while the user
 * is typing, and allow them to select an entry
 * from that list, but the approach is fairly buggy.
 * The combobox will alter the edit text and selection
 * on list box selection changes, as well as when the
 * list is shown/hidden.
 * Another weakness is that the caret cannot be positioned
 * correctly (see below).
 *
 * A better solution might be to use a rich-edit control
 * in combination with a list box.
 *
 * Written by David Erceg
 * www.explorerplusplus.com
 *
 *****************************************************************/

#include "stdafx.h"
#include "Helper.h"
#include "WindowHelper.h"
#include "ComboBoxHelper.h"
#include "ComboBox.h"
#include "Macros.h"


UINT_PTR CComboBox::m_StaticSubclassCounter = 0;

CComboBox *CComboBox::CreateNew(HWND hComboBox)
{
	return new CComboBox(hComboBox);
}

CComboBox::CComboBox(HWND hComboBox) :
CBaseWindow(hComboBox),
m_SubclassCounter(m_StaticSubclassCounter++),
m_SuppressAutocomplete(false)
{
	COMBOBOXINFO cbi;
	cbi.cbSize = sizeof(cbi);
	GetComboBoxInfo(m_hwnd,&cbi);
	SetWindowSubclass(cbi.hwndItem,ComboBoxEditProcStub,m_SubclassCounter,reinterpret_cast<DWORD_PTR>(this));

	/* Subclass the parent window so that the CBN_EDITCHANGE can be caught. */
	SetWindowSubclass(GetParent(m_hwnd),ComboBoxParentProcStub,m_SubclassCounter,reinterpret_cast<DWORD_PTR>(this));
}

CComboBox::~CComboBox()
{

}

INT_PTR CComboBox::OnDestroy()
{
	COMBOBOXINFO cbi;
	cbi.cbSize = sizeof(cbi);
	GetComboBoxInfo(m_hwnd,&cbi);
	RemoveWindowSubclass(cbi.hwndItem,ComboBoxEditProcStub,m_SubclassCounter);

	RemoveWindowSubclass(GetParent(m_hwnd),ComboBoxParentProcStub,m_SubclassCounter);

	return 0;
}

LRESULT CALLBACK ComboBoxEditProcStub(HWND hwnd,UINT uMsg,
	WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);

	CComboBox *pcb = reinterpret_cast<CComboBox *>(dwRefData);

	return pcb->ComboBoxEditProc(hwnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK CComboBox::ComboBoxEditProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_KEYDOWN:
		if(wParam == VK_BACK ||
			wParam == VK_DELETE)
		{
			m_SuppressAutocomplete = true;
		}
		break;
	}

	return DefSubclassProc(hwnd,Msg,wParam,lParam);
}

LRESULT CALLBACK ComboBoxParentProcStub(HWND hwnd,UINT uMsg,
	WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);

	CComboBox *pcb = reinterpret_cast<CComboBox *>(dwRefData);

	return pcb->ComboBoxParentProc(hwnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK CComboBox::ComboBoxParentProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMMAND:
		if(HIWORD(wParam) != 0)
		{
			switch(HIWORD(wParam))
			{
			case CBN_EDITCHANGE:
				if(reinterpret_cast<HWND>(lParam) == m_hwnd)
				{
					return OnCBNEditChange();
				}
				break;
			}
		}
		break;
	}

	return DefSubclassProc(hwnd,Msg,wParam,lParam);
}

INT_PTR CComboBox::OnCBNEditChange()
{
	if(m_SuppressAutocomplete)
	{
		m_SuppressAutocomplete = false;
		return 1;
	}

	std::wstring CurrentText;
	GetWindowString(m_hwnd,CurrentText);

	if(CurrentText.length() > 0)
	{
		std::list<std::wstring> StringEntries = NComboBox::ComboBox_GetStrings(m_hwnd);

		int Index = 0;

		for each(auto StringEntry in StringEntries)
		{
			if(StringEntry.compare(0,CurrentText.length(),CurrentText) == 0)
			{
				DWORD CurrentSelection = ComboBox_GetEditSel(m_hwnd);

				DWORD SelectionStart = LOWORD(CurrentSelection);
				DWORD SelectionEnd = HIWORD(CurrentSelection);

				/* Autocomplete will only be provided when typing at
				the end of the text. */
				if(SelectionStart == CurrentText.length() &&
					SelectionEnd == CurrentText.length())
				{
					/* The first matching entry will be used to autocomplete the text. */
					ComboBox_SetCurSel(m_hwnd,Index);

					/* It would be better to select the text in reverse (i.e.
					from the end of the string to the last character the
					user typed), so that the caret position would be just
					after the last character typed.
					Single-line edit controls don't seem to support this
					however, and the caret will always be placed at the
					end of the selection (given by whichever selection
					position is greater).
					Multi-line edit controls and rich edit controls don't
					have this limitation. */
					ComboBox_SetEditSel(m_hwnd,CurrentText.length(),-1);
				}

				break;
			}

			Index++;
		}
	}

	return 0;
}
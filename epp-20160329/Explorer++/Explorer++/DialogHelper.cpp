/******************************************************************
 *
 * Project: Explorer++
 * File: DialogHelper.cpp
 * License: GPL - See LICENSE in the top level directory
 *
 * Dialog helper functionality.
 *
 * Written by David Erceg
 * www.explorerplusplus.com
 *
 *****************************************************************/

#include "stdafx.h"
#include "Explorer++.h"
#include "DialogHelper.h"
#include "SearchDialog.h"
#include "WildcardSelectDialog.h"
#include "SetFileAttributesDialog.h"
#include "RenameTabDialog.h"
#include "MassRenameDialog.h"
#include "FilterDialog.h"
#include "ColorRuleDialog.h"
#include "CustomizeColorsDialog.h"
#include "SplitFileDialog.h"
#include "DestroyFilesDialog.h"
#include "MergeFilesDialog.h"
#include "SelectColumnsDialog.h"
#include "SetDefaultColumnsDialog.h"
#include "AddBookmarkDialog.h"
#include "DisplayColoursDialog.h"
#include "UpdateCheckDialog.h"
#include "../Helper/XMLSettings.h"


namespace
{
	const TCHAR DIALOGS_REGISTRY_KEY[] = _T("Software\\Explorer++\\Dialogs");
	const TCHAR DIALOGS_XML_KEY[] = _T("State");

	/* Safe provided that the object returned through
	GetInstance is stable throughout the lifetime of
	the program (which is true, as these are all
	singletons). */
	CDialogSettings* const DIALOG_SETTINGS[] = {
		&CSearchDialogPersistentSettings::GetInstance(),
		&CWildcardSelectDialogPersistentSettings::GetInstance(),
		&CSetFileAttributesDialogPersistentSettings::GetInstance(),
		&CRenameTabDialogPersistentSettings::GetInstance(),
		&CMassRenameDialogPersistentSettings::GetInstance(),
		&CFilterDialogPersistentSettings::GetInstance(),
		&CColorRuleDialogPersistentSettings::GetInstance(),
		&CCustomizeColorsDialogPersistentSettings::GetInstance(),
		&CSplitFileDialogPersistentSettings::GetInstance(),
		&CDestroyFilesDialogPersistentSettings::GetInstance(),
		&CMergeFilesDialogPersistentSettings::GetInstance(),
		&CSelectColumnsDialogPersistentSettings::GetInstance(),
		&CSetDefaultColumnsDialogPersistentSettings::GetInstance(),
		&CAddBookmarkDialogPersistentSettings::GetInstance(),
		&CDisplayColoursDialogPersistentSettings::GetInstance(),
		&CUpdateCheckDialogPersistentSettings::GetInstance()
	};
}

void Explorerplusplus::LoadDialogStatesFromRegistry(void)
{
	HKEY hKey;
	LONG ReturnValue = RegOpenKeyEx(HKEY_CURRENT_USER,DIALOGS_REGISTRY_KEY,0,KEY_READ,&hKey);

	if(ReturnValue == ERROR_SUCCESS)
	{
		for each(CDialogSettings *ds in DIALOG_SETTINGS)
		{
			ds->LoadRegistrySettings(hKey);
		}

		RegCloseKey(hKey);
	}
}

void Explorerplusplus::SaveDialogStatesToRegistry(void)
{
	HKEY hKey;
	LONG ReturnValue = RegCreateKeyEx(HKEY_CURRENT_USER,DIALOGS_REGISTRY_KEY,
		0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,NULL);

	if(ReturnValue == ERROR_SUCCESS)
	{
		for each(CDialogSettings *ds in DIALOG_SETTINGS)
		{
			ds->SaveRegistrySettings(hKey);
		}

		RegCloseKey(hKey);
	}
}

void Explorerplusplus::LoadDialogStatesFromXML(MSXML2::IXMLDOMDocument *pXMLDom)
{
	MSXML2::IXMLDOMNodeList		*pNodes = NULL;
	MSXML2::IXMLDOMNode			*pNode = NULL;
	MSXML2::IXMLDOMNamedNodeMap	*am = NULL;
	MSXML2::IXMLDOMNode			*pChildNode = NULL;
	BSTR						bstrName;
	BSTR						bstrValue;
	BSTR						bstr = NULL;
	HRESULT						hr;
	long						length;
	long						lChildNodes;

	if(pXMLDom == NULL)
		goto clean;

	TCHAR tempNodeSelector[64];
	StringCchPrintf(tempNodeSelector, SIZEOF_ARRAY(tempNodeSelector),
		_T("//%s/*"), DIALOGS_XML_KEY);
	bstr = SysAllocString(tempNodeSelector);
	pXMLDom->selectNodes(bstr,&pNodes);

	if(!pNodes)
	{
		goto clean;
	}
	else
	{
		pNodes->get_length(&length);

		for(long i = 0;i < length;i++)
		{
			/* This should never fail, as the number
			of nodes has already been counted (so
			they must exist). */
			hr = pNodes->get_item(i,&pNode);

			if(SUCCEEDED(hr))
			{
				hr = pNode->get_attributes(&am);

				if(SUCCEEDED(hr))
				{
					/* Retrieve the total number of attributes
					attached to this node. */
					am->get_length(&lChildNodes);

					if(lChildNodes >= 1)
					{
						am->get_item(0,&pChildNode);

						pChildNode->get_nodeName(&bstrName);
						pChildNode->get_text(&bstrValue);

						for each(CDialogSettings *ds in DIALOG_SETTINGS)
						{
							TCHAR settingsKey[64];
							bool success = ds->GetSettingsKey(settingsKey, SIZEOF_ARRAY(settingsKey));
							assert(success);

							if(!success)
							{
								continue;
							}

							if(lstrcmpi(bstrValue, settingsKey) == 0)
							{
								ds->LoadXMLSettings(am, lChildNodes);
							}
						}
					}
				}
			}

			pNode->Release();
			pNode = NULL;
		}
	}

clean:
	if (bstr) SysFreeString(bstr);
	if (pNodes) pNodes->Release();
	if (pNode) pNode->Release();
}

void Explorerplusplus::SaveDialogStatesToXML(MSXML2::IXMLDOMDocument *pXMLDom,
MSXML2::IXMLDOMElement *pRoot)
{
	MSXML2::IXMLDOMElement	*pe = NULL;
	BSTR					bstr = NULL;
	BSTR					bstr_wsnt = SysAllocString(L"\n\t");

	NXMLSettings::AddWhiteSpaceToNode(pXMLDom,bstr_wsnt,pRoot);

	bstr = SysAllocString(DIALOGS_XML_KEY);
	pXMLDom->createElement(bstr,&pe);
	SysFreeString(bstr);
	bstr = NULL;

	for each(CDialogSettings *ds in DIALOG_SETTINGS)
	{
		ds->SaveXMLSettings(pXMLDom, pe);
	}

	NXMLSettings::AddWhiteSpaceToNode(pXMLDom,bstr_wsnt,pe);

	NXMLSettings::AppendChildToParent(pe,pRoot);
	pe->Release();
	pe = NULL;
}
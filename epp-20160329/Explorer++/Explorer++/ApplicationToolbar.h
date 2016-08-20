#pragma once

#include <vector>
#include "Explorer++_internal.h"
#include "ApplicationToolbarDropHandler.h"
#include "../Helper/BaseWindow.h"

#import <msxml3.dll> raw_interfaces_only

class CApplicationToolbar;
class CApplicationToolbarDropHandler;

struct ApplicationButton_t
{
	std::wstring	Name;
	std::wstring	Command;
	BOOL			ShowNameOnToolbar;

	int				ID;
};

class CApplicationToolbarPersistentSettings
{
public:

	~CApplicationToolbarPersistentSettings();

	static CApplicationToolbarPersistentSettings &GetInstance();

	void	SaveRegistrySettings(HKEY hParentKey);
	void	LoadRegistrySettings(HKEY hParentKey);

	void	SaveXMLSettings(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void	LoadXMLSettings(MSXML2::IXMLDOMNode *pNode);

private:

	friend CApplicationToolbar;

	static const TCHAR SETTING_NAME[];
	static const TCHAR SETTING_COMMAND[];
	static const TCHAR SETTING_SHOW_NAME_ON_TOOLBAR[];

	CApplicationToolbarPersistentSettings();

	CApplicationToolbarPersistentSettings(const CApplicationToolbarPersistentSettings &);
	CApplicationToolbarPersistentSettings & operator=(const CApplicationToolbarPersistentSettings &);

	bool AddButton(const std::wstring &name, const std::wstring &command,
		BOOL showNameOnToolbar, ApplicationButton_t *buttonOut);

	std::vector<ApplicationButton_t> m_Buttons;
	int m_IDCounter;
};

class CApplicationToolbar : public CBaseWindow
{
	friend LRESULT CALLBACK ParentWndProcStub(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData);

public:

	static CApplicationToolbar *Create(HWND hParent, UINT uIDStart, UINT uIDEnd, HINSTANCE hInstance, IExplorerplusplus *pexpp);

	void				ShowNewItemDialog();
	void				AddNewItem(const std::wstring &name, const std::wstring &command, BOOL showNameOnToolbar);
	void				OpenItem(int iItem, std::wstring *parameters);
	void				ShowItemProperties(int iItem);
	void				DeleteItem(int iItem);

private:

	static const UINT_PTR PARENT_SUBCLASS_ID = 0;

	struct ApplicationInfo_t
	{
		std::wstring Application;
		std::wstring Parameters;
	};

	LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CApplicationToolbar(HWND hParent, UINT uIDStart, UINT uIDEnd, HINSTANCE hInstance, IExplorerplusplus *pexpp);
	~CApplicationToolbar();

	static HWND			CreateApplicationToolbar(HWND hParent);

	void				Initialize(HWND hParent);

	void				AddButtonsToToolbar();
	void				AddButtonToToolbar(const ApplicationButton_t &Button);
	void				UpdateButton(int iItem);

	ApplicationInfo_t	ProcessCommand(const std::wstring &Command);
	ApplicationButton_t	*MapToolbarButtonToItem(int index);

	HINSTANCE			m_hInstance;

	UINT				m_uIDStart;
	UINT				m_uIDEnd;

	int					m_RightClickItem;

	IExplorerplusplus	*m_pexpp;

	CApplicationToolbarDropHandler	*m_patd;

	CApplicationToolbarPersistentSettings	*m_atps;
};
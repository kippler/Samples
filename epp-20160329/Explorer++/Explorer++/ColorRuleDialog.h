#pragma once

#include "CustomizeColorsDialog.h"
#include "../Helper/BaseDialog.h"
#include "../Helper/DialogSettings.h"

class CColorRuleDialog;

class CColorRuleDialogPersistentSettings : public CDialogSettings
{
public:

	~CColorRuleDialogPersistentSettings();

	static CColorRuleDialogPersistentSettings &GetInstance();

private:

	friend CColorRuleDialog;

	static const TCHAR SETTINGS_KEY[];
	static const COLORREF DEFAULT_INITIAL_COLOR;

	static const TCHAR SETTING_INITIAL_COLOR[];
	static const TCHAR SETTING_CUSTOM_COLORS[];

	CColorRuleDialogPersistentSettings();

	CColorRuleDialogPersistentSettings(const CColorRuleDialogPersistentSettings &);
	CColorRuleDialogPersistentSettings & operator=(const CColorRuleDialogPersistentSettings &);

	void		SaveExtraRegistrySettings(HKEY hKey);
	void		LoadExtraRegistrySettings(HKEY hKey);

	void		SaveExtraXMLSettings(MSXML2::IXMLDOMDocument *pXMLDom, MSXML2::IXMLDOMElement *pParentNode);
	void		LoadExtraXMLSettings(BSTR bstrName, BSTR bstrValue);

	COLORREF	m_cfInitialColor;
	COLORREF	m_cfCustomColors[16];
};

class CColorRuleDialog : public CBaseDialog
{
public:

	CColorRuleDialog(HINSTANCE hInstance,int iResource,HWND hParent,NColorRuleHelper::ColorRule_t *pColorRule,BOOL bEdit);
	~CColorRuleDialog();

	LRESULT CALLBACK	StaticColorProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);

protected:

	INT_PTR	OnInitDialog();
	INT_PTR	OnCommand(WPARAM wParam,LPARAM lParam);
	INT_PTR	OnClose();

	void	SaveState();

private:

	void	OnChangeColor();

	void	OnOk();
	void	OnCancel();

	BOOL		m_bEdit;
	NColorRuleHelper::ColorRule_t	*m_pColorRule;

	COLORREF	m_cfCurrentColor;

	CColorRuleDialogPersistentSettings	*m_pcrdps;
};
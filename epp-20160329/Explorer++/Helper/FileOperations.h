#pragma once

#include <list>

namespace NFileOperations
{
	enum OverwriteMethod_t
	{
		OVERWRITE_ONEPASS	= 1,
		OVERWRITE_THREEPASS	= 2
	};

	BOOL	RenameFile(const std::wstring &strOldFilename,const std::wstring &strNewFilename);
	BOOL	DeleteFiles(HWND hwnd,const std::list<std::wstring> &FullFilenameList,BOOL bPermanent,BOOL bSilent);
	void	DeleteFileSecurely(const std::wstring &strFilename,OverwriteMethod_t uOverwriteMethod);
	BOOL	CopyFilesToFolder(HWND hOwner,const std::wstring &strTitle,const std::list<std::wstring> &FullFilenameList,BOOL bMove);

	TCHAR	*BuildFilenameList(const std::list<std::wstring> &FilenameList);

	BOOL	SaveDirectoryListing(const std::wstring &strDirectory,const std::wstring &strFilename);

	HRESULT	CreateLinkToFile(const std::wstring &strTargetFilename,const std::wstring &strLinkFilename,const std::wstring &strLinkDescription);
	HRESULT	ResolveLink(HWND hwnd,DWORD fFlags,TCHAR *szLinkFilename,TCHAR *szResolvedPath,int nBufferSize);

	BOOL	CreateBrowseDialog(HWND hOwner,const std::wstring &strTitle,std::wstring &strOutputFilename);
	BOOL	CreateBrowseDialog(HWND hOwner,const std::wstring &strTitle,LPITEMIDLIST *ppidl);
};

HRESULT	CreateNewFolder(const TCHAR *Directory, TCHAR *szNewFolderName, int cchMax);

HRESULT	CopyFiles(const std::list<std::wstring> &FileNameList, IDataObject **pClipboardDataObject);
HRESULT	CutFiles(const std::list<std::wstring> &FileNameList, IDataObject **pClipboardDataObject);
HRESULT	CopyFilesToClipboard(const std::list<std::wstring> &FileNameList, BOOL bMove, IDataObject **pClipboardDataObject);

int		PasteLinksToClipboardFiles(const TCHAR *szDestination);
int		PasteHardLinks(const TCHAR *szDestination);
#pragma once

#include <list>
#include "Macros.h"

/* Within a resizable dialog, controls
either: move, resize, or hold the same
size and position.
For controls that do move/resize, they
may be constrained along one axis.
For example, a particular control may
resize/move horizontally, but not vertically. */
class CResizableDialog
{
public:

	enum ControlType
	{
		TYPE_MOVE,
		TYPE_RESIZE
	};

	enum ControlConstraint
	{
		CONSTRAINT_NONE,
		CONSTRAINT_X,
		CONSTRAINT_Y
	};

	struct Control_t
	{
		int					iID;
		ControlType			Type;
		ControlConstraint	Constraint;
	};

	CResizableDialog(HWND hDlg,const std::list<Control_t> &ControlList);
	~CResizableDialog();

	void	UpdateControls(int iWidth,int iHeight);

private:

	DISALLOW_COPY_AND_ASSIGN(CResizableDialog);

	struct ControlInternal_t
	{
		int					iID;
		ControlType			Type;
		ControlConstraint	Constraint;

		int					iWidthDelta;
		int					iHeightDelta;
	};

	const HWND m_hDlg;
	std::list<ControlInternal_t> m_ControlList;
};
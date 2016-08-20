#pragma once

#define DEFAULT_ITEM_ALLOCATION		100

/* Timer information. */
#define DIRECTORYMODIFIED_TIMER_ID		0
#define DIRECTORYMODIFIED_TIMER_ELAPSE	500

#define DRAGEXPAND_TIMER_ID		1
#define DRAGEXPAND_TIMER_ELAPSE	800

#define DRAGSCROLL_TIMER_ID		2
#define DRAGSCROLL_TIMER_ELAPSE	1000

typedef struct
{
	HWND			hTreeView;
	LPITEMIDLIST	pidlFull;
	HTREEITEM		hItem;
} TreeViewInfo_t;
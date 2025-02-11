//------------------------------------------------------------------------------
//  hostwindow.cc
//  (C) 2010 xoyojank
//------------------------------------------------------------------------------
#include "stdeditor.h"
#include "editor/control/hostwindow.h"
#include "editor/control/windowmessagehandler.h"

//------------------------------------------------------------------------------
namespace Editor
{

//------------------------------------------------------------------------------
namespace Control
{
using namespace Input;

//------------------------------------------------------------------------------
HostWindow::HostWindow() :
	wndMsgHandler(NULL),
	hParentWnd(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
HandleRef
HostWindow::BuildWindowCore(HandleRef hwndParent)
{
	this->hParentWnd = (HWND)hwndParent.Handle.ToPointer();

	// register window class
	WNDCLASS wndClass = {0};
	wndClass.style			= CS_DBLCLKS;
	wndClass.lpfnWndProc	= DefWindowProc;
	wndClass.hInstance		= GetModuleHandle(NULL);
	wndClass.hbrBackground	= (HBRUSH) GetStockObject(NULL_BRUSH);
	wndClass.lpszClassName	= NEBULA3_WINDOW_CLASS;
	RegisterClass(&wndClass);
	// create the parent window
	RECT rect;
	GetClientRect(this->hParentWnd, &rect);
	HWND hWnd = CreateWindow(NEBULA3_WINDOW_CLASS,
	                         NULL,
	                         WS_CHILD | WS_VISIBLE,
	                         0,
	                         0,
	                         rect.right - rect.left,
	                         rect.bottom - rect.top,
	                         this->hParentWnd,
	                         NULL,
	                         NULL,
	                         NULL);
	if (NULL == hWnd)
		return HandleRef(nullptr, IntPtr::Zero);
	// create message handler
	this->wndMsgHandler = new WindowMessageHandler(hWnd);

	return HandleRef(this, IntPtr(hWnd));
}

//------------------------------------------------------------------------------
void
HostWindow::DestroyWindowCore(HandleRef hwnd)
{
	// destroy handler
	delete this->wndMsgHandler;
	this->wndMsgHandler = NULL;
}

//------------------------------------------------------------------------------
IntPtr
HostWindow::WndProc(IntPtr hwnd, int msg, IntPtr wparam, IntPtr lparam, bool% handled)
{
	handled = false;
	WPARAM wParam = (WPARAM)wparam.ToPointer();
	LPARAM lParam = (LPARAM)lparam.ToPointer();
	switch (msg)
	{
	case WM_ERASEBKGND:
		// prevent Windows from erasing the background
		handled = true;
		break;

	case WM_SIZE:
		{
			// inform input server about focus change
			if ((SIZE_MAXHIDE == wParam) || (SIZE_MINIMIZED == wParam))
			{
				this->wndMsgHandler->OnMinimized();
			}
			else
			{
				this->wndMsgHandler->OnRestored();
			}
		}
		break;

	case WM_PAINT:
		this->wndMsgHandler->OnPaint();
		break;

	case WM_SETFOCUS:
		this->wndMsgHandler->OnSetFocus();
		break;

	case WM_KILLFOCUS:
		this->wndMsgHandler->OnKillFocus();
		break;

	case WM_CLOSE:
		this->wndMsgHandler->OnCloseRequested();
		break;

	case WM_KEYDOWN:
		this->wndMsgHandler->OnKeyDown(wParam);
		break;

	case WM_KEYUP:
		this->wndMsgHandler->OnKeyUp(wParam);
		break;

	case WM_CHAR:
		this->wndMsgHandler->OnChar(wParam);
		break;

	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		this->wndMsgHandler->OnMouseButton(msg, lParam);
		break;

	case WM_MOUSEMOVE:
		this->wndMsgHandler->OnMouseMove(lParam);
		break;

	case WM_MOUSEWHEEL:
		this->wndMsgHandler->OnMouseWheel(wParam);
		break;
	}
	return IntPtr::Zero;
}

}// Control

}// Editor
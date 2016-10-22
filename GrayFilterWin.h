#pragma once

#include <windows.h>
#include <wincodec.h>
#include <magnification.h>

#define MAGFACTOR  1.0f

const TCHAR         WindowClassName[] = TEXT("MagnifierWindow");
const TCHAR         WindowTitle[] = TEXT("Screen Magnifier Sample");
const UINT          timerInterval = 16; // close to the refresh rate @60hz

class GrayFilterWin
{
public:
	GrayFilterWin();
	~GrayFilterWin();
	static ATOM RegisterHostWindowClass();
	BOOL SetupMagnifier(HWND);

private:
	void hide();
	static void CALLBACK UpdateMagWindow(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/);
	static LRESULT CALLBACK HostWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hwndParent;
	HWND	m_hwndMag;
	HWND	m_hwndHost;
	RECT	m_magWindowRect;
	RECT	m_hostWindowRect;
	UINT_PTR m_timerId;
	bool	m_isInit;
};


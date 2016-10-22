#include "GrayFilterWin.h"



GrayFilterWin::GrayFilterWin() : m_isInit(false)
{
}


GrayFilterWin::~GrayFilterWin()
{
	hide();
	::KillTimer(m_hwndHost, m_timerId);
	::DestroyWindow(m_hwndMag);
	::DestroyWindow(m_hwndHost);
}

void GrayFilterWin::hide()
{
	SetWindowPos(m_hwndHost, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE);

	::ShowWindow(m_hwndMag, SW_HIDE);
}

BOOL GrayFilterWin::SetupMagnifier(HWND parent)
{
	if (m_isInit) return true;
	if (!::IsWindow(parent) || !::IsWindowVisible(parent)) return false;
	m_hwndParent = parent;
	// Set bounds of host window according to screen size.
	RECT parentRect;
	::GetWindowRect(parent, &parentRect);
	m_hostWindowRect = parentRect;

	m_hwndHost = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		WindowClassName, WindowTitle,
		WS_POPUP,
		m_hostWindowRect.left, m_hostWindowRect.top, m_hostWindowRect.right - m_hostWindowRect.left, m_hostWindowRect.bottom - m_hostWindowRect.top, NULL, NULL, NULL, NULL);
	if (!m_hwndHost)
	{
		return FALSE;
	}

	// Make the window opaque.
	SetLayeredWindowAttributes(m_hwndHost, 0, 255, LWA_ALPHA);

	// Create a magnifier control that fills the client area.
	GetClientRect(m_hwndHost, &m_magWindowRect);
	m_hwndMag = CreateWindow(WC_MAGNIFIER, TEXT("MagnifierWindow"),
		WS_CHILD | MS_CLIPAROUNDCURSOR | WS_VISIBLE,
		m_magWindowRect.left, m_magWindowRect.top, m_magWindowRect.right, m_magWindowRect.bottom, m_hwndHost, NULL, NULL, NULL);
	if (!m_hwndMag)
	{
		return FALSE;
	}

	// Set the magnification factor.
	MAGTRANSFORM matrix;
	memset(&matrix, 0, sizeof(matrix));
	matrix.v[0][0] = MAGFACTOR;
	matrix.v[1][1] = MAGFACTOR;
	matrix.v[2][2] = 1.0f;

	BOOL ret = FALSE;
	ret = MagSetWindowTransform(m_hwndMag, &matrix);

	if (ret)
	{
		MAGCOLOREFFECT magEffectInvert =
		{ { // MagEffectGrayscale
			{ 0.3f,  0.3f,  0.3f,  0.0f,  0.0f },
			{ 0.6f,  0.6f,  0.6f,  0.0f,  0.0f },
			{ 0.1f,  0.1f,  0.1f,  0.0f,  0.0f },
			{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
			{ 0.0f,  0.0f,  0.0f,  0.0f,  1.0f }
			} };

		ret = MagSetColorEffect(m_hwndMag, &magEffectInvert);

		// Create a timer to update the control.
		m_timerId = SetTimer(m_hwndHost, (UINT_PTR) this, timerInterval, UpdateMagWindow);
	}


	//ShowWindow(m_hwndHost, SW_SHOWNOACTIVATE);
	//UpdateWindow(m_hwndHost);

	::SetWindowLong(m_hwndHost, GWL_USERDATA, (LONG)this);

	m_isInit = true;

	return ret;
}

ATOM GrayFilterWin::RegisterHostWindowClass()
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GrayFilterWin::HostWndProc;
	wcex.hInstance = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
	wcex.lpszClassName = WindowClassName;

	return RegisterClassEx(&wcex);
}

void GrayFilterWin::UpdateMagWindow(HWND, UINT, UINT_PTR id, DWORD)
{
	GrayFilterWin* pThis = (GrayFilterWin*)id;
	RECT rect;
	
	HWND hwnd = pThis->m_hwndParent;

	if (::IsWindow(hwnd) && ::IsWindowVisible(hwnd))
	{
		::GetWindowRect(hwnd, &rect);

		// Set the source rectangle for the magnifier control.
		MagSetWindowSource(pThis->m_hwndMag, rect);

		//HWND preWindow = ::GetNextWindow(hwnd, GW_HWNDPREV);
		SetWindowPos(pThis->m_hwndHost, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			SWP_NOACTIVATE);

		::ShowWindow(pThis->m_hwndHost, SW_SHOW);
		::ShowWindow(pThis->m_hwndMag, SW_SHOW);
	}
	else
	{
		pThis->hide();
	}

	// Force redraw.
	InvalidateRect(pThis->m_hwndMag, NULL, TRUE);
}

LRESULT GrayFilterWin::HostWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SIZE:
		{
			GrayFilterWin* pThis = (GrayFilterWin*) ::GetWindowLong(hWnd, GWL_USERDATA);
			if (pThis)
			{
				RECT rect;
				::GetClientRect(pThis->m_hwndHost, &rect);
				SetWindowPos(pThis->m_hwndMag, NULL, rect.left, rect.top, rect.right, rect.bottom, 0);
			}
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

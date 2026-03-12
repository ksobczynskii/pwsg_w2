#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <algorithm>

class RectApp
{
private:
    static constexpr wchar_t CLASS_NAME[] = L"NotWMPaintClass";

    HINSTANCE m_instance{};
    HWND m_main{};

    HBRUSH m_backgroundBrush{};
    HBRUSH m_rectBrush{};

    std::vector<HWND> m_rects;   // zapisane prostokąty
    HWND m_activeRect{};         // aktualnie przeciągany prostokąt
    bool m_dragging = false;
    POINT m_start{};             // punkt początkowy przeciągania

public:
    explicit RectApp(HINSTANCE instance)
        : m_instance(instance)
    {
        m_backgroundBrush = CreateSolidBrush(RGB(30, 50, 90));
        m_rectBrush = CreateSolidBrush(RGB(170, 70, 80));

        register_class();
        create_main_window();
    }

    ~RectApp()
    {
        if (m_rectBrush)
            DeleteObject(m_rectBrush);

        if (m_backgroundBrush)
            DeleteObject(m_backgroundBrush);
    }

    int run(int show_command)
    {
        ShowWindow(m_main, show_command);
        UpdateWindow(m_main);

        MSG msg{};
        BOOL result;
        while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
        {
            if (result == -1)
                return 1;

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

private:
    bool register_class()
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = window_proc_static;
        wc.hInstance = m_instance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = m_backgroundBrush;
        wc.lpszClassName = CLASS_NAME;

        return RegisterClassExW(&wc) != 0;
    }

    bool create_main_window()
    {
        // Stały rozmiar, ale można przesuwać i minimalizować.
        DWORD style =
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU |
            WS_MINIMIZEBOX |
            WS_CLIPCHILDREN;

        // Chcemy obszar klienta dokładnie 800x600.
        RECT rc{ 0, 0, 800, 600 };
        AdjustWindowRectEx(&rc, style, FALSE, 0);

        m_main = CreateWindowExW(
            0,
            CLASS_NAME,
            L"Not WM_PAINT",
            style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rc.right - rc.left,
            rc.bottom - rc.top,
            nullptr,
            nullptr,
            m_instance,
            this
        );

        return m_main != nullptr;
    }

    void start_rect(HWND parent, int x, int y)
    {
        m_start.x = x;
        m_start.y = y;
        m_dragging = true;

        // Tworzymy aktywny prostokąt jako dziecko klasy STATIC.
        m_activeRect = CreateWindowExW(
            0,
            L"STATIC",
            nullptr,
            WS_CHILD | WS_VISIBLE,
            x, y, 0, 0,
            parent,
            nullptr,
            m_instance,
            nullptr
        );

        // Dzięki temu dalej dostajemy ruch myszy i puszczenie przycisku
        // nawet jeśli kursor wyjdzie poza główne okno.
        SetCapture(parent);
    }

    void update_active_rect(int x, int y)
    {
        if (!m_activeRect)
            return;

        int left = std::min(m_start.x, x);
        int top = std::min(m_start.y, y);
        int width = std::abs(x - m_start.x);
        int height = std::abs(y - m_start.y);

        SetWindowPos(
            m_activeRect,
            nullptr,
            left,
            top,
            width,
            height,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    void finish_rect(int x, int y)
    {
        if (!m_dragging)
            return;

        update_active_rect(x, y);

        ReleaseCapture();
        m_dragging = false;

        if (!m_activeRect)
            return;

        RECT rc{};
        GetWindowRect(m_activeRect, &rc);

        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // Jeśli kliknięto bez sensownego przeciągnięcia, usuń zerowy prostokąt.
        if (width == 0 || height == 0)
        {
            DestroyWindow(m_activeRect);
        }
        else
        {
            m_rects.push_back(m_activeRect);
        }

        m_activeRect = nullptr;
    }

    void remove_last_rect()
    {
        // Najpierw usuń aktywny, jeśli istnieje.
        if (m_activeRect)
        {
            DestroyWindow(m_activeRect);
            m_activeRect = nullptr;
            m_dragging = false;
            ReleaseCapture();

            InvalidateRect(m_main, nullptr, TRUE);
            return;
        }

        // W przeciwnym razie usuń ostatni zapisany.
        if (!m_rects.empty())
        {
            DestroyWindow(m_rects.back());
            m_rects.pop_back();

            InvalidateRect(m_main, nullptr, TRUE);
        }
    }

    static LRESULT CALLBACK window_proc_static(
        HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        RectApp* self = nullptr;

        if (msg == WM_NCCREATE)
        {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
            self = reinterpret_cast<RectApp*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        else
        {
            self = reinterpret_cast<RectApp*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (self)
            return self->window_proc(hwnd, msg, wparam, lparam);

        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);
            start_rect(hwnd, x, y);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (m_dragging)
            {
                int x = GET_X_LPARAM(lparam);
                int y = GET_Y_LPARAM(lparam);
                update_active_rect(x, y);
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);
            finish_rect(x, y);
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wparam == VK_BACK)
            {
                remove_last_rect();
                return 0;
            }
            break;
        }

        case WM_CTLCOLORSTATIC:
        {
            // Kolor wszystkich prostokątów-STATICów.
            HDC dc = reinterpret_cast<HDC>(wparam);
            SetBkColor(dc, RGB(170, 70, 80));
            return reinterpret_cast<LRESULT>(m_rectBrush);
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
};

constexpr wchar_t RectApp::CLASS_NAME[];

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command)
{
    RectApp app(instance);
    return app.run(show_command);
}

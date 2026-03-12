#include "app.h"
#include "resource.h"

#include <cwctype>
#include <string>

// Tekstowa nazwa klasy okna.
const wchar_t* App::s_class_name = L"PWSG_WinAPI_Demo_Class";

// Konstruktor tylko zeruje uchwyty i zapamiętuje instancję programu.
App::App(HINSTANCE instance)
    : m_instance(instance),
      m_main_window(nullptr),
      m_label(nullptr),
      m_edit(nullptr),
      m_show_button(nullptr),
      m_output(nullptr),
      m_uppercase_checkbox(nullptr),
      m_listbox(nullptr),
      m_dialog_button(nullptr)
{
}

// Rejestracja klasy okna.
bool App::register_window_class()
{
    // Struktura opisująca klasę okna.
    WNDCLASSEXW wc{};

    // Rozmiar struktury musi być ustawiony.
    wc.cbSize = sizeof(wc);

    // Adres statycznej procedury okna.
    wc.lpfnWndProc = App::window_proc_static;

    // HINSTANCE bieżącego programu.
    wc.hInstance = m_instance;

    // Kursor myszy widoczny nad oknem.
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    // Domyślne tło klienta okna.
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    // Nazwa klasy, po której system rozpoznaje ten typ okna.
    wc.lpszClassName = s_class_name;

    // Tu podpinamy menu z zasobów RC do klasy okna.
    // Dzięki temu okno tej klasy automatycznie dostanie pasek menu.
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);

    // Rejestracja klasy w systemie.
    return RegisterClassExW(&wc) != 0;
}

// Tworzenie głównego okna.
HWND App::create_main_window()
{
    // Chcemy mieć określony rozmiar obszaru klienta.
    RECT rc{ 0, 0, 420, 260 };

    // Trzeci parametr = TRUE, bo okno ma mieć menu.
    // System doliczy pasek tytułu, ramki i wysokość menu.
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, TRUE, 0);

    // Tworzymy okno klasy zarejestrowanej wcześniej.
    return CreateWindowExW(
        0,
        s_class_name,
        L"PWŚG - WinAPI demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        m_instance,
        this // przekazujemy wskaźnik do obiektu App
    );
}

// Tworzenie kontrolek dziecka.
void App::create_child_controls(HWND hwnd)
{
    // Napis opisujący pole tekstowe.
    m_label = CreateWindowExW(
        0,
        L"STATIC",
        L"Wpisz tekst:",
        WS_CHILD | WS_VISIBLE,
        15, 15, 80, 20,
        hwnd,
        reinterpret_cast<HMENU>(IDC_MAIN_LABEL),
        m_instance,
        nullptr
    );

    // Pole edycyjne. WS_TABSTOP umożliwia przechodzenie tabulatorem.
    m_edit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        100, 12, 180, 24,
        hwnd,
        reinterpret_cast<HMENU>(IDC_MAIN_EDIT),
        m_instance,
        nullptr
    );

    // Przycisk wyświetlający tekst.
    m_show_button = CreateWindowExW(
        0,
        L"BUTTON",
        L"Pokaż tekst",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        290, 12, 100, 24,
        hwnd,
        reinterpret_cast<HMENU>(IDC_BTN_SHOW),
        m_instance,
        nullptr
    );

    // Checkbox sterujący dodatkową logiką.
    m_uppercase_checkbox = CreateWindowExW(
        0,
        L"BUTTON",
        L"WIELKIE LITERY",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        15, 48, 140, 22,
        hwnd,
        reinterpret_cast<HMENU>(IDC_CHK_UPPERCASE),
        m_instance,
        nullptr
    );

    // Drugi przycisk - pokażemy nim, że kontrolka też wysyła WM_COMMAND.
    m_dialog_button = CreateWindowExW(
        0,
        L"BUTTON",
        L"Otwórz dialog",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        170, 46, 110, 24,
        hwnd,
        reinterpret_cast<HMENU>(IDC_BTN_DIALOG),
        m_instance,
        nullptr
    );

    // Static, który pokaże wynik działania programu.
    m_output = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"STATIC",
        L"Tu pojawi się tekst...",
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
        15, 82, 375, 24,
        hwnd,
        reinterpret_cast<HMENU>(IDC_MAIN_OUTPUT),
        m_instance,
        nullptr
    );

    // Listbox - historia wpisów.
    m_listbox = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"LISTBOX",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY,
        15, 118, 375, 90,
        hwnd,
        reinterpret_cast<HMENU>(IDC_MAIN_LISTBOX),
        m_instance,
        nullptr
    );
}

// Kopiowanie tekstu z edit do statica.
void App::copy_edit_to_output()
{
    // Bufor na tekst pobrany z kontrolki EDIT.
    wchar_t buffer[256]{};

    // Pobieramy tekst wpisany przez użytkownika.
    GetWindowTextW(m_edit, buffer, 256);

    // Zamieniamy na std::wstring, żeby wygodniej go modyfikować.
    std::wstring text = buffer;

    // Jeżeli checkbox jest zaznaczony, zamieniamy litery na wielkie.
    if (SendMessageW(m_uppercase_checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED)
    {
        for (wchar_t& ch : text)
        {
            ch = static_cast<wchar_t>(std::towupper(ch));
        }
    }

    // Jeśli pole jest puste, ustawiamy tekst pomocniczy.
    if (text.empty())
    {
        text = L"[pusty tekst]";
    }

    // Ustawiamy napis w kontrolce STATIC.
    SetWindowTextW(m_output, text.c_str());

    // Dodajemy wpis do listboxa jako historię.
    SendMessageW(m_listbox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
}

// Czyszczenie głównych pól.
void App::clear_main_text()
{
    // Czyścimy edit box.
    SetWindowTextW(m_edit, L"");

    // Czyścimy output.
    SetWindowTextW(m_output, L"Tu pojawi się tekst...");

    // Ustawiamy fokus z powrotem do pola edit.
    SetFocus(m_edit);
}

// Otwieranie dialogu modalnego.
void App::open_settings_dialog(HWND owner)
{
    // DialogBoxParamW tworzy dialog modalny i blokuje właściciela,
    // dopóki nie wywołamy EndDialog.
    DialogBoxParamW(
        m_instance,
        MAKEINTRESOURCEW(IDD_SETTINGS),
        owner,
        App::dialog_proc,
        reinterpret_cast<LPARAM>(this) // przekazujemy wskaźnik do App do dialogu
    );
}

// Obsługa poleceń z menu i kontrolek.
void App::on_command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // LOWORD(wParam) to identyfikator polecenia albo kontrolki.
    const WORD id = LOWORD(wParam);

    // HIWORD(wParam) to kod powiadomienia od kontrolki, np. BN_CLICKED.
    const WORD notify_code = HIWORD(wParam);

    switch (id)
    {
    case IDM_CLEAR_TEXT:
        // Menu: wyczyść tekst.
        clear_main_text();
        break;

    case IDM_OPEN_DIALOG:
        // Menu: otwórz dialog.
        open_settings_dialog(hwnd);
        break;

    case IDM_EXIT:
        // Menu: zamknij program.
        DestroyWindow(hwnd);
        break;

    case IDM_ABOUT:
        // Menu: najprostszy gotowy dialog systemowy.
        MessageBoxW(
            hwnd,
            L"Przykład pokazuje: menu, kontrolki, dialog modalny i WM_COMMAND.",
            L"O programie",
            MB_OK | MB_ICONINFORMATION
        );
        break;

    case IDC_BTN_SHOW:
        // Kliknięcie przycisku "Pokaż tekst".
        if (notify_code == BN_CLICKED)
        {
            copy_edit_to_output();
        }
        break;

    case IDC_BTN_DIALOG:
        // Kliknięcie przycisku otwierającego dialog.
        if (notify_code == BN_CLICKED)
        {
            open_settings_dialog(hwnd);
        }
        break;

    case IDC_MAIN_LISTBOX:
        // Dwuklik na elemencie listboxa kopiuje go do pola edit.
        if (notify_code == LBN_DBLCLK)
        {
            // Pobieramy indeks aktualnie zaznaczonego elementu.
            const LRESULT selected_index = SendMessageW(m_listbox, LB_GETCURSEL, 0, 0);

            if (selected_index != LB_ERR)
            {
                wchar_t item_text[256]{};

                // Pobieramy tekst zaznaczonego elementu.
                SendMessageW(m_listbox, LB_GETTEXT, static_cast<WPARAM>(selected_index), reinterpret_cast<LPARAM>(item_text));

                // Wstawiamy go z powrotem do edit boxa.
                SetWindowTextW(m_edit, item_text);
            }
        }
        break;
    }
}

// Statyczna procedura okna.
LRESULT CALLBACK App::window_proc_static(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Wskaźnik na obiekt App skojarzony z danym HWND.
    App* app = nullptr;

    if (msg == WM_NCCREATE)
    {
        // Podczas tworzenia okna lpCreateParams zawiera wskaźnik przekazany do CreateWindowExW.
        CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        app = static_cast<App*>(cs->lpCreateParams);

        // Zapisujemy wskaźnik wewnątrz okna, aby odzyskać go przy kolejnych komunikatach.
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    else
    {
        // Dla zwykłych komunikatów odczytujemy wskaźnik zapisany wcześniej.
        app = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    // Jeśli mamy poprawny wskaźnik, delegujemy obsługę do metody niestatycznej.
    if (app != nullptr)
    {
        return app->window_proc(hwnd, msg, wParam, lParam);
    }

    // Gdy nie mamy jeszcze wskaźnika, korzystamy z domyślnej procedury.
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Właściwa procedura głównego okna.
LRESULT App::window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        // WM_CREATE przychodzi po utworzeniu okna.
        // To dobre miejsce na utworzenie kontrolek dziecka.
        m_main_window = hwnd;
        create_child_controls(hwnd);
        return 0;

    case WM_COMMAND:
        // Tu trafiają kliknięcia z menu i wiele akcji kontrolek.
        on_command(hwnd, wParam, lParam);
        return 0;

    case WM_CLOSE:
        // System pyta, czy zamknąć okno - my je niszczymy.
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        // Kończymy pętlę komunikatów całej aplikacji.
        PostQuitMessage(0);
        return 0;
    }

    // Nieobsłużone komunikaty przekazujemy do systemu.
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Procedura dialogu modalnego.
INT_PTR CALLBACK App::dialog_proc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Wskaźnik do obiektu App powiązanego z dialogiem.
    App* app = nullptr;

    if (msg == WM_INITDIALOG)
    {
        // Przy inicjalizacji dialogu przekazany LPARAM zawiera wskaźnik do App.
        app = reinterpret_cast<App*>(lParam);

        // Zapamiętujemy go w danych użytkownika dialogu.
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));

        // Jeśli mamy główne okno, wpisujemy jego obecny tytuł do pola edit w dialogu.
        if (app && app->m_main_window)
        {
            wchar_t title[256]{};
            GetWindowTextW(app->m_main_window, title, 256);
            SetDlgItemTextW(hDlg, IDC_DLG_EDIT_TITLE, title);
        }

        // TRUE oznacza: ustaw fokus zgodnie z domyślnym zachowaniem dialogu.
        return TRUE;
    }

    // Dla dalszych komunikatów pobieramy wskaźnik zapisany w WM_INITDIALOG.
    app = reinterpret_cast<App*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));

    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            // Pobieramy nowy tytuł z pola edit dialogu.
            wchar_t new_title[256]{};
            GetDlgItemTextW(hDlg, IDC_DLG_EDIT_TITLE, new_title, 256);

            // Jeśli mamy dostęp do głównego okna, ustawiamy jego nowy tytuł.
            if (app && app->m_main_window)
            {
                SetWindowTextW(app->m_main_window, new_title);

                // Sprawdzamy stan checkboxa w dialogu.
                const LRESULT checked = SendDlgItemMessageW(
                    hDlg,
                    IDC_DLG_CHK_CLEAR_HISTORY,
                    BM_GETCHECK,
                    0,
                    0
                );

                // Jeśli zaznaczono opcję czyszczenia historii, czyścimy listbox.
                if (checked == BST_CHECKED && app->m_listbox)
                {
                    SendMessageW(app->m_listbox, LB_RESETCONTENT, 0, 0);
                }
            }

            // EndDialog kończy dialog modalny i odblokowuje okno właściciela.
            EndDialog(hDlg, IDOK);
            return TRUE;
        }

        case IDCANCEL:
            // Zamykamy dialog bez zapisywania zmian.
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }

    // FALSE oznacza: dialog nie obsłużył komunikatu.
    return FALSE;
}

// Start aplikacji i pętla komunikatów.
int App::run(int show_command)
{
    // Najpierw rejestrujemy klasę okna.
    if (!register_window_class())
    {
        MessageBoxW(nullptr, L"Nie udało się zarejestrować klasy okna.", L"Błąd", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Następnie tworzymy główne okno.
    m_main_window = create_main_window();
    if (!m_main_window)
    {
        MessageBoxW(nullptr, L"Nie udało się utworzyć głównego okna.", L"Błąd", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Pokazujemy okno.
    ShowWindow(m_main_window, show_command);

    // Wymuszamy jego pierwsze odmalowanie.
    UpdateWindow(m_main_window);

    // Klasyczna pętla komunikatów WinAPI.
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        // TranslateMessage przygotowuje np. WM_CHAR na bazie klawiszy.
        TranslateMessage(&msg);

        // DispatchMessage przekazuje komunikat do odpowiedniej procedury okna/dialogu.
        DispatchMessageW(&msg);
    }

    // Kod wyjścia aplikacji.
    return static_cast<int>(msg.wParam);
}

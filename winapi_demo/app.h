#pragma once

// Ograniczamy zawartość windows.h do najczęściej używanych rzeczy.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <string>

// Klasa App zbiera w jednym miejscu:
// - uchwyt instancji programu,
// - uchwyt głównego okna,
// - uchwyty kontrolek,
// - logikę tworzenia okna i obsługi komunikatów.
class App
{
private:
    // Nazwa klasy okna - identyfikator używany przy RegisterClassExW i CreateWindowExW.
    static const wchar_t* s_class_name;

    // Uchwyt instancji programu przekazany przez system do wWinMain.
    HINSTANCE m_instance;

    // Uchwyt głównego okna aplikacji.
    HWND m_main_window;

    // Uchwyty kontrolek utworzonych wewnątrz głównego okna.
    HWND m_label;
    HWND m_edit;
    HWND m_show_button;
    HWND m_output;
    HWND m_uppercase_checkbox;
    HWND m_listbox;
    HWND m_dialog_button;

    // Rejestruje klasę okna w systemie.
    bool register_window_class();

    // Tworzy główne okno aplikacji.
    HWND create_main_window();

    // Tworzy wszystkie kontrolki wewnątrz głównego okna.
    void create_child_controls(HWND hwnd);

    // Reaguje na polecenia z menu i kontrolek.
    void on_command(HWND hwnd, WPARAM wParam, LPARAM lParam);

    // Kopiuje tekst z pola edit do statica i opcjonalnie zmienia go na wielkie litery.
    void copy_edit_to_output();

    // Czyści edit, output i zaznaczenie listboxa.
    void clear_main_text();

    // Otwiera modalny dialog zdefiniowany w Resource.rc.
    void open_settings_dialog(HWND owner);

    // Statyczna procedura okna - potrzebna, bo WinAPI wymaga zwykłej funkcji.
    static LRESULT CALLBACK window_proc_static(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Właściwa procedura okna - tu mamy dostęp do pól klasy App.
    LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Procedura dialogu modalnego.
    static INT_PTR CALLBACK dialog_proc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    // Konstruktor zapisuje HINSTANCE.
    explicit App(HINSTANCE instance);

    // Uruchamia aplikację: tworzy okno i startuje pętlę komunikatów.
    int run(int show_command);
};

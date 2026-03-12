#pragma once

// ===== GŁÓWNE ZASOBY =====
// ID głównego menu aplikacji.
#define IDR_MAINMENU 101

// ID głównego dialogu modalnego.
#define IDD_SETTINGS 102

// ===== POLECENIA MENU =====
// Polecenie: wyczyść pole tekstowe.
#define IDM_CLEAR_TEXT 40001

// Polecenie: otwórz dialog ustawień.
#define IDM_OPEN_DIALOG 40002

// Polecenie: zakończ aplikację.
#define IDM_EXIT 40003

// Polecenie: prosty dialog "O programie".
#define IDM_ABOUT 40004

// ===== KONTROLKI W OKNIE GŁÓWNYM =====
// Etykieta opisująca pole edycji.
#define IDC_MAIN_LABEL 50001

// Jednoliniowy edit box, do którego użytkownik wpisuje tekst.
#define IDC_MAIN_EDIT 50002

// Przycisk kopiujący tekst z edit boxa do statica.
#define IDC_BTN_SHOW 50003

// Static pokazujący aktualny tekst.
#define IDC_MAIN_OUTPUT 50004

// Checkbox decydujący o tym, czy tekst ma być zamieniany na WIELKIE LITERY.
#define IDC_CHK_UPPERCASE 50005

// Listbox pokazujący historię zatwierdzonych tekstów.
#define IDC_MAIN_LISTBOX 50006

// Przycisk otwierający dialog modalny.
#define IDC_BTN_DIALOG 50007

// ===== KONTROLKI W DIALOGU =====
// Pole edycyjne w dialogu, w którym wpisujemy nowy tytuł okna.
#define IDC_DLG_EDIT_TITLE 60001

// Checkbox w dialogu: czy wyczyścić historię po zatwierdzeniu.
#define IDC_DLG_CHK_CLEAR_HISTORY 60002

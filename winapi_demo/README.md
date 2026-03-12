# WinAPI demo pod PWŚG: kontrolki, menu i dialogi

Ten mini-projekt jest zrobiony tak, żeby spełnić dokładnie to, o co zwykle chodzi na zajęciach z podstaw WinAPI:
- **główne okno**,
- **pasek menu z pliku `.rc`**,
- **kontrolki dziecka** tworzone programowo,
- **dialog modalny** zdefiniowany w `.rc`,
- **obsługa `WM_COMMAND`** zarówno z menu, jak i z kontrolek,
- bardzo gęste komentarze, żeby było jasne **co, gdzie i po co**.

---

## Co zawiera przykład

### W oknie głównym
- `STATIC` z napisem „Wpisz tekst:”
- `EDIT` do wpisania tekstu
- `BUTTON` „Pokaż tekst”
- `BUTTON`-checkbox „WIELKIE LITERY”
- `BUTTON` „Otwórz dialog”
- `STATIC` pokazujący wynik
- `LISTBOX` z historią wpisów

### W menu
- `Plik -> Wyczyść tekst`
- `Plik -> Dialog ustawień...`
- `Plik -> Zakończ`
- `Pomoc -> O programie`

### W dialogu modalnym
- pole edycyjne do zmiany tytułu głównego okna,
- checkbox „Wyczyść historię po OK”,
- przyciski `OK` i `Anuluj`.

---

## Struktura plików

- `main.cpp` – punkt wejścia `wWinMain`
- `app.h` – deklaracja klasy `App`
- `app.cpp` – cała logika okna, kontrolek i dialogu
- `resource.h` – identyfikatory zasobów i kontrolek
- `Resource.rc` – definicja menu i dialogu

---

## Jak ten projekt pokazuje najważniejsze mechanizmy WinAPI

## 1. Rejestracja klasy okna
To robi `register_window_class()`.

Najważniejsze pola:
- `lpfnWndProc` – adres procedury okna,
- `hInstance` – instancja programu,
- `lpszClassName` – nazwa klasy,
- `lpszMenuName` – menu z zasobów.

W tym przykładzie menu jest podpinane tak:

```cpp
wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
```

To jest dokładnie ten prosty wariant, który zwykle pojawia się na laboratoriach: menu zapisane w zasobach i przypięte do klasy okna. W Twoim źródle opisano właśnie ten sposób oraz to, że po dodaniu menu trzeba uwzględnić je w `AdjustWindowRectEx`, bo pasek menu wpływa na rozmiar całego okna. fileciteturn3file1

---

## 2. Tworzenie okna
To robi `create_main_window()`.

Najpierw liczony jest rozmiar całego okna na podstawie oczekiwanego rozmiaru **obszaru klienta**:

```cpp
RECT rc{ 0, 0, 420, 260 };
AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, TRUE, 0);
```

Trzeci parametr ma wartość `TRUE`, bo nasze okno **ma menu**. To też jest dokładnie motyw z Twojego materiału: po dodaniu menu trzeba zmienić trzeci parametr `AdjustWindowRectEx`, żeby area klienta nie „zmalała” przez pasek menu. fileciteturn3file1

---

## 3. Jak działa `window_proc_static`
WinAPI wymaga zwykłej funkcji jako `WndProc`, ale my chcemy pisać logikę w metodzie klasy.

Dlatego robimy wzorzec:
- statyczna procedura okna,
- zapis wskaźnika do obiektu w `GWLP_USERDATA`,
- delegowanie do metody niestatycznej.

Schemat jest taki:
- przy `WM_NCCREATE` bierzemy `this` z `CREATESTRUCTW`,
- zapisujemy przez `SetWindowLongPtrW`,
- potem odzyskujemy wskaźnik przez `GetWindowLongPtrW` i przekazujemy komunikat dalej.

To jest klasyczny układ „statyczny forwarder -> niestatyczna metoda”, zgodny z tym, jak w źródle zbudowano klasę aplikacji obsługującą komunikaty. fileciteturn3file0

---

## 4. Gdzie tworzą się kontrolki
Kontrolki tworzą się w `WM_CREATE`, w metodzie `create_child_controls()`.

To znaczy:
- najpierw system tworzy okno główne,
- potem przychodzi `WM_CREATE`,
- wtedy tworzymy wewnętrzne elementy GUI.

Każda kontrolka jest po prostu osobnym **oknem dziecka** (`WS_CHILD`) klasy systemowej, np.:
- `STATIC`
- `EDIT`
- `BUTTON`
- `LISTBOX`

To dobrze pasuje do Twojego materiału, gdzie statyczne kontrolki są tworzone jako child windows przez `CreateWindowExW`, a rodzic reaguje później na ich komunikaty i powiadomienia. fileciteturn3file1

---

## 5. Jak działa `WM_COMMAND`
To jest najważniejszy komunikat w tym przykładzie.

Do `WM_COMMAND` wpadają m.in.:
- kliknięcia menu,
- kliknięcia przycisków,
- część powiadomień z kontrolek.

W kodzie:

```cpp
case WM_COMMAND:
    on_command(hwnd, wParam, lParam);
    return 0;
```

W `on_command()` bierzemy:
- `LOWORD(wParam)` – **ID polecenia / kontrolki**,
- `HIWORD(wParam)` – **kod powiadomienia**, np. `BN_CLICKED`.

To dokładnie pokrywa się z opisem w źródle: przy wyborze pozycji menu okno dostaje `WM_COMMAND`, a dolne 16 bitów `wParam` zawiera identyfikator pozycji menu. fileciteturn3file1

### Przykłady z tego projektu
- `IDM_CLEAR_TEXT` – przyszło z menu,
- `IDC_BTN_SHOW + BN_CLICKED` – przyszło z przycisku,
- `IDC_MAIN_LISTBOX + LBN_DBLCLK` – przyszło z listboxa.

Dzięki temu widzisz, że **menu i kontrolki spotykają się w jednym miejscu**.

---

## 6. Po co jest `resource.h`
`resource.h` zawiera tylko **identyfikatory liczbowe**.

Na przykład:
- `IDR_MAINMENU` – całe menu,
- `IDD_SETTINGS` – cały dialog,
- `IDM_EXIT` – konkretna pozycja menu,
- `IDC_MAIN_EDIT` – konkretna kontrolka.

Kod C++ i plik `.rc` komunikują się właśnie przez te ID.

---

## 7. Po co jest `Resource.rc`
`Resource.rc` to **plik zasobów**. On nie zawiera logiki programu, tylko opisuje gotowe zasoby Windowsowe:
- menu,
- dialog,
- potencjalnie także ikony, string table, akceleratory itd.

W Twoim źródle właśnie tak jest to przedstawione: menu i inne stałe elementy interfejsu najwygodniej trzymać w zasobach, bo wtedy łatwo je edytować i ładować przez `MAKEINTRESOURCEW(...)`. fileciteturn3file1turn3file2

---

## 8. Menu: cały przepływ
Pełny przepływ w tym projekcie wygląda tak:

1. Menu jest zdefiniowane w `Resource.rc`.
2. Klasa okna dostaje to menu przez `lpszMenuName`.
3. Użytkownik klika np. `Plik -> Zakończ`.
4. Okno dostaje `WM_COMMAND`.
5. `LOWORD(wParam)` ma wartość `IDM_EXIT`.
6. W `on_command()` wywołujemy `DestroyWindow(hwnd)`.

To jest dokładnie to, co trzeba umieć wyjaśnić na zajęciach.

---

## 9. Kontrolki: jak z nich korzystać

### `EDIT`
Z pola edycyjnego pobieramy tekst przez:

```cpp
GetWindowTextW(m_edit, buffer, 256);
```

### `STATIC`
Ustawiamy tekst wyniku przez:

```cpp
SetWindowTextW(m_output, text.c_str());
```

### `BUTTON` jako zwykły przycisk
Kliknięcie daje `BN_CLICKED`.

### `BUTTON` jako checkbox
Stan sprawdzamy przez:

```cpp
SendMessageW(m_uppercase_checkbox, BM_GETCHECK, 0, 0)
```

### `LISTBOX`
- dodanie wpisu: `LB_ADDSTRING`
- pobranie zaznaczenia: `LB_GETCURSEL`
- pobranie tekstu elementu: `LB_GETTEXT`
- wyczyszczenie listy: `LB_RESETCONTENT`

To daje Ci przekrój kilku typowych kontrolek bez robienia z przykładu dużego kombajnu.

---

## 10. Dialog modalny: co tu się dzieje
Dialog jest zdefiniowany w `Resource.rc` jako `IDD_SETTINGS`.

Otwieramy go tak:

```cpp
DialogBoxParamW(
    m_instance,
    MAKEINTRESOURCEW(IDD_SETTINGS),
    owner,
    App::dialog_proc,
    reinterpret_cast<LPARAM>(this)
);
```

To oznacza:
- użyj zasobu dialogu `IDD_SETTINGS`,
- ustaw właściciela `owner`,
- obsługuj dialog przez `dialog_proc`,
- przekaż dodatkowe dane (`this`) przez `lParam`.

### Dlaczego to jest dialog modalny?
Bo `DialogBoxParamW` blokuje dalszą normalną pracę z oknem właściciela, dopóki dialog nie zostanie zakończony przez `EndDialog(...)`.

### `WM_INITDIALOG`
Przy starcie dialogu dostajemy `WM_INITDIALOG`. To jest miejsce na:
- ustawienie stanu początkowego,
- wpisanie tekstu do kontrolek,
- zapisanie wskaźnika do obiektu.

W tym projekcie w `WM_INITDIALOG`:
- zapisujemy `App*` w `GWLP_USERDATA`,
- pobieramy aktualny tytuł okna głównego,
- wpisujemy go do pola `IDC_DLG_EDIT_TITLE`.

### `IDOK` i `IDCANCEL`
To standardowe przyciski dialogu:
- `IDOK` – zatwierdź,
- `IDCANCEL` – anuluj.

Przy `IDOK`:
- pobieramy tekst przez `GetDlgItemTextW`,
- ustawiamy nowy tytuł głównego okna,
- sprawdzamy checkbox przez `SendDlgItemMessageW(..., BM_GETCHECK, ...)`,
- ewentualnie czyścimy historię,
- kończymy dialog przez `EndDialog(hDlg, IDOK)`.

Przy `IDCANCEL` po prostu zamykamy dialog bez zmian.

---

## 11. Najważniejsze miejsca do zapamiętania

### Menu
- definicja: `Resource.rc`
- identyfikatory: `resource.h`
- podpięcie: `lpszMenuName = MAKEINTRESOURCEW(...)`
- obsługa: `WM_COMMAND`

### Kontrolki
- tworzenie: `CreateWindowExW`
- identyfikacja: ID kontrolki
- obsługa akcji: też `WM_COMMAND`

### Dialog
- definicja: `Resource.rc`
- uruchomienie: `DialogBoxParamW`
- inicjalizacja: `WM_INITDIALOG`
- zamknięcie: `EndDialog`

### Główna pętla programu
- `GetMessageW`
- `TranslateMessage`
- `DispatchMessageW`

To zresztą bardzo dobrze pokrywa się z układem Twojego materiału: najpierw klasa aplikacji i pętla komunikatów, potem child controls, potem menu, a potem rozszerzenia o dodatkowe interakcje. fileciteturn3file0turn3file1

---

## 12. Jak uruchomić w Visual Studio

1. Utwórz projekt **Windows Desktop Application (.exe)**.
2. Dodaj pliki:
   - `main.cpp`
   - `app.h`
   - `app.cpp`
   - `resource.h`
   - `Resource.rc`
3. Upewnij się, że projekt jest Unicode.
4. Skompiluj i uruchom.

Jeśli Visual Studio nie dodało automatycznie pliku `.rc` do „Resource Files”, dodaj go ręcznie jako istniejący plik.

---

## 13. Co ten przykład pokazuje „egzaminowo”

Po przerobieniu tego projektu powinieneś umieć powiedzieć:
- czym jest `Resource.rc`,
- po co jest `resource.h`,
- skąd bierze się `WM_COMMAND`,
- czym różni się `LOWORD(wParam)` od `HIWORD(wParam)`,
- jak tworzy się kontrolki dziecka,
- jak działa `DialogBoxParamW`,
- czym różni się `WndProc` od `DlgProc`,
- po co używa się `WM_INITDIALOG`,
- po co używa się `EndDialog`,
- jak działa podstawowa pętla komunikatów.

---

## 14. Najkrótszy opis całego flow programu

1. `wWinMain` tworzy `App`.
2. `App::run()` rejestruje klasę i tworzy główne okno.
3. System wysyła `WM_NCCREATE`, potem `WM_CREATE`.
4. W `WM_CREATE` powstają kontrolki.
5. Program wchodzi do pętli komunikatów.
6. Kliknięcia menu i kontrolek wpadają do `WM_COMMAND`.
7. Dialog jest otwierany przez `DialogBoxParamW` i obsługiwany przez `dialog_proc`.
8. Zamknięcie okna kończy pętlę przez `PostQuitMessage`.

---

## 15. Dlaczego ten przykład jest celowo prosty

Nie dorzucałem tutaj:
- rysowania GDI,
- timerów,
- akceleratorów klawiaturowych,
- wielu klas okien,
- własnych kontrolek,
- złożonego zarządzania pamięcią.

Cel był inny: zrobić **mały, przejrzysty projekt**, który naprawdę da się przeczytać od początku do końca i zrozumieć.

A jednocześnie cały czas opiera się na dokładnie tych fundamentach, które Twój materiał też buduje: klasa aplikacji, rejestracja klasy okna, `WndProc`, child controls, menu z zasobów i `WM_COMMAND`. fileciteturn3file0turn3file1

#include "app_2048.h"
#include <stdexcept>
std::wstring const app_2048::s_class_name{ L"2048 Window" };

bool app_2048::register_class() { // dokumentacja winapi kaze nam zarejestrowac klase okienka odpowiednimi parametrami przed utworzeniem okienka
	WNDCLASSEXW desc{}; // struktura opisujaca paramsy okna
	if (GetClassInfoExW(m_instance, s_class_name.c_str(), // funkcja zwraca 0 jak nie jest zarejestrowana, zapobieganie ponownej rejestracji
		&desc) != 0) return true;
	desc = { .cbSize = sizeof(WNDCLASSEXW), // rozmiar struktury, wymaganie winapi
	.lpfnWndProc = window_proc_static,// wskaznik do funkcji obslugujacej zdarzenia
	.hInstance = m_instance, //uchwyt do aplikacji 
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),// typ kursora
	.lpszClassName = s_class_name.c_str() }; // tworzymy strukture opisujaca klase do rejestracji klasy
	return RegisterClassExW(&desc) != 0; // zwracamy sukces w przypadku zarejestrowania klasy
}

HWND app_2048::create_window() // tu tworzymy okienko  po wstepnym zarejestrowaniu
{
	return CreateWindowExW( // funkcja CreateWindowExW zwraca handler do window 
		0 /*empty extended styles*/, // nie dodajemy zadnych stylow
		s_class_name.c_str(), // nazwe klasy przekazujemy, teraz okno wie jaka metode wywolac po kliknieciu/ innej akcji
		L"2048", // co wyswietli sie na pasku okienka
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX, // flagi m.in CAPTION - ma pasek tytulu, WS_SYSMENU - menu systemowe (zamykanie etc.)
		// WS_MINIMIZEBOX - mozna minimalizowac, WS_SIZEBOX - mozna zmieniac rozmiar okna -- tu nie ma
		CW_USEDEFAULT, 0, /*default position*/
		CW_USEDEFAULT, 0, /*default size*/
		nullptr, // pointer do parent
		nullptr, // pointer do menu
		m_instance, // instancja aplikacji
		this); // przekazujemy instancje klasy aby moc potem wywolac funkcje niestatyczna
}
LRESULT app_2048::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app_2048* app = nullptr;
	if (message == WM_NCCREATE) // komnuikat do aplikacji po powstaniu okienka
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam); // jak okienko jest tworzone to lparam zawiera pointer na struct 
		app = static_cast<app_2048*>(p->lpCreateParams); // jednym z parametrow struktury jest pointer na instancje aplikacji
		SetWindowLongPtrW(window, GWLP_USERDATA, // przypisuje okienku instancje klasy wyluskana (przecastowana na pointer)
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_2048*>(
			GetWindowLongPtrW(window, GWLP_USERDATA)); // funkcja wyciaga instancje klasy
	}
	if (app != nullptr)
	{
		return app->window_proc(window, message,
			wparam, lparam); // przekazujemy wszystko i wywolujemy na metodzie niestatycznej - pelna dostepnosc do klasy
	}
	return DefWindowProcW(window, message, wparam, lparam); // default action handler
}

LRESULT app_2048::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam)
{
	switch (message) { // switch case na podstawie message ktory dostaniemy
	case WM_CLOSE: // przycisk x lub alt+f4
		DestroyWindow(window); // wysyla komunikat WM_DESTROY i zamyka okno ale nie program
		return 0;
	case WM_DESTROY:
		if (window == m_main) // czy zamkniete okno to nasze okno ? 
			PostQuitMessage(EXIT_SUCCESS); // do kolejki dodajemy message exit - powoduje przerwanie petli while z run()
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam); // defaultowa procedura windows
}

app_2048::app_2048(HINSTANCE instance) // konstruktor
	: m_instance{ instance }, m_main{}
{
	register_class(); // rejestrujemy klase
	m_main = create_window(); // tworzymy okienko
}

int app_2048::run(int show_command) // funkcja glowna
{
	ShowWindow(m_main, show_command);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) // pobieramy z kolejki komunikaty do msg i sprawdzamy rezultat. 
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg); // translacja kombinacji klawiszowych
		DispatchMessageW(&msg); // wysyla procedure do funkcji obslugujacej
	}
	return EXIT_SUCCESS;
}



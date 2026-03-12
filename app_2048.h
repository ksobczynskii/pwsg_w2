#pragma once
#include <windows.h>
#include <string>
// Poprawiona ścieżka do nagłówka `board.h` — plik znajduje się w katalogu projektu (dwa poziomy w górę)
#include "../../board.h"
class app_2048
{
private:
	bool register_class();
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	// WPARAM - word parameter, info o tym co zostalo klikniete
	// LPARAM - dlugi parametr, mowi o szczegolowych rzeczach jak polozenie mysze etc.
	// LRESULT - liczba calkowita, mowi o tym jak program przetworzyl komunikat
	LRESULT window_proc(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	HWND create_window(DWORD style, HWND parent = nullptr);
	HINSTANCE m_instance; // uchwyt do instancji aplikacji otrzymany z maina
	HWND m_main, m_popup; // uchwyt do glownego okna programu i m_popup czyli okno zduplikowane

	board m_board;

	HBRUSH m_field_brush;
public:
	app_2048(HINSTANCE instance);
	int run(int show_command);
};

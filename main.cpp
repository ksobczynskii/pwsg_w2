#include <Windows.h>
#include "app_2048.h"

int WINAPI wWinMain(HINSTANCE instance, // uchwyt do aktualnej instancji aplikacji
	HINSTANCE /*prevInstance*/, // uchwyt do poprzedniej instancji, nieistotny i ma domyslnie null
	LPWSTR /*command_line*/, // long pointer do argumentow wejsciowych
	int show_command) // flaga mowiaca o tym w jaki sposob wyswietlac okno po starcie zminimalizowane, maksymalne etc.
{
	app_2048 app{ instance };
	return app.run(show_command);


}
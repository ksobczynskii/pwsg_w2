// Minimalny punkt wejścia aplikacji okienkowej WinAPI.

#include "app.h"

// wWinMain jest wersją Unicode funkcji wejściowej dla aplikacji GUI.
int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE /*prev_instance*/,
    PWSTR /*command_line*/,
    int show_command)
{
    // Tworzymy obiekt aplikacji.
    App app(instance);

    // Uruchamiamy aplikację i zwracamy jej kod zakończenia.
    return app.run(show_command);
}

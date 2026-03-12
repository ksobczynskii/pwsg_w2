#pragma once
#include <array>
#include <Windows.h>
// Reprezentuje pojedyncze pole planszy.
// Przechowuje pozycję/pole prostokątne (piksele) używane do rysowania i wykrywania kliknięć.
struct field
{
	RECT position;
};

// Klasa opisująca siatkę planszy gry.
// Zawiera stałe określające rozmiar planszy i tablicę pól z ich pozycjami.
class board
{
public:
	// Liczba kolumn w planszy (statyczna, stała w czasie kompilacji).
	static constexpr LONG columns = 4;
	// Liczba wierszy w planszy.
	static constexpr LONG rows = 4;
	// Margines (odstęp) pomiędzy polami oraz od krawędzi planszy (w pikselach).
	static constexpr LONG margin = 10;
	// Całkowita liczba pól na planszy (rows * columns).
	static constexpr LONG field_count = rows * columns;
	// Rozmiar pojedynczego pola (szerokość i wysokość w pikselach).
	static constexpr LONG field_size = 60;
	// Obliczona szerokość całej planszy (uwzględnia marginesy pomiędzy polami).
	static constexpr LONG width =
		columns * (field_size + margin) + margin;
	// Obliczona wysokość całej planszy.
	static constexpr LONG height =
		rows * (field_size + margin) + margin;

	using field_array = std::array<field, field_count>;

	// Konstruktor inicjalizuje pola planszy (np. oblicza ich pozycje).
	board();

	// Zwraca stałą referencję do tablicy pól. Używane do rysowania i obsługi zdarzeń.
	field_array const& fields() const { return m_fields; }

private:
	// Przechowuje pozycje wszystkich pól na planszy.
	field_array m_fields;
};

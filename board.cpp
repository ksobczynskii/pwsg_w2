#include "board.h"

board::board() : m_fields{}
{
	for (LONG row = 0; row < rows; ++row)
	{
		for(LONG column = 0; column < columns; ++column)
		{
			auto& f = m_fields[row * columns + column]; // obliczamy indeks pola w tablicy na podstawie wiersza i kolumny

			f.position.top = row * (field_size + margin) + margin; // obliczamy pozycje gorna pola
			f.position.left = column * (field_size + margin) + margin; // obliczamy pozycje lewa pola
			f.position.bottom = f.position.top + field_size; // obliczamy pozycje dolna pola
			f.position.right = f.position.left + field_size; // obliczamy pozycje prawa pola
		}
	}
}
/** @file
 * Interfejs umożliwiający wybór trybu.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#ifndef MODE_H
#define MODE_H

/** @brief Tryb gry.
 * Typ wyliczeniowy reprezentujący tryb gry (lub wczytanie znaku @p eof).
 */
enum mode {batch_mode, interactive_mode, end_of_file};

/** @brief Wybiera tryb gry.
 * Wczytuje kolejne wiersze wejścia dopóki nie trafi na wiersz zawierający
 * poprawne polecenie utworzenia nowej gry lub znak @p EOF.
 * Jeśli wiersz zaczyna się od znaku @p # lub składa się tylko ze znaku
 * przejścia do nowego wiersza, ignoruje go.
 * Jeśli wiersz zawiera niepoprawne polecenie, wypisuje na standardowe wyjście
 * diagnostyczne komunikat @p "ERROR line", gdzie @p line jest numerem wiersza.
 * Jeśli wiersz zawiera poprawne polecenie, tworzy nową grę i zwraca wartość
 * odpowiadającą wybranemu trybowi. Jeśli wybrano tryb wsadowy, wypisuje na
 * standardowe wejście komunikat @p "OK line",
 * gdzie @p line jest numerem wiersza.
 * @param[in, out] line – wskaźnik na numer aktualnej linii wejścia,
 * @param[in, out] g    – wskaźnik na na strukturę, do której zapisany będzie
 *                        wynik wywołania funkcji @ref gamma_new.
 * @return Tryb gry, jaki został wybrany.
 */
enum mode choose_mode(unsigned long long* line, gamma_t** g);

#endif /* MODE_H */
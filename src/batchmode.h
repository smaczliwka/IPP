/** @file
 * Interfejs umożliwiający przeprowadzanie rozgrywki w trybie interaktywnym.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#ifndef BATCHMODE_H
#define BATCHMODE_H

/** @brief Przeprowadza rozgrywkę w trybie wsadowym.
 * Czyta kolejne wiersze wejścia dopóki nie trafi na znak @p EOF.
 * Jeśli wiersz zaczyna się od znaku @p # lub składa się tylko ze znaku
 * przejścia do nowego wiersza, ignoruje go.
 * Jeśli wiersz zawiera niepoprawne polecenie, wypisuje na standardowe wyjście
 * diagnostyczne komunikat @p "ERROR line", gdzie @p line jest numerem wiersza.
 * Jeśli wiersz zawiera poprawne polecenie, wykonuje odpowiednią funkcję
 * i wypisuje jej wynik na standardowe wyjście. 
 * Wartość @p false wypisuje jako @p 0, a wartość @p true jako @p 1.
 * @param[in, out] line   – wskaźnik na numer aktualnej linii wejścia,
 * @param[in, out] g      – wskaźnik na wskaźnik na strukturę przechowującą 
 *                          stan gry.
 */
void batch(unsigned long long* line, gamma_t** g);

#endif /* BATCH_MODE_H */
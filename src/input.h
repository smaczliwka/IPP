/** @file
 * Interfejs umożliwiający czytanie wejścia w trybie wsadowym i przed
 * wyborem trybu.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#ifndef INPUT_H
#define INPUT_H

/** @brief Wczytuje odstęp.
 * Wczytuje puste znaki do końca linii lub pierwszego niepustego znaku.
 * @param[in, out] eof   – wskaźnik na informację, czy wczytano już znak @p EOF,
 * @param[in, out] eol   – wskaźnik na informację, czy wczytano już znak
                           przejścia do nowej linii,
 * @param[in, out] valid – wskaźnik na informację, czy aktualnie wczytywane
                           polecenie jest poprawne,
 * @param[in] c          – ostatni wcześniej wczytany znak.
 * @return Pierwszy wczytany niepusty znak lub @p 0, jeśli takiego nie będzie.
 */
int read_empty(bool* eof, bool* eol, bool* valid, int c);

/** @brief Wczytuje parametr.
 * Wczytuje niepuste znaki do końca linii lub pierwszego pustego znaku.
 * Jeśli ciąg znaków jest poprawnym parametrem, zapisuje ten parametr do liczby
 * wskazywanej przez @p target_param.
 * @param[in, out] eof   – wskaźnik na informację, czy wczytano już znak @p EOF,
 * @param[in, out] eol   – wskaźnik na informację, czy wczytano już znak przejścia
                           do nowej linii,
 * @param[in, out] valid – wskaźnik na informację, czy aktualnie wczytywane
                           polecenie jest poprawne,
 * @param[in] c          – ostatni wcześniej wczytany znak,
 * @param[in, out] target_param
 *                       – wskaźnik na liczbę, do której będzie zapisany poprawny
 *                       parametr.
 * @return Pierwszy wczytany pusty znak pusty lub równy @p EOF.
 */
int read_parameter(bool* eof, bool* eol, bool* valid, int c, uint32_t* target_param);

/** @brief Wczytuje linię.
 * Wczytuje wszystkie znaki do końca linii lub znaku @p EOF.
 * @param[in, out] eof   – wskaźnik na informację, czy wczytano już znak @p EOF,
 * @param[in] c          – ostatni wcześniej wczytany znak.
 */
void read_line(bool* eof, int c);

/** @brief Wypisuje komunikat @p "ERROR line".
 * Wypisuje na standardowe wyjście diagnostyczne komunikat @p "ERROR line",
 * gdzie @p line jest numerem wiersza.
 * @param[in] line  – numer wiersza, liczba nieujemna.
 */
void line_error(unsigned long long line);

#endif /* INPUT_H */
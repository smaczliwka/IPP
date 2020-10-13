/** @file
 * Interfejs umożliwiający przeprowadzanie rozgrywki w trybie interaktywnym.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */
 
#ifndef INTERACTIVEMODE_H
#define INTERACTIVEMODE_H

/** @brief Przeprowadza rozgrywkę w trybie interaktywnym.
 * Jeśli nie uda się zmienić trybu terminala lub pobrać parametrów okna,
 * wypisuje na standardowe wyjście diagnostyczne komunikat @p "TERMINAL ERROR",
 * usuwa z pamięci strukturę przechowującą stan gry i kończy działanie programu
 * z kodem @p 0.
 * Jeśli okno terminala jest za małe, by wyświetlić planszę, wypisuje na
 * standardowe wyjście diagnostyczne komunikat @p "BOARD TOO BIG",
 * usuwa z pamięci strukturę przechowującą stan gry i kończy działanie programu
 * z kodem @p 0.
 * W przeciwnym razie wyświetla planszę, a pod planszą wiersz
 * zachęcający gracza do wykonania ruchu. Prosi o wykonanie ruchu kolejnych
 * graczy, przy czym pomija graczy, dla których funkcja @ref gamma_free_fields
 * zwróciła @p 0 i funkcja @ref gamma_golden_possible zwróciła @p false. 
 * Ruch wykonuje się, przesuwając kursor na wybrane pole za pomocą klawiszy ze
 * strzałkami, a następnie wciskając klawisz @p spacja, aby wykonać zwykły ruch, 
 * lub klawisz @p G, aby wykonać złoty ruch. Gracz może zrezygnować z ruchu,
 * wciskając klawisz @p C. Wciśnięcie innych klawiszy jest ignorowane.
 * Gra kończy się, kiedy żaden z graczy nie może wykonać ruchu lub w momencie
 * wciśnięcia kombinacji klawiszy @p Ctrl-D. Wtedy pod planszą wypisywane jest
 * podsumowanie, ile pól zajął każdy z graczy.
 * @param[in, out] g – wskaźnik na wskaźnik na strukturę przechowującą stan gry.
 */
void interactive(gamma_t** g);

#endif /* INTERACTIVEMODE_H */
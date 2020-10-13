/** @file
 * Implementacja przeprowadzania rozgrywki w trybie interaktywnym.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#include "gamma.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/** @brief Działanie gracza.
 * Typ wyliczeniowy reprezentujący możliwe działania gracza.
 */
enum action {up, down, right, left, normal, golden, skip, quit};

/** @brief Czeka na poprawny znak.
 * Wczytuje kolejne znaki, dopóki nie wczyta znaku oznaczającego akcję gracza
 * lub zakończenie gry.
 * @return Działanie gracza, któremu odpowiada wczytany znak.
 */
static enum action wait_for_sign() {
  int prev = 0;
  int prev_prev = 0;
  while (1) {
    int x = getchar();
    
    // Najpierw sprawdź czy strzałka.
    if (prev_prev == '\e' && prev == '[') {
      if (x == 'A') return up; // W górę.
      if (x == 'B') return down; // W dół.
      if (x == 'C') return right; // W prawo.
      if (x == 'D') return left; // W lewo.
    }
    
    if (x == ' ') { // Spacja - zwykły ruch.
      return normal;
    }
    if (x == 'G' || x == 'g') { // G - złoty ruch.
      return golden;
    }
    if (x == 'C' || x == 'c') { // C - pomiń ruch.
      return skip;
    }
    if (x == CEOT) { // Koniec gry.
      return quit;
    }
    
    prev_prev = prev;
    prev = x;
  }
}

/** @brief Wypisuje komunikat zachęcający gracza do wykonania ruchu.
 * Czyści wiersz.
 * Wypisuje komunikat @p "PLAYER i", gdzie @p i jest numerem gracza, liczbę
 * pól zajętych przez tego gracza, liczbę wolnych pól, które może zająć
 * oraz znak @p G, jeśli gracz może wykonać złoty ruch, po czym ustawia kursor
 * na początku wiersza.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] i – numer gracza, liczba dodatnia.
 */
static void comunicate(gamma_t* g, int i) {
  printf("\033[K"); // Czyści linię.
  printf("PLAYER %u %lu %lu", i, 
          gamma_busy_fields(g, i), gamma_free_fields(g, i));
          
  if (gamma_golden_possible(g, i) == true) printf(" G");
  printf("\033[0G"); // Ustaw kursor w kolumnie 0.
}

/** @brief Ustawia kursor pod planszą.
 * Ustawia kursor znajdujący się na polu planszy (@p x, @p y) pod planszą,
 * w pierwszej kolumnie.
 * @param[in] x              – numer kolumny, liczba nieujemna,
 * @param[in] y              – numer wiersza, liczba nieujemna,
 * @param[in] width_of_field – szerokość pojedynczego pola planszy, liczba dodatnia.
 */
static void cursor_under_board(uint32_t x, uint32_t y, uint32_t width_of_field) {
  printf("\033[%uB", y + 1);// Ustaw kursor na wysokość.
  if (x > 0) 
    printf("\033[%uD", x * width_of_field); // Ustaw kursor na szerokość.
}

/** @brief Ustawia kursor z powrotem na planszy.
 * Ustawia kursor znajdujący się w pierwszej kolumnie pod planszą na polu
 * planszy (@p x, @p y).
 * @param[in] x              – numer kolumny, liczba nieujemna,
 * @param[in] y              – numer wiersza, liczba nieujemna,
 * @param[in] width_of_field – szerokość pojedynczego pola planszy, liczba dodatnia.
 */
static void cusor_back_to_board(uint32_t x, uint32_t y, uint32_t width_of_field) {
  printf("\033[%uA", y + 1); // Ustaw kursor na wysokość
  if (x > 0) 
    printf("\033[%uC", x * width_of_field); // Ustaw kursor na szerokość.
}

/** @brief Wypisuje pole planszy.
 * Wypisuje reprezentację pola (@p x, @p y) o odpowiedniej szerokości,
 * wskazanej przez funkcję @ref get_width_of_field.
 * Jeśli pole jest zajęte przez jakiegoś gracza, wypisuje numer tego gracza,
 * w przeciwnym wypadku wypisuje kropkę.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x – numer kolumny, liczba nieujemna,
 * @param[in] y – numer wiersza, liczba nieujemna.
 */
static void print_normal(gamma_t* g, uint32_t x, uint32_t y) {
  printf("\033[0m");                     
  if (player_on_position(g, x, y) != 0)
    printf("%*d", get_width_of_field(g), player_on_position(g, x, y));
  else 
    printf("%*c", get_width_of_field(g), '.');
  printf("\033[%uD", get_width_of_field(g));
}

/** @brief Wypisuje pole planszy w negatywie.
 * Wypisuje w negatywie reprezentację pola (@p x, @p y) o odpowiedniej
 * szerokości, wskazanej przez funkcję @ref get_width_of_field.
 * Jeśli pole jest zajęte przez jakiegoś gracza, wypisuje numer tego gracza,
 * w przeciwnym wypadku wypisuje kropkę.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x – numer kolumny, liczba nieujemna,
 * @param[in] y – numer wiersza, liczba nieujemna.
 */
static void print_highlighted(gamma_t* g, uint32_t x, uint32_t y) {
  printf("\033[7m");                     
  if (player_on_position(g, x, y) != 0)
    printf("%*d", get_width_of_field(g), player_on_position(g, x, y));
  else 
    printf("%*c", get_width_of_field(g), '.');
  printf("\033[%uD", get_width_of_field(g));
}

/** @brief Przesuwa kursor po planszy.
 * Przesuwa kursor znajdujący się na polu planszy (@p *x, @p *y) w zależności od
 * wartości @p c (o ile nie spowoduje to wyjścia poza planszę):
 * na pole po lewej dla @p left,
 * na pole po prawej dla @p right,
 * na pole powyżej dla @p up,
 * na pole poniżej dla @p down.
 * @param[in] c              – działanie gracza,
 * @param[in, out] x         – wskaźnik na numer kolumny, liczbę nieujemną,
 * @param[in, out] y         – wskaźnik na numer wiersza, liczbę nieujemną,
 * @param[in] width_of_field – szerokość pojedynczego pola planszy, liczba dodatnia,
 * @param[in] width          – liczba kolumn planszy, liczba dotatnia,
 * @param[in] height         – liczba wierszy planszy, liczba dodatnia.
 */
static void move_cursor(enum action c, uint32_t* x, uint32_t* y,
                     uint32_t width_of_field, uint32_t width, uint32_t height) {                    
  if (c == up) {
    if (*y + 1 < height) {
      printf("\033[1A");
      (*y)++;
    }
  }
  if (c == down) {
    if (*y >= 1) {
      printf("\033[1B");
      (*y)--;
    }
  }
  if (c == right) {
    if (*x + 1 < width) {
      printf("\033[%uC", width_of_field);
      (*x)++;
    }
  }
  if (c == left) {
    if (*x >= 1) {
      printf("\033[%uD", width_of_field);
      (*x)--;
    }
  }
}

/** @brief Struktura przechowująca informacje związane z terminalem.
 * Struktura przechowująca informacje związane z terminalem, wykorzystywana
 * przez funkcje @ref setup_console i @ref restore_console do przywrócenia
 * ustawień terminala.
 */
static struct termios oldattr;

/** @brief Zmienia ustawnienia terminala.
 * Wysyła odpowiednie sekwencje sterujące do terminala przed uruchomieniem
 * trybu interaktywnego.
 * @return Wartość @p true, jeśli operacja się powiedzie, w przeciwnym razie
 * wartość @p false.
 */
static bool setup_console() {
  struct termios newattr;
  if (tcgetattr(STDIN_FILENO, &oldattr) == -1) {
    return false;
  }
  newattr = oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &newattr) == -1) {
    return false;
  }
  return true;
}

/** @brief Przywraca ustawnienia terminala.
 * Wysyła odpowiednie sekwencje sterujące do terminala po zakończeniu działania
 * trybu interaktywnego.
 * @return Wartość @p true, jeśli operacja się powiedzie, w przeciwnym razie
 * wartość @p false.
 */
static bool restore_console() {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &oldattr) == -1) {
    return false;
  }
  return true;
}

void interactive(gamma_t** g) {
  bool end = false;
    
  printf("\e[1;1H\e[2J");
  
  uint32_t x = 0;
  uint32_t y = 0;
  
  uint32_t last_possible = 0;
  
  uint32_t players = get_players(*g);
  uint32_t width_of_field = get_width_of_field(*g);
  uint32_t width = get_width(*g);
  uint32_t height = get_height(*g);
  
  struct winsize w;
  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    fprintf(stderr, "TERMINAL ERROR\n");
    gamma_delete(*g);
    exit(1);
  }
  
  if ((uint64_t)(w.ws_col) < (uint64_t)(width) * (uint64_t)(width_of_field)
      || (uint32_t)(w.ws_row) < height + 1) {
    fprintf(stderr, "BOARD TOO BIG\n");
    gamma_delete(*g);
    exit(1);
  }
  
  char* p = gamma_board(*g);
  
  if (p == NULL) {
    gamma_delete(*g);
    exit(1);
  }
  
  printf("%s", p);
  free(p);
  
  if (setup_console() == false) {
    gamma_delete(*g);
    exit(1);
  }
  
  printf("\e[?25l"); // Chowa kursor.
  
  while (end == false) {
 
    for (uint32_t j = 0; j < players; j++) { // Dla każdego gracza.
      uint32_t i = j + 1;
      
      // Jeśli nie pomijam gracza.
      if ((gamma_free_fields(*g, i) > 0 || 
          gamma_golden_possible(*g, i) == true) && end == false) { 
          
        comunicate(*g, i);
        cusor_back_to_board(x, y, width_of_field);
        print_highlighted(*g, x, y);
        
        enum action c = wait_for_sign();
        while (c == up || c == down || c == left || c == right) {
          print_normal(*g, x, y);
          move_cursor(c, &x, &y, width_of_field, width, height);
          print_highlighted(*g, x, y);
          c = wait_for_sign();
        }
        
        if (c == quit) {
          end = true;
          print_normal(*g, x, y);
          cursor_under_board(x, y, width_of_field);
        }
        
        else {
          if (c == normal) {
            if(gamma_move(*g, i, x, y) == true) 
              printf("%*u\033[%uD", width_of_field, i, width_of_field);
          }
          if (c == golden) {
            if (gamma_golden_move(*g, i, x, y) == true) 
              printf("%*u\033[%uD", width_of_field, i, width_of_field);
          }
          print_normal(*g, x, y);
          cursor_under_board(x, y, width_of_field);
        }
        last_possible = i;
      }
      else {
        if (last_possible == i) {
          end = true;
          print_normal(*g, x, y);
        }
      }
    }
  }
  
  printf("\033[K"); // Czyści linię.
  for (uint32_t j = 0; j < players; j++) {
    int i = j + 1;
    printf("PLAYER %u %lu\n", i, gamma_busy_fields(*g, i));
  }
  printf("\e[?25h"); // Przywraca kursor.
  
  if (restore_console() == false) {
    gamma_delete(*g);
    exit(1);
  }
}
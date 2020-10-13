/** @file
 * Implementacja klasy przechowującej stan gry gamma
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.04.2020
 */

#include "gamma.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

/**
 * Struktura przechowująca stan pola.
 */
struct field {
  struct field* rep; ///<adres reprezentanta
  uint32_t player; ///<numer gracza
  bool visited; ///<informacja, czy zostało przetworzone przez funkcję @ref dfs
};

/**
 * Typ przechowujący stan pola.
 */
typedef struct field field_t;

/**
 * Struktura przechowująca stan gry.
 */
struct gamma {
  field_t** tab; ///<tablica pól planszy
  uint32_t width; ///<szerokość planszy
  uint32_t height; ///<wysokość planszy
  uint32_t players; ///<numba graczy
  uint32_t areas; ///<maksymalna liczba obszarów jednego gracza
  
  uint32_t* areas_of_player; 
  ///<tablica przechowująca aktualną liczbę obszarów każdego z graczy
  bool* golden_move; 
  ///<tablica przechowująca informację, czy dany gracz już wykonał złoty ruch
  
  uint64_t* fields_of_player; 
  ///<tablica przechowująca aktualną liczbę pól zajętych przez każdego z graczy
  uint64_t* neighbours_of_player; 
  ///<tablica przechowująca liczbę wolnych pól sąsiadujących z polami danego gracze
  uint64_t free_fields; ///<aktualna liczba wolnych pól na planszy
  
  uint32_t width_of_field; 
  ///<szerokość jednego pola w tekstowej reprezentacji planszy
};

void gamma_delete(gamma_t *g) {
  if (g != NULL) {
    for (uint32_t i = 0; i < (*g).height; i++) {
      free((*g).tab[i]);
    }
    free((*g).tab);
    
    free((*g).areas_of_player);
    free((*g).fields_of_player);
    free((*g).golden_move);
    free((*g).neighbours_of_player);
    
    free(g);
  }
}

/** @brief Liczy ilu znaków potrzeba do wypisania liczby.
 * Liczy ilu znaków potrzeba do wypisania liczby @p x w tekstowym opisie planszy.
 * @param[in] x       – liczba nieujemna.
 * @return Liczba znaków potrzebnych do wypisania liczby.
 */
static uint64_t number_of_characters(uint32_t x) {
	if (x == 0) return 1;
  
	uint64_t num = 0;
	while (x > 0) {
		num++;
		x /= 10;
	}
	return num;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
  
  if (width <= 0 || height <= 0 || players <= 0 || areas <= 0) return NULL;
 
  gamma_t* new = (gamma_t*)malloc(sizeof(gamma_t));
  if (new == NULL) return NULL;
 
  (*new).tab = (field_t**) malloc(sizeof(field_t*) * height);
  if ((*new).tab == NULL) {
    free(new);
    return NULL;
  }
  
  for (uint32_t i = 0; i < height; i++) {
    (*new).tab[i] = (field_t*) malloc(sizeof(field_t) * width);
    
    if ((*new).tab[i] == NULL) {
      for (uint32_t j = 0; j < i; j ++) {
        free((*new).tab[j]);
      }
      free((*new).tab);
      free(new);
      return NULL;
    }
  }
  
  for (uint32_t i = 0; i < height; i++) {
    for (uint32_t j = 0; j < width; j++) {
      ((*new).tab[i][j]).player = 0;
      ((*new).tab[i][j]).rep = &((*new).tab[i][j]);
      ((*new).tab[i][j]).visited = false;
    }
  }
  
  (*new).areas_of_player = (uint32_t*) malloc(sizeof(uint32_t) * (players + 1));
  
  if ((*new).areas_of_player == NULL) {
    for (uint32_t i = 0; i < height; i++) {
      free((*new).tab[i]);
    }
    free((*new).tab);
    free(new);
    return NULL;
  }
  
  (*new).fields_of_player = (uint64_t*) malloc(sizeof(uint64_t) * (players + 1));
  
  if ((*new).fields_of_player == NULL) {
    for (uint32_t i = 0; i < height; i++) {
      free((*new).tab[i]);
    }
    free((*new).tab);
    free((*new).areas_of_player);
    free(new);
    return NULL;
  }
  
  (*new).neighbours_of_player = (uint64_t*) malloc(sizeof(uint64_t) * (players + 1));
  
  if ((*new).neighbours_of_player == NULL) {
    for (uint32_t i = 0; i < height; i++) {
      free((*new).tab[i]);
    }
    free((*new).tab);
    free((*new).areas_of_player);
    free((*new).fields_of_player);
    free(new);
    return NULL;
  }
  
  (*new).golden_move = (bool*) malloc(sizeof(bool) * (players + 1));
  
  if ((*new).golden_move == NULL) {
    for (uint32_t i = 0; i < height; i++) {
      free((*new).tab[i]);
    }
    free((*new).tab);
    free((*new).areas_of_player);
    free((*new).fields_of_player);
    free((*new).neighbours_of_player);
    free(new);
    return NULL;
  }
  
  for (uint32_t i = 0; i <= players; i++) {
    (*new).areas_of_player[i] = 0;
    (*new).fields_of_player[i] = 0;
    (*new).neighbours_of_player[i] = 0;
    (*new).golden_move[i] = false;
  }
  
  (*new).free_fields = (uint64_t)(width) * (uint64_t)(height);
  (*new).height = height;
  (*new).width = width;
  (*new).players = players;
  (*new).areas = areas;
  
  if (players <= 9) (*new).width_of_field = 1;
  else (*new).width_of_field = number_of_characters(players) + 1;
  
  return new;
}

/** @brief Sprawdza czy pole sąsiaduje z polem gracza.
 * Sprawdza czy w grze wskazywanej przez @p g pole (@p x, @p y)
 * sąsiaduje z jakimś polem gracza @p player.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli tak, @p false w przeciwnym wypadku.
 */
static bool neighbour(gamma_t g, uint32_t player, uint32_t x, uint32_t y) {
  if (x > 0 && g.tab[y][x - 1].player == player) return true;
  if (x < g.width - 1 && g.tab[y][x + 1].player == player) return true;
  if (y > 0 && g.tab[y - 1][x].player == player) return true;
  if (y < g.height - 1 && g.tab[y + 1][x].player == player) return true;
  return false;
}

/** @brief Zwraca adres reprezentanta.
 * Zwraca adres pola będącego reprezentantem obszaru, do którego należy
 * pole wskazywane przez @p x.
 * @param[in] x       – wskaźnik na pole.
 * @return Wskaźnik na pole będące reprezentantem obszaru.
 */
static field_t* find(field_t* x) {
	if ((*x).rep == x) { // Sam jest swoim reprezentantem.
		return x; // Zwraca adres.
	}
	(*x).rep = find((*x).rep);
	return (*x).rep;
}

/** @brief Łączy dwa obszary.
 * Łączy dwa obszary, do których należą pola wskazywane przez @p a i @p b.
 * Jeśli nie były wcześniej połączone, 
 * zmniejsza o 1 liczbę obszarów należących do gracza @p player.
 * @param[in] a       – wskaźnik na pierwsze pole,
 * @param[in] b       – wskaźnik na drugie pole,
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new.
 */
static void uni(field_t *a, field_t* b, gamma_t* g, uint32_t player) {
	field_t* temp_a = find(a);
  field_t* temp_b = find(b);
  
  if (temp_a != temp_b) { // Jeśli mają różnych reprezentantów.
    (*temp_b).rep = temp_a;
    (*g).areas_of_player[player]--; // Zmniejszam liczbę obszarów.
  }
}

/** @brief Łączy pole i jego sąsiadów w jeden obszar.
 * Przypisuje polu (@p x, @p y) numer gracza @p player.
 * Aktualizuje liczbę obszarów i pól zajętych przez gracza @p player.
 * Łączy obszary, w których leżą należące do gracza @p player
 * pola sąsiadujące z polem (@p x, @p y) i obszar pola (@p x, @p y).
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 */
static void uni_neighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  (*g).areas_of_player[player]++; // Dodaję nowy.
  (*g).fields_of_player[player]++;
  (*g).tab[y][x].player = player;
  
  if (x > 0 && (*g).tab[y][x - 1].player == player) {
    uni(&((*g).tab[y][x - 1]), &((*g).tab[y][x]), g, player);
  }
  if (x < (*g).width - 1 && (*g).tab[y][x+1].player == player) {
    uni(&((*g).tab[y][x + 1]), &((*g).tab[y][x]), g, player);
  }
  if (y > 0 && (*g).tab[y - 1][x].player == player) {
    uni(&((*g).tab[y - 1][x]), &((*g).tab[y][x]), g, player);
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player == player) {
    uni(&((*g).tab[y + 1][x]), &((*g).tab[y][x]), g, player);
  }
}

/** @brief Liczy wolnych sąsadów pola.
 * Liczy wolne pola sąsiadujące z polem (@p x, @p y), niesąsiadujące z żadnym
 * polem gracza @p player;
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Liczba pól spełniających odpowiednie warunki.
 */
static uint32_t check_neighbours(gamma_t* g, uint32_t player,
                                  uint32_t x, uint32_t y) {
  uint32_t count = 0;
  // Jeśli sąsiad jest wolny i nie sąsiaduje z żadnym innym moim.
  if (x > 0 && (*g).tab[y][x - 1].player == 0) { 
    if (neighbour(*g, player, x - 1, y) == false) count++;
  }
  if (x < (*g).width - 1 && (*g).tab[y][x + 1].player == 0) {
    if (neighbour(*g, player, x + 1, y) == false) count++;
  }
  if (y > 0 && (*g).tab[y - 1][x].player == 0) {
    if (neighbour(*g, player, x, y - 1) == false) count++;
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player == 0) {
    if (neighbour(*g, player, x, y + 1) == false) count++;
  }
  return count;
}

/** @brief Zmniejsza liczbę wolnych sąsiadów graczy sąsiadujących z polem.
 * Zmniejsza o 1 liczbę wolnych sąsiadów graczy innyh niż @p player, 
 * których pola sąsiadują z polem (@p x, @p y).
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 */
static void change_neighbours(gamma_t* g, uint32_t player,
                              uint32_t x, uint32_t y) {
  uint32_t p = 0, q = 0, r = 0;
  
  // Jeśli sąsiad nie jest moim i nie jest zerem.
  if (x > 0 && (*g).tab[y][x - 1].player != player 
      && (*g).tab[y][x - 1].player != 0) { 
    p = (*g).tab[y][x - 1].player;
    (*g).neighbours_of_player[p]--;
  }
  if (x < (*g).width - 1 && (*g).tab[y][x + 1].player != player 
      && (*g).tab[y][x + 1].player != 0) {
    if ((*g).tab[y][x + 1].player != p) {
      q = (*g).tab[y][x + 1].player;
      (*g).neighbours_of_player[q]--;
    }
  }
  if (y > 0 && (*g).tab[y - 1][x].player != player 
      && (*g).tab[y - 1][x].player != 0) {
    if ((*g).tab[y - 1][x].player != p && (*g).tab[y - 1][x].player != q) {
      r = (*g).tab[y - 1][x].player;
      (*g).neighbours_of_player[r]--;
    }
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player != player 
      && (*g).tab[y + 1][x].player != 0) {
    if ((*g).tab[y + 1][x].player != p && (*g).tab[y + 1][x].player != q 
        && (*g).tab[y + 1][x].player != r) {
      (*g).neighbours_of_player[(*g).tab[y + 1][x].player]--;
    }
  }
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  // Czy parametry prawidłowe?
  if (g == NULL) return false;
  if (player <= 0 || player > (*g).players) return false;
  if (x >= (*g).width) return false;
  if (y >= (*g).height) return false;
  // Jest tu pionek jakiegoś gracza.
  if (((*g).tab[y][x]).player != 0) return false; 
  // Pole nie sąsiaduje z moim polem.
  if (neighbour(*g, player, x, y) == false) { 
    // Za dużo obszarów.
    if ((*g).areas_of_player[player] == (*g).areas) return false; 
    
    // Aktualizuję liczbę wolnych sąsiadów gracza.
    (*g).neighbours_of_player[player] += check_neighbours(g, player, x, y); 
    change_neighbours(g, player, x, y);
    
    (*g).tab[y][x].player = player; // Dodaję nowy obszar.
    (*g).areas_of_player[player]++;
    (*g).fields_of_player[player]++;
    (*g).free_fields--;
    
    return true;
  }
  
  else { // Pole sąsiaduje z przynajmniej jednym moim polem.
    (*g).free_fields--;
    
    (*g).neighbours_of_player[player]--; // To pole już nie jest wolnym sąsiadem.
    // Aktualizuję liczbę wolnych sąsiadów.
    (*g).neighbours_of_player[player] += check_neighbours(g, player, x, y); 
    change_neighbours(g, player, x, y);
    
    uni_neighbours(g, player, x, y);
    
    return true;
  }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  if (g == NULL || player > (*g).players || player <= 0) return 0;
  return (*g).fields_of_player[player];
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  if (g == NULL || player > (*g).players || player <= 0) return 0;
  
  if ((*g).areas_of_player[player] == (*g).areas) 
    return (*g).neighbours_of_player[player];
  
  return (*g).free_fields;
}

/** @brief Przechodzi obszar wgłąb.
 * Przechodzi wgłąb należący do gracza @p player obszar zawierający
 * pole (@p x, @p y), zmieniając reprezentanta pól na @p new_rep oraz informację
 * o ich przetworzeniu na @p b.
 * @param[in, out] g  – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] new_rep – wskaźnik na pole,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new,
 * @param[in] b       – wartość @p true lub @p false.
 */
static void dfs(gamma_t* g, field_t* new_rep, 
                uint32_t player, uint32_t x, uint32_t y, bool b) {
  
  (*g).tab[y][x].rep = new_rep; 
  (*g).tab[y][x].visited = b;
  
  if (x > 0 && (*g).tab[y][x - 1].player == player 
      && (*g).tab[y][x - 1].visited != b) {
    dfs(g, new_rep, player, x - 1, y, b);
  }
  if (x < (*g).width - 1 && (*g).tab[y][x + 1].player == player 
      && (*g).tab[y][x + 1].visited != b) {
    dfs(g, new_rep, player, x + 1, y, b);
  }
  if (y > 0 && (*g).tab[y - 1][x].player == player 
      && (*g).tab[y - 1][x].visited != b) {
    dfs(g, new_rep, player, x, y - 1, b);
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player == player 
      && (*g).tab[y + 1][x].visited != b) {
    dfs(g, new_rep, player, x, y + 1, b);
  }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  if (g == NULL || player > (*g).players || player <= 0) return false;
  
  if ((*g).golden_move[player] == false) { // Nie wykonał złotego ruchu.
  
  // Istnieje jakieś pole należące do innego gracza.
   if ((*g).free_fields + (*g).fields_of_player[player] < 
        (uint64_t)(*g).width * (uint64_t)(*g).height) {
          
     // Liczba moich obszarów jest mniejsza niż maksymalna.
     if ((*g).areas_of_player[player] < (*g).areas) {
       return true;
     }
     
     else { // Liczba moich obszarów jest maksymalna;
       for (uint32_t y = 0; y < (*g).height; y++) {
         for (uint32_t x = 0; x < (*g).width; x++) {
           
           //Jeśli pole innego gracza sąsiaduje z moim obszarem.
           if ((*g).tab[y][x].player != 0 && (*g).tab[y][x].player != player
            && neighbour(*g, player, x, y) == true) {
             uint32_t prev_player = (*g).tab[y][x].player; // Poprzedni gracz.
             
             //Jeśli udało się na nim wykonać złoty ruch.
             if (gamma_golden_move(g, player, x, y) == true) {
              // Cofamy jego skutki.
              bool temp = (*g).golden_move[prev_player];
              (*g).golden_move[prev_player] = false;
              gamma_golden_move(g, prev_player, x, y);
              (*g).golden_move[prev_player] = temp;
              (*g).golden_move[player] = false;
              return true;
             }
           }
         }
       }
     }
   }
  }
  return false;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  // Czy parametry prawidłowe?
  if (g == NULL) return false;
  if (player <= 0 || player > (*g).players) return false;
  if (x >= (*g).width) return false;
  if (y >= (*g).height) return false;
  
  // Gracz wykonał złoty ruch.
  if ((*g).golden_move[player] == true) return false; 
  // Ruch nie jest złoty.
  if ((*g).tab[y][x].player == 0 || (*g).tab[y][x].player == player) return false; 
  // Za dużo obszarów.
  if (neighbour(*g, player, x, y) == false 
      && (*g).areas_of_player[player] == (*g).areas) return false; 
  
  uint32_t prev_player = (*g).tab[y][x].player; // Poprzedni gracz.
  
  // Tyle wolnych do dodania w przypadku wstawienia.
  uint32_t to_add = check_neighbours(g, player, x, y); 
  
  (*g).tab[y][x].player = player; // Niech pole puste.
  (*g).tab[y][x].rep = &((*g).tab[y][x]);
  (*g).fields_of_player[prev_player]--; 
  // Zmieniam liczbę pól poprzedniego gracza.
  uint32_t parts = 0;
  
  if (x > 0 && (*g).tab[y][x - 1].player == prev_player) {
    parts++;
    dfs(g, &((*g).tab[y][x - 1]), prev_player, x - 1, y, true);
  }
  if (x < (*g).width - 1 && (*g).tab[y][x + 1].player == prev_player
      && (*g).tab[y][x + 1].visited == false) {
    parts++;
    dfs(g, &((*g).tab[y][x + 1]), prev_player, x + 1, y, true);
  }
  if (y > 0 && (*g).tab[y - 1][x].player == prev_player
      && (*g).tab[y - 1][x].visited == false) {
    parts++;
    dfs(g, &((*g).tab[y - 1][x]), prev_player, x, y - 1, true);
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player == prev_player
      && (*g).tab[y + 1][x].visited == false) {
    parts++;
    dfs(g, &((*g).tab[y + 1][x]), prev_player, x, y + 1, true);
  }
  
  // Cofam odwiedzenie pól.
  if (x > 0 && (*g).tab[y][x - 1].player == prev_player) {
    dfs(g, &((*g).tab[y][x - 1]), prev_player, x - 1, y, false);
  }
  if (x < (*g).width - 1 && (*g).tab[y][x + 1].player == prev_player
      && (*g).tab[y][x + 1].visited == true) {
    dfs(g, &((*g).tab[y][x + 1]), prev_player, x + 1, y, false);
  }
  if (y > 0 && (*g).tab[y - 1][x].player == prev_player
      && (*g).tab[y - 1][x].visited == true) {
    dfs(g, &((*g).tab[y - 1][x]), prev_player, x, y - 1, false);
  }
  if (y < (*g).height - 1 && (*g).tab[y + 1][x].player == prev_player
      && (*g).tab[y + 1][x].visited == true) {
    dfs(g, &((*g).tab[y + 1][x]), prev_player, x, y + 1, false);
  }
  
  // Zmieniam liczbę obszarów poprzedniego gracza.
  (*g).areas_of_player[prev_player] += parts; 
  (*g).areas_of_player[prev_player]--;
  
  // Można usunąć bez naruszania zasad.
  if ((*g).areas_of_player[prev_player] <= (*g).areas) { 
    // Zmniejszam liczbę wolnych sąsiadów poprzedniego gracza.
    (*g).neighbours_of_player[prev_player] -= check_neighbours(g, prev_player, x, y); 
    // Zwiększam liczbę wolnych sąsiadów nowego gracza.
    (*g).neighbours_of_player[player] += to_add; 
    uni_neighbours(g, player, x, y); // Wstawiam.
    (*g).golden_move[player] = true;
    
    return true;
  }
  
  else{
    uni_neighbours(g, prev_player, x, y);
    return false;
  }
}

/** @brief Zwraca cyfrę w formie znaku.
 * Zwraca znak reprezentujący cyfrę @p x;
 * @param[in] x       – liczba nieujemna mniejsza niż 10.
 * @return Znak reprezentujący cyfrę lub 0, jeśli parametr niepoprawny.
 */
static char to_char(uint32_t x) {
  if (x >= 10) return 0;
  return (char)(x) + '0';
}

/** @brief Dopisuje liczbę na koniec napisu.
 * Dopisuje tekstową reprezentację liczby @p x na koniec napisu wskazywanego
 * przez @p c.
 * @param[in] c       – wskaźnik na napis,
 * @param[in] num     – wskaźnik na numer pierwszej wolnej pozycji napisu,
 * @param[in] x       – liczba nieujemna,
 * @param[in] width_of_field
                      – liczba pozycji, jaką ma zająć tekstowa reprezentacja
                        liczby @p x w napisie.
 */
static void add_char(char** c, uint64_t* num, uint32_t x, uint32_t width_of_field) {
  uint32_t l = number_of_characters(x); // Długość tej liczby.
  for (uint32_t i = l; i < width_of_field; i++) {
    (*c)[*num] = ' ';
    (*num)++; 
  }
  if (x < 10) {
    if (x == 0)
      (*c)[*num] = '.';
		else 
      (*c)[*num] = to_char(x);
    (*num)++;
	}
  else {
    uint32_t m = 1;
    for (uint32_t i = 1; i < l; i++) {
      m *= 10;
    }
    while (m > 0) {
      (*c)[*num] = to_char(x/m);
      (*num)++;
      x %= m;
      m /= 10;
    }
  }
}

char* gamma_board(gamma_t* g) {
  
  if (g == NULL) return NULL;
  
  uint64_t num = 0;
  
  char* c = (char*) malloc(sizeof(char) * 
    ((((uint64_t)((*g).width) * (uint64_t)(*g).width_of_field)+ 1)
    * (uint64_t)((*g).height) + 1));
  
  if (c == NULL) return NULL;
  
  for (uint32_t i = 0; i < (*g).height; i++) {
    for (uint32_t j = 0; j < (*g).width; j++) {
      add_char(&c, &num, (*g).tab[(*g).height - 1 - i][j].player,
      (*g).width_of_field);
    }
    c[num] = '\n';
    num++;
  }
  c[num] = 0;
  return c;
}

/** @brief Podaje szerokość pojedynczego pola.
 * Podaje szerokość pojedynczego pola w tekstowym opisie stanu planszy
 * w grze wskazywanej przez @p g.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry.
 * @return Szerokość pojednyczego pola planszy, liczba dodatnia.
 */
uint32_t get_width_of_field(gamma_t* g) {
  return (*g).width_of_field;
}

uint32_t get_width(gamma_t* g) {
  return (*g).width;
}

uint32_t get_height(gamma_t* g) {
  return (*g).height;
}

uint32_t get_players(gamma_t* g) {
  return (*g).players;
}

uint32_t player_on_position(gamma_t* g, int x, int y) {
  return (*g).tab[y][x].player;
}

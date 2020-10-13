/** @file
 * Implementacja funkcji służących do czytania wejścia w trybie wsadowym
 * i przed wyborem trybu.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/** @brief Sprawdza, czy znak jest pusty.
 * Sprawdza czy znak @p x jest znakiem pustym lub czy jest równy @p EOF.
 * @param[in] x   – znak jako liczba całkowita.
 * @return Wartość @p true, jeśli z jest znakiem pustym lub równym @p EOF,
 * wartość @p false w przeciwnym wypadku.
 */
static bool empty(int x) {
  if (x == ' ') return true;
  if (x == '\t') return true;
  if (x == '\v') return true;
  if (x == '\r') return true;
  if (x == '\n') return true;
  if (x == '\f') return true;
  if (x == EOF) return true;
  
  return false;
}

int read_empty(bool* eof, bool* eol, bool* valid, int c) {
  if (*eol == true || *eof == true) return 0;
  if (empty(c) == false) *valid = false; // Nie ma odstępu.
  while (empty(c) == true && c!= EOF && c!= '\n') c = getchar();
  if (c == EOF) {
    *eof = true;
    return 0;
  }
  if (c == '\n') {
    *eol = true;
    return 0;
  }
  return c;
}

int read_parameter(bool* eof, bool* eol, bool* valid, int c, uint32_t* target_param) {
  if (*eol == true || *eof == true) { // Chcę czytać więcej niż mogę.
    *valid = false;
    return c;
  }
  uint64_t param = 0;
  while (empty(c) == false) {
    if (c < '0' || c > '9') *valid = false; // Nie cyfra.
    if (*valid == true) param = (param * 10) + (uint64_t)(c - '0');
    if (param > UINT32_MAX) *valid = false; // Za duża liczba.
    c = getchar();
  }
  if (c == EOF) {
    *eof = true;
    *valid = false; // Nie ma '\n' jak w poprawnym poleceniu.
  }
  if (c == '\n') {
    *eol = true;
  }
  *target_param  = (uint32_t)(param);
  return c;
}

void read_line(bool* eof, int c) {
  while (c != '\n' && c != EOF) c = getchar();
  if (c == EOF) *eof = true;
}

void line_error(unsigned long long line){
  fprintf(stderr, "ERROR %lld\n", line); // Zły wiersz.
}
/** @file
 * Implementacja przeprowadzania rozgrywki w trybie interaktywnym.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */
 
#include "gamma.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/** @brief Sprawdza czy znak oznacza pewną komenę.
 * Sprawdza czy znak @p c jest poprawnym początkiem polecenia w trybie wsadowym.
 * @param[in] c   – znak jako liczba całkowita,
 * @return Watrość @p true, jeśli @p c może być początkiem pewnego polecenia,
 * wartość @p false w przeciwnym przyapdku.
 */
static bool command(int c) {
  if (c == 'm' || c == 'g' || c == 'b' || c == 'f' || c == 'q' || c == 'p')
    return true;
  return false;
}

void batch(unsigned long long* line, gamma_t** g) {
  bool eof = false;
  
  while (eof == false) {
    int c = getchar();
    (*line)++;
    
    if (command(c) == false) {
      if (c != '#' && c != '\n' && c != EOF)
        line_error(*line);
      read_line(&eof, c);
    }
    
    else {
      if (c == 'm' || c == 'g') {
        bool eol = false;
        bool valid = true;
        uint32_t player, x, y;
        
        int next_int = getchar();
        next_int = read_parameter(&eof, &eol, &valid,
          read_empty(&eof, &eol, &valid, next_int), &player);
        next_int = read_parameter(&eof, &eol, &valid,
          read_empty(&eof, &eol, &valid, next_int), &x);
        next_int = read_parameter(&eof, &eol, &valid,
          read_empty(&eof, &eol, &valid, next_int), &y);
        
        if (eol == false) {
          char temp = read_empty(&eof, &eol, &valid, getchar());
          if (eol == false) {
            read_line(&eof, temp);
            valid = false;
          }
        }
        
        if (valid == true) {
          if (c == 'm') printf("%d\n",(int)(gamma_move((*g), player, x, y)));
          if (c == 'g') printf("%d\n",(int)gamma_golden_move((*g), player, x, y));
        }
        else {
          line_error(*line);
        }
      }
      
      if (c == 'b' || c == 'f' || c == 'q') {
        bool eol = false;
        bool valid = true;
        uint32_t player;
        
        int next_int = getchar();
        next_int = read_parameter(&eof, &eol, &valid,
          read_empty(&eof, &eol, &valid, next_int), &player);
        
        if (eol == false) {
          char temp = read_empty(&eof, &eol, &valid, getchar());
          if (eol == false) {
            read_line(&eof, temp);
            valid = false;
          }
        }
        
        if (valid == true) {
          if (c == 'b') printf("%lu\n", gamma_busy_fields((*g), player));
          if (c == 'f') printf("%lu\n", gamma_free_fields((*g), player));
          if (c == 'q') printf("%d\n", (int)gamma_golden_possible((*g), player));
        }
        else {
          line_error(*line);
        }
      }
      
      if (c == 'p') {
        bool valid = true;
        bool eol = false;
        char temp = read_empty(&eof, &eol, &valid, getchar());
        if (eol == false) {
          read_line(&eof, temp);
          valid = false;
        }
        if (valid == true) {
          char* p = gamma_board((*g));
          
          if (p != NULL) printf("%s", p);
          else line_error(*line);
          
          free(p);
        }
        else {
          line_error(*line);
        }
      }
    }
  } 
}
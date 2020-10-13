/** @file
 * Implementacja funkcji wyboru trybu.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#include "gamma.h"
#include "input.h"
#include "mode.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum mode choose_mode(unsigned long long* line, gamma_t** g) {
  bool eof = false;
  
  while (*g == NULL && eof == false) {
    int c = getchar();
    (*line)++;
    
    if (c != 'B' && c != 'I') {
      if (c != '#' && c != '\n' && c != EOF)
        line_error(*line);
      read_line(&eof, c);
    }
    
    if (c == 'B' || c == 'I') {
      bool eol = false;
      bool valid = true;
      uint32_t width, height, players, areas;
      
      int next_int = getchar();
      next_int = read_parameter(&eof, &eol, &valid,
                              read_empty(&eof, &eol, &valid, next_int), &width);
      next_int = read_parameter(&eof, &eol, &valid,
                             read_empty(&eof, &eol, &valid, next_int), &height);
      next_int = read_parameter(&eof, &eol, &valid,
                            read_empty(&eof, &eol, &valid, next_int), &players);
      next_int = read_parameter(&eof, &eol, &valid,
                              read_empty(&eof, &eol, &valid, next_int), &areas);
      
      if (eol == false) {
        char temp = read_empty(&eof, &eol, &valid, getchar());
        if (eol == false) {
          read_line(&eof, temp);
          valid = false;
        }
      } 
      
      if (valid == false) {
        line_error(*line);
      }
      
      else {
        *g = gamma_new(width, height, players, areas); // Inicjacja gry.
        if (*g != NULL) {
          if (c == 'B' && eof == false) {
            printf("OK %lld\n", *line);
            return batch_mode;
          }
          if (c == 'I' && eof == false) return interactive_mode;
        }
        else{
          line_error(*line);
        }
      }
    }
  }
  return end_of_file;
}
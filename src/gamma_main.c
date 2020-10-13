/** @file
 * Implementacja programu umożliwiającego przeprowadzenie rozgrywki,
 * korzystającego z modułu slinkia gry gamma.
 *
 * @author Karolina Drabik <kd417818@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 17.05.2020
 */

#include "gamma.h"
#include "input.h"
#include "mode.h"
#include "batchmode.h"
#include "interactivemode.h"
#include <stdio.h>
#include <stdlib.h>

/** @brief Przeprowadza rozgrywkę.
 * Wybiera tryb rozgrywki, a następnie przeprowadza grę w trybie wsadowym
 * lub interaktywnym. Na koniec usuwa strukturę przechowującą stan gry.
 */
int main(){
  gamma_t* g = NULL;
  unsigned long long line = 0;
  enum mode m = choose_mode(&line, &g);
  
  if (m == batch_mode) { 
    batch(&line, &g);
  }
  
  if (m == interactive_mode) {
    interactive(&g);
  }
  
  gamma_delete(g);
  return 0;
}
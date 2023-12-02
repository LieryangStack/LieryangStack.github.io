#include <glib.h>
#include <stdio.h>

G_LOCK_DEFINE (current_number);

int
give_me_next_number (void)
{
  static int current_number = 0;
  int ret_val;

  G_LOCK (current_number);
  ret_val = current_number = calc_next_number (current_number);
  G_UNLOCK (current_number);

  return ret_val;
}


int main() {

  

  return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include <locale.h>
#include <string.h>

int main (int argc, char * argv[]) {
  char * prevLocale = setlocale (LC_NUMERIC, NULL);
  char * tmp = prevLocale;
  size_t localeLen = 1 + strlen (tmp);
  prevLocale = (char *) malloc (sizeof(char) * localeLen);
  strncpy (prevLocale, tmp, localeLen);

  char buf[64];

  printf ("%s <--   locale numeric config\n", prevLocale);
  sprintf (buf, "%lf", 0.03);
  printf ("%s <--    current number formatting !\n", buf);

  printf ("%s <--   locale numeric config\n", setlocale(LC_NUMERIC, "C"));
  sprintf (buf, "%lf", 0.03);
  printf ("%s <--    C number formatting !\n", buf);

  printf ("%s <--   locale numeric config\n", setlocale(LC_NUMERIC, "fr_FR.utf8"));
  sprintf (buf, "%lf", 0.03);
  printf ("%s <--    FR number formatting (with a comma instead of a dot) !\n", buf);

  setlocale(LC_NUMERIC, prevLocale);
  free(prevLocale);

  return 0;
}

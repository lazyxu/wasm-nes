
#include "mirror.h"

/**
 * Names of the mirrors.
 */
static const char * const g_mirror_names[] =
{
  "HORIZONTAL", "VERTICAL", "SINGLE0", "SINGLE1", "FOUR"
};
const char * const get_mirror_name(mirror_t mirror) {
  return g_mirror_names[mirror];
}
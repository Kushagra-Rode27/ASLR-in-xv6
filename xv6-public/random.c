#include "types.h"
#include "defs.h"
#define RAND_MAX 1000
static uint seed = 1; // Seed for the random number generator

// Seed the random number generator
void srand(uint s)
{
  seed = s;
}

// Generate a pseudo-random number between 0 and RAND_MAX
int rand(void)
{
  // LCG parameters (use any values you like)
  const uint a = 12345;
  const uint c = 1103515245;

  // Update the seed
  seed = c * seed + a;

  // Return the pseudo-random number
  return (seed & 0x7fffffff) % RAND_MAX;
}

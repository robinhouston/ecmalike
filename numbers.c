#include <stdint.h>

#include "numbers.h"

// Originally from Forišek’s code; extracted from forisek_prime.c for reuse
void xbinGCD(uint64_t a, uint64_t b, uint64_t *pu, uint64_t *pv)  { 
  uint64_t alpha, beta, u, v; 
  u = 1; v = 0; 
  alpha = a; beta = b; // Note that alpha is 
  // even and beta is odd. 
 
  /* The invariant maintained from here on is: 
  a = u*2*alpha - v*beta. */ 

  while (a > 0) { 
    a = a >> 1; 
    if ((u & 1) == 0) { // Delete a common 
      u = u >> 1; v = v >> 1; // factor of 2 in 
    } // u and v. 
    else { 
      /* We want to set u = (u + beta) >> 1, but 
      that can overflow, so we use Dietz's method. */ 
      u = ((u ^ beta) >> 1) + (u & beta); 
      v = (v >> 1) + alpha; 
    }
  }
 
  *pu = u; 
  *pv = v; 
  return;
}

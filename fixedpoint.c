#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
//this is a test
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint x;
  x.whole_part = whole;
  x.frac_part = 0;
  x.tags = vnon;
  return x;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint x;
  x.whole_part = whole;
  x.frac_part = frac;
  x.tags = vnon;
  return x;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole_part;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac_part;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  u_int64_t result = 0;
  //If they are both positive
  if ((left.tags == vnon) && (right.tags == vnon)) {
    result = 
    int carry_val;
    if ()
  }
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
<<<<<<< HEAD
  //need to make sure that they dont differ in signs
  if (left.tags == vneg && right.tags == vnon) {
=======
  uint64_t leftWhole = left.whole_part;
  uint64_t leftFrac = left.frac_part;
  uint64_t rightWhole = right.whole_part;
  uint64_t rightFrac = left.frac_part;
  if (left.tags == vneg && right.tags == vnon) {
    return -1;
  }
  else if (left.tags == vnon && right.tags == vneg) {
    return 1;
  }
  
  if (leftWhole < rightWhole) {
>>>>>>> 0d1c78a6806050737cccebd45583649753f71060
    return -1;
  } else if (left.tags == vnon && right.tags == vneg) {
    return 1;
  }

  uint64_t c_whole = left.whole_part^right.whole_part;
  uint64_t c_frac = left.frac_part^right.frac_part;

  //Think this will only work for positive values of left and right
  if (c_whole) { //This means that the Fixedpoint values are not equivalent in the whole parts
    //need to compare the most significant bit
    return whole_compare(left.whole_part, right.whole_part);
  }
  if (c_frac) { //Fixedpoint values are not equivalent in the frac parts
    //compare most significant bit
    return whole_compare(left.frac_part, right.frac_part);
  }
  return 0;
}

int whole_compare(uint64_t left, uint64_t right) { //helper for fixedpoint_compare
  uint64_t difference = left^right;
  difference |= difference >> 1;
  difference |= difference >> 2;
  difference |= difference >> 4;
  difference |= difference >> 8;
  difference |= difference >> 16;
  difference |= difference >> 32;
  difference ^= difference >> 1;
  //checks if the most significant digit is in left
  if (left&difference) {
    return 1;
  }
  return -1;
}

int fixedpoint_is_zero(Fixedpoint val) {
  return ((val.whole_part == 0) && (val.frac_part == 0));
}

int fixedpoint_is_err(Fixedpoint val) {
  if (val.tags == error) return 1;
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.tags == vneg) return 1;
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.tags == negover) return 1;
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.tags == posover) return 1;
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.tags == negunder) return 1;
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  if (val.tags == posunder) return 1;
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  if ((val.tags == vnon) || (val.tags == vneg)) return 1;
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

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
  // TODO: implement
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
  // TODO: implement
  assert(0);
  return 0;
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

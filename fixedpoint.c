#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
//this is a test
static Fixedpoint DUMMY;
// ask about how the values will be entered as negatives for the other functions
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
  Fixedpoint result = fixedpoint_create2(0,0);
  uint64_t og_leftwhole = left.whole_part;
  if ((left.whole_part == 0) &&
      (left.frac_part == 0) &&
      (right.whole_part == 0) &&
      (right.frac_part == 0)) { //both left and right are 0
      return result;
    }
  //if they are the same sign
  if (left.tags == right.tags) {
    result.frac_part = left.frac_part + right.frac_part;
    if (left.tags == vnon) { //for non-negative values
      result.tags = ((result.frac_part < left.frac_part) || (result.frac_part < left.frac_part)) ? posover : left.tags; 
    } else { //for negative values
      result.tags = ((result.frac_part < left.frac_part) || (result.frac_part < left.frac_part)) ? negover : left.tags;
    }
    //need to see if 1 needs to be carried
    if ((result.tags == posover) || (result.tags == negover)) { //need to carry the one
      //***is this the corect way to "carry" the 1?
      left.whole_part += (0x0000000000000001);
      if (left.whole_part < og_leftwhole) result.tags = result.tags; //compare to both right and left
      result.whole_part = left.whole_part + right.whole_part;
    } else {
      result.whole_part = left.whole_part + right.whole_part;
    }
    if (left.tags == vnon) { //for non-negative values, check whether overflow occured in whole (again)
      result.tags = ((result.whole_part < left.whole_part) || (result.whole_part < right.whole_part)) ? posover : left.tags;
    } else { //for negative values, check whether overflow occured
      result.tags = ((result.whole_part < left.whole_part) || (result.whole_part < right.whole_part)) ? negover : left.tags;
    }
  }

    //Need to check for +- and -+
  if (((left.tags == vnon) && (right.tags == vneg)) ||
      ((left.tags == vneg) && (right.tags == vnon))) {
    return mag_sub(left, right);
  }
  return result;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint result = fixedpoint_create2(0, 0);
  if ((left.whole_part == 0) && 
      (left.frac_part == 0) && 
      (right.whole_part == 0) && 
      (right.frac_part == 0)) { //both left and right are 0
    return result;
  }
  if ((left.tags == vnon) && (right.tags == vneg)) { //this is for the case left - (-right)
    right = fixedpoint_negate(right);
    return fixedpoint_add(left, right); // = left + right
  } else if ((left.tags == vneg) && (right.tags == vneg)) { // this is for -left - (-right) = -left + right
    right = fixedpoint_negate(right);
    return mag_sub(left, right);
  } else if ((left.tags = vnon) && (right.tags == vnon)) { //this is for left - right
    right = fixedpoint_negate(right);
    return mag_sub(left, right);
  } else { // this if for -left - (+right) = -left + (-right)
    right = fixedpoint_negate(right);
    return fixedpoint_add(left, right);
  }
  return result;
}

Fixedpoint mag_sub(Fixedpoint left, Fixedpoint right) { //signs always differ
  Fixedpoint result = fixedpoint_create2(0, 0);
  int sign_tag = 0;
  //get the sign of the fixedpoint with the higher magnitude
  if (left.whole_part > right.whole_part) {
    sign_tag = left.tags;
    result.whole_part = left.whole_part - right.whole_part;
    result.frac_part = left.frac_part - right.frac_part;
    if ((result.frac_part > left.frac_part) || (result.frac_part > right.frac_part)) {
      result.whole_part = result.whole_part - (0x0000000000000001);
    }
  } else if (right.whole_part > left.whole_part) {
    sign_tag = right.tags;
    result.whole_part = right.whole_part - left.whole_part;
    result.frac_part = right.frac_part - left.frac_part;
    if ((result.frac_part > right.frac_part) || (result.frac_part > left.frac_part)) {
      result.whole_part = result.whole_part - (0x0000000000000001);
    }
  } else if (left.frac_part > right.frac_part) {
    sign_tag = left.tags;
    result.frac_part = left.frac_part - right.frac_part;
  } else if (right.frac_part > left.frac_part) {
    sign_tag = right.tags;
    result.frac_part = right.frac_part - left.frac_part;
  }
  result.tags = sign_tag;
  return result;
}

  Fixedpoint fixedpoint_negate(Fixedpoint val)
  {
    if (fixedpoint_is_zero(val))
    {
      val.tags = vnon;
    } else if (val.tags == vnon) {
      val.tags = vneg;
  } else if (val.tags == vneg) {
      val.tags = vnon;
  }
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // Try to use bitwise right shift operator
  if (val.whole_part % 2) { //check to see if whole_part is even
    val.whole_part = val.whole_part>>1;
    if (!(val.frac_part % 2) && (val.tags == vnon)) {
      val.tags = posunder;
    }
    else if (!(val.frac_part % 2) && (val.tags == vneg)) {
      val.tags = negunder;
    }
    val.frac_part = val.frac_part >> 1; //10000000000
  } else { //whole_part is odd and so we need to add something to the frac part?
    val.whole_part = val.whole_part >> 1;
    val.frac_part = val.frac_part + 0x1000000000000000;
    if (!(val.frac_part % 2) && (val.tags == vnon)) {
      val.tags = posunder;
    }
    else if (!(val.frac_part % 2) && (val.tags == vneg)) {
      val.tags = negunder;
    }
    val.frac_part = val.frac_part >> 1;
  }
return val;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  uint64_t ogWhole = val.whole_part;
  uint64_t ogFrac = val.frac_part;
  val.whole_part = val.whole_part << 1;
  val.frac_part = val.frac_part << 1;
  // idea is that overflow would cause value to wrap around on the number line and be a smaller value than original
  int wholeOverflow = val.whole_part < ogWhole; //int as a boolean
  int fracOverflow = val.frac_part < ogFrac;
  if (wholeOverflow || fracOverflow) {
    val.tags = posover;
  }
  return val;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) { 
  //if they differ in signs (-+) or (+-)
  if ((left.tags == vneg) && (right.tags == vnon)) {
    return -1; //left < right (- < +)
  } else if ((left.tags == vnon) && (right.tags == vneg)) {
    return 1; //left > right (+ > -)
  }
  //if they are both non-negative
  if ((left.tags == vnon) && (right.tags == vnon)) {
    if (left.whole_part < right.whole_part) {
      return -1; //left < right
    } else if (left.whole_part > right.whole_part) {
      return 1; //left > right
    } else { //the two whole parts are equal and the frac parts need to be compared
      if (left.frac_part < right.frac_part) {
        return -1; //left < right
      } else if (left.frac_part > right.frac_part) {
        return 1; //left > right
      }
    }
  }
  //if they are both negative
  if ((left.tags == vneg) && (right.tags == vneg)) {
    if (left.whole_part < right.whole_part) {
      return 1; //left > right
    } else if (left.whole_part > right.whole_part) {
      return -1; //left < right
    } else { //the two whole parts are equal and the frac parts need to be compared
      if (left.frac_part < right.frac_part) {
        return 1; //left > right
      } else if (left.frac_part > right.frac_part) {
        return -1; //left < right
      }
    }
  }
  //they are of the same value
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

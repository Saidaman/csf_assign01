/*
 * Implementation of functions useful for a fixedpoint arithmetic program.
 * CSF Assignment 1
 * Shayan Hossain, Sai Earla
 * shossa11@jhu.edu, searla1@jhu.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

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

int valid_hex(const char *hex) {
   uint64_t points = 0; // number of decimal points
   uint64_t valid_chars_in_each_half[2] = {0, 0}; // number of valid hex characters in each half of string
   int which_half = 0; // indicator of which half of string (before/after decimal point)
   uint64_t length = strlen(hex); // length of string
   int valid = 1; // return variable;
   
   for (uint64_t i = 0; i < length; i++) {
      uint64_t is_valid_char = (hex[i] >= '0' && hex[i] <= '9') || (hex[i] >= 'a' && hex[i] <= 'f') || (hex[i] >= 'A' && hex[i] <= 'F');
      if (hex[i] == '.') {
         points++;
         which_half = 1; // switches to second half valid hex char count
      }
      if (!(is_valid_char || hex[i] == '-' || hex[i] == '.')) {
         valid = 0; // no longer a valid hex string
      }
      if (is_valid_char) {
         valid_chars_in_each_half[which_half] += 1; // counting number of valid hex characters in given half of string
      }
   }
   if (valid_chars_in_each_half[0] > 16 || valid_chars_in_each_half[1] > 16 || points > 1) {
      valid = 0; // no longer a valid hex string
   }
   
   return valid;
}

Fixedpoint create_fixedpoint_in_hex(uint64_t points, uint64_t point_index, int negated, const char *hex) {
   uint64_t whole = 0;
   uint64_t frac = 0;
   
   if (points == 0) { // No need to worry about a frac part
      whole = strtoull(hex, NULL, 16);
      frac = 0;
   } else { // Need to worry about frac part
      whole = strtoull(hex, NULL, 16);
   
      char frac_string[17];
      strcpy(frac_string, hex + point_index + 1 - negated); // pointer arithmetic to copy chars over
      uint64_t frac_length = strlen(frac_string); // length after pointer arithmetic
      uint64_t padding = 16 - frac_length; // padding count
      char zero_padding[17];
      memset(zero_padding, '0', padding);
      zero_padding[padding] = '\0';
      char * padded_frac = strcat(frac_string, zero_padding);
      frac = strtoull(padded_frac, NULL, 16);
   }
   
   Fixedpoint result = fixedpoint_create2(whole, frac);
   if (negated) {
      result.tags = vneg;
   } else {
      result.tags = vnon;   
   }

   return result;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  if (!valid_hex(hex)) {
      Fixedpoint error_fpv = fixedpoint_create2(0, 0);
      error_fpv.tags = error;
      return error_fpv; // return error-tagged Fixedpoint value
  }

  uint64_t points = 0; // decimal point(s) count
  uint64_t point_index = 0; // decimal point location
  uint64_t length = strlen(hex);
  int negated = 0; // Indicator variable for whether hex is signed

  for (uint64_t i = 0; i < length; i++) {
    if (hex[i] == '.') {
      points++;
      point_index = i;
    }
  }
  
  if (hex[0] == '-') {
    negated = 1;
    hex++; // pointer arithmetic to look over sign
  }

   return create_fixedpoint_in_hex(points, point_index, negated, hex);
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole_part;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac_part;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint result = fixedpoint_create2(0,0);
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
      result.tags = ((result.frac_part < left.frac_part) || (result.frac_part < right.frac_part)) ? 7 : left.tags; 
    } else { //for negative values
      result.tags = ((result.frac_part < left.frac_part) || (result.frac_part < right.frac_part)) ? 7 : left.tags;
    }
    int fraction_overflow = (result.tags == 7) ? 1 : 0;
    //need to see if 1 needs to be carried
    if (result.tags == 7) { //need to carry the one
      //***is this the corect way to "carry" the 1?
      result.whole_part = left.whole_part + right.whole_part;
      //if ((result.whole_part < left.whole_part) ) result.tags = result.tags; //compare to both right and left
    } else {
      result.whole_part = left.whole_part + right.whole_part;
    }
    //check for overflow
    if (left.tags == vnon) { //for non-negative values, check whether overflow occured in whole (again)
      result.tags = ((result.whole_part < left.whole_part) || (result.whole_part < right.whole_part)) ? posover : left.tags;
    } else { //for negative values, check whether overflow occured
      result.tags = ((result.whole_part < left.whole_part) || (result.whole_part < right.whole_part)) ? negover : left.tags;
    }
    if (result.whole_part == 0xFFFFFFFFFFFFFFFFUL && fraction_overflow) {
      result.tags = left.tags == vnon ? posover : negover; 
    } else if (result.whole_part != 0xFFFFFFFFFFFFFFFFUL && fraction_overflow) {
      result.whole_part++;
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

  if ((left.tags == vneg) && (right.tags == vneg)) { // this is for -left - (-right) = -left + right
    right = fixedpoint_negate(right);
    return mag_sub(left, right);
  } else if ((left.tags == vnon) && (right.tags == vnon)) { //this is for left - right
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
    if ((result.frac_part > left.frac_part)) { //checking for "carry"
      result.whole_part = result.whole_part - (0x0000000000000001);
    }
  } else if (right.whole_part > left.whole_part) {
    sign_tag = right.tags;
    result.whole_part = right.whole_part - left.whole_part;
    result.frac_part = right.frac_part - left.frac_part;
    if ((result.frac_part > right.frac_part)) {
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
  if (val.frac_part & 1){
     if (val.tags == vneg) {
      val.tags = negunder;
    } else {
      val.tags = posunder;
    }
  }

  if (val.whole_part&1) { //if odd
    val.whole_part = val.whole_part >> 1;
    val.frac_part = val.frac_part >> 1;
    val.frac_part = val.frac_part + 0x8000000000000000;
  } else { //if even
    val.whole_part = val.whole_part >> 1;
    val.frac_part = val.frac_part >> 1;
  }
  return val;
  }

Fixedpoint fixedpoint_double(Fixedpoint val) {
  if (val.whole_part>>63) {
    if (val.tags == vnon) val.tags = posover;
    if (val.tags == vneg) val.tags = negover;
    return val;
  }
  //need to make sure to correctly show overflow in add
  return fixedpoint_add(val,val);
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
  if (val.tags == error) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.tags == vneg) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.tags == negover) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.tags == posover) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.tags == negunder) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  if (val.tags == posunder) {
    return 1;
  }
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  if ((val.tags == vnon) || (val.tags == vneg)) {
    return 1;
  }
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  //string for the Fixedpoing as string output
  char *s = malloc(35); //32 bits for digits, 1 for sign, 1 for decimal, 1 for \0
  //check to see if val is negative
  if (fixedpoint_is_neg(val)) {
    s[0] = '-';
  }
  //the frac part is 0
  if (val.frac_part == 0) {
    if (fixedpoint_is_neg(val)) { //'-' takes first index
      sprintf(s + 1, "%llx", val.whole_part);
    } else {
      sprintf(s, "%llx", val.whole_part);
    }
  } else {
    if (fixedpoint_is_neg(val)) {
      sprintf(s + 1, "%llx.%016llx", val.whole_part, val.frac_part);
    } else {
      sprintf(s, "%llx.%016llx", val.whole_part, val.frac_part);
    }
    //to remove 0 padding on the right
    size_t s_size =  strlen(s) - 1;
    char *null_terminator = s + s_size;
    while(*null_terminator == '0') {
      *null_terminator = '\0';
      null_terminator--;
    }
  }
  // char result[35];
  // for (int i = 0; i < 35; i++) {
  //   result[i] = s[i];
  // }
  // free(s);
  return s;
}

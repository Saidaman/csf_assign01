#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint max;

  // TODO: add more objects to the test fixture
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_err(TestObjs *objs);

    // TODO: add more test functions
void test_compare(TestObjs *objs);
void test_halve(TestObjs *objs);

int main(int argc, char **argv)
{
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_frac_part);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_is_overflow_pos);
  TEST(test_is_err);
  

  // IMPORTANT: if you add additional test functions (which you should!),
  // make sure they are included here.  E.g., if you add a test function
  // "my_awesome_tests", you should add
  //
  //   TEST(my_awesome_tests);
  //
  // here. This ensures that your test function will actually be executed.
  TEST(test_compare);
  TEST(test_halve);

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL); //this was already there?

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");

  ASSERT(fixedpoint_is_valid(val1));

  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));

  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));

  // all of the other values should have become negative when negated
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, sum;

  // lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  // rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  // sum = fixedpoint_add(lhs, rhs);
  //ASSERT(fixedpoint_is_neg(sum));
  //ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
  //ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));

  lhs = fixedpoint_create(0x659UL);
  rhs = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  rhs = fixedpoint_negate(rhs);
  sum = fixedpoint_add(lhs, rhs);
  printf("%llu\n", sum.whole_part);
  printf("%llu\n", sum.frac_part);
  ASSERT(0xFCBED7CUL == sum.whole_part);
  // FCBED7CUL
  // 00004D1A23C24FAFUL

}

void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, diff;

  //edge case: 0 - 0, also tests is_zero
  lhs = fixedpoint_create(0x0UL);
  rhs = fixedpoint_create(0x0UL);
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_zero(diff));
  ASSERT(fixedpoint_compare(diff, objs->zero) == 0);

  //edge case max - 0 //test case + +
  lhs = objs->max;
  rhs = fixedpoint_create(0x0UL);
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_valid(diff));
  ASSERT(fixedpoint_compare(diff, objs->max) == 0);

  //test case - + (-1 + max)
  lhs = objs->one;
  lhs = fixedpoint_negate(objs->one);
  rhs = objs->max;
  diff = fixedpoint_sub(lhs, rhs);
  Fixedpoint expected = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL - 0x1, 0xFFFFFFFFFFFFFFFFUL);
  ASSERT(fixedpoint_is_valid(diff));
  ASSERT(fixedpoint_compare(diff, expected) == 0);

  //test case - - (-1 - (-1))
  lhs = fixedpoint_negate(objs->one);
  rhs = fixedpoint_negate(objs->one);
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_compare(diff, objs->zero) == 0);

  //test case + + (+1 - (+ 1))
  lhs = (objs->one);
  rhs = (objs->one);
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_compare(diff, objs->zero) == 0);

  //edge case 1 - 1
  lhs = objs->one;
  rhs = objs->one;
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_valid(diff));
  ASSERT(fixedpoint_compare(diff, objs->zero) == 0);

  //-b0fc7e0.e993 - caf18a60.2c70 = -d6015241.1603 (random case) - +
  lhs = fixedpoint_create_from_hex("-b0fc7e0.e993");
  rhs = fixedpoint_create_from_hex("-caf18a60.2c70");
  diff = fixedpoint_create_from_hex("-d6015241.1603");
  ASSERT(fixedpoint_compare(fixedpoint_sub(lhs, rhs), diff) == 0);

  //9c593ed1f34d.8938ecf70cd - 6861ca8a2.70f12f2e3e0b902 = 9c52b8b54aab.1847bdc8cec46fe (random case) + +
  lhs = fixedpoint_create_from_hex("9c593ed1f34d.8938ecf70cd");
  rhs = fixedpoint_create_from_hex("6861ca8a2.70f12f2e3e0b902");
  diff = fixedpoint_create_from_hex("9c52b8b54aab.1847bdc8cec46fe");
  ASSERT(fixedpoint_compare(fixedpoint_sub(lhs, rhs), diff) == 0);

  //-b6e9bcaa6.405ab60434a - -f32cd8bf.9d9bf03380008 = -a7b6ef1e6.a2bec5d0b49f8 (random case) - -
  lhs = fixedpoint_create_from_hex("-b6e9bcaa6.405ab60434a");
  rhs = fixedpoint_create_from_hex("-f32cd8bf.9d9bf03380008");
  diff = fixedpoint_create_from_hex("-a7b6ef1e6.a2bec5d0b49f8");
  ASSERT(fixedpoint_compare(fixedpoint_sub(lhs, rhs), diff) == 0);

  // 0x659UL - 0xf75UL = 0x15ceUL (random case)
  lhs = fixedpoint_negate(fixedpoint_create(0x659UL));
  rhs = fixedpoint_create(0xf75UL);
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0x15ceUL == fixedpoint_whole_part(diff));
  ASSERT(0x0UL == fixedpoint_frac_part(diff));
}

void test_is_overflow_pos(TestObjs *objs) {
  Fixedpoint sum;

  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(objs->one, objs->max);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  Fixedpoint negative_one = fixedpoint_negate(objs->one);

  sum = fixedpoint_sub(objs->max, negative_one);
  ASSERT(fixedpoint_is_overflow_pos(sum));
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

// TODO: implement more test functions

void test_compare(TestObjs *objs) {
  //check to see if 0 is returned (same Fixedpoint) using create
  Fixedpoint zero_2 = fixedpoint_create(0x0UL);
  ASSERT(fixedpoint_compare(objs->zero, zero_2));

  //check to see if compare returns 0 for the same Fixedpoint with both whole and frac parts
  Fixedpoint one_half_3 = fixedpoint_create2(0, 0x10);
  ASSERT(fixedpoint_compare(objs->one_half, one_half_3));

  //check to see if -1 is returned when left = 1/4 and right = 1/2 (checking frac part)
  ASSERT(fixedpoint_compare(objs->one_fourth, objs->one_half) == -1);

  //check to see if 1 is returned when left = 1/2 and right = 1/4 (checking frac part)
  ASSERT(fixedpoint_compare(objs->one_half, objs->one_fourth) == 1);

  //check to see if comparison in whole parts works as expected (expecting -1)
  Fixedpoint four_and_a_half = fixedpoint_create2(0x0000000000000004UL, 0x8000000000000000UL);
  ASSERT(fixedpoint_compare(objs->one_half, four_and_a_half) == -1);

  //check to see if comparison in whole parts works as expected (expecting 1)
  ASSERT(fixedpoint_compare(four_and_a_half, objs->one_fourth) == 1);
}

void test_halve(TestObjs *objs) {

  //check to see if halving 1/2 produces 1/4 and no underflow
  ASSERT(fixedpoint_halve(objs->one_half).tags == vnon);

  //Testing 4.5 halve, result should be 2.25
  Fixedpoint four_and_a_half = fixedpoint_create2(0x0000000000000004UL, 0x8000000000000000UL);
  Fixedpoint two_and_one_fourth = fixedpoint_create2(0x0000000000000002UL, 0x4000000000000000UL);
  ASSERT(fixedpoint_halve(four_and_a_half).whole_part == two_and_one_fourth.whole_part);
  ASSERT(fixedpoint_halve(four_and_a_half).frac_part == two_and_one_fourth.frac_part);
  ASSERT(fixedpoint_is_valid(fixedpoint_halve(four_and_a_half)));

  //Testing edge case for 0
  Fixedpoint lhs = (objs->zero);
  Fixedpoint rhs = (objs->zero);
  Fixedpoint half = fixedpoint_halve(lhs);
  ASSERT(fixedpoint_compare(fixedpoint_halve(lhs), objs->zero) == 0);
  ASSERT(fixedpoint_is_zero(half));

  //Testing for 1 and 1/2
  lhs = (objs->one);
  rhs = (objs->one_half);
  half = fixedpoint_halve(lhs);
  ASSERT(fixedpoint_compare(half, objs->one_half) == 0);
  ASSERT(fixedpoint_is_valid(half));

  //Testing for -1 and -1/2
  lhs = fixedpoint_negate(objs->one);
  rhs = fixedpoint_negate(objs->one_half);
  half = fixedpoint_halve(lhs);
  ASSERT(fixedpoint_compare(half, rhs) == 0);
  ASSERT(fixedpoint_is_neg(half));

  //test for positive underflow
  lhs = objs->max;
  half = fixedpoint_halve(lhs);
  ASSERT(fixedpoint_is_underflow_pos(half));

  //test for negative underflow
  lhs = fixedpoint_negate(objs->max);
  half = fixedpoint_halve(lhs);
  ASSERT(fixedpoint_is_underflow_neg(half));
}
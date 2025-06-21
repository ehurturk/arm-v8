#include <stdio.h>
#include <stdbool.h>
#include "bits_utils.h"

void test_cond( bool condition, char *testname ) {
  if( condition ) {
    printf( "T %s: OK\n", testname );
  } else {
    printf( "T %s: FAIL\n", testname );
  }
}

void test_int( int got, int expected, char *testname ) {
  if( expected==got ) {
    printf( "T %s == %d: OK\n", testname, expected );
  } else {
    printf( "T %s (expected=%d, got=%d): FAIL\n", testname, expected, got );
  }
}
int main( void ) {
  test_int(extract_bits_u64(2, 0, 0), 0, "Extract [0,0] from 2");
  
  test_int(extract_bits_u32(3, 0, 0), 1, "Extract [0,0] from 3");
  
  test_cond(!check_bit_u64(1, 1), "Check bit 1 from 1 is 0");
  
  test_cond(check_bit_u32(3, 1), "Check bit 1 from 3 is 1");

  u64 x = 7;
  insert_bits_u64(&x, 0, 1, 0);
  test_int(x, 4, "Inserted 0 in [0,1] of 7");
  x = 8;
  insert_bits_u64(&x, 0, 3, 3);
  test_int(x, 3, "Inserted 3 in [0,3] of 8");
  
  x = 8;
  insert_bits_u64(&x, 0, 2, 3);
  test_int(x, 11, "Inserted 3 in [0,2] of 8");

  u32 y = 16;
  insert_bits_u32(&y, 1, 2, 3);
  test_int(y, 22, "Inserted 3 in [1, 2] of 16");
}

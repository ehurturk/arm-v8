#include "../defs.h"
#include "hashmap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_TESTS 100
#define SIZE_OF_TEST 1000

static bool failed = false;

void test_cond( bool condition, char *testname )
{
  if( condition ) {
    printf( "T %s: OK\n", testname );
  } else {
    printf( "T %s: FAIL\n", testname );
    failed = true;
  }
}

void test_u32( u32 got, u32 expected, char *testname )
{
  if( expected==got ) {
    printf( "T %s == %u: OK\n", testname, expected );
  } else {
    printf( "T %s (expected=%u, got=%u): FAIL\n", testname, expected, got );
    failed = true;
  }
}

int main( void ) {
  symbol_table_ptr_t table = create_table_ADT();

  test_u32(get_label_address(table, "a"), UINT32_MAX, "!get(a)");
  test_u32(get_label_address(table, "d"), UINT32_MAX, "!get(d)");
  
  put_label(table, "a", 1, true);
  put_label(table, "b", 1, true);
  put_label(table, "c", 2, true);
  put_label(table, "d", 1, false);
  put_label(table, "d", 10, false);
  put_label(table, "b", 5, true);

  test_u32(get_label_address(table, "a"), 1, "get(a)");
  test_u32(get_label_address(table, "b"), 5, "get(b)");
  test_u32(get_label_address(table, "c"), 2, "get(c)");
  test_u32(get_label_address(table, "d"), 1, "get(d)");

  free_table(table);

  for(int i = 0; i < NUMBER_OF_TESTS; i++) {
    symbol_table_ptr_t t = create_table_ADT();
    for(int j = 0; j < 100; j++) {
      char str[30], test_name[30];
      sprintf(str, "Label %d", j);
      sprintf(test_name, "Doesn\'t have label %d", j);
      test_u32(get_label_address(t, str), UINT32_MAX, test_name);
    }

    char str[SIZE_OF_TEST][30];
    char **entries = malloc(SIZE_OF_TEST * sizeof(*entries));
    for(int j = 0; j <SIZE_OF_TEST; j++) {
      sprintf(str[j], "Label %d", j);
      entries[j] = put_label(t, str[j], (u32)j, j % 2 == 1);
    }

    for(int j = 0; j <SIZE_OF_TEST; j++) {
      entries[j] = put_label(t, str[j], (u32)j+1, false);
    }

    for(int j = 0; j < SIZE_OF_TEST; j++) {
      // char str[30], test_name[30];
      char test_name[30];
      // sprintf(str, "Label %d", j);
      sprintf(test_name, "Has label %d", j);
      test_u32(get_label_address(t, entries[j]), j, test_name);
    }

    for(int j = 0; j <SIZE_OF_TEST; j++) {
      entries[j] = put_label(t, str[j], (u32)j+1, true);
    }

    for(int j = 0; j < SIZE_OF_TEST; j++) {
      // char str[30], test_name[30];
      char test_name[30];
      // sprintf(str, "Label %d", j);
      sprintf(test_name, "Has label %d", j);
      test_u32(get_label_address(t, entries[j]), j+1, test_name);
    }
    free(entries);
    free_table(t);
  }
  if (failed) {
    printf("At least a test has failed!!!!!!!!\n");
  }
  return 0;
}

#include "ini_parser.h"

#include <string.h>

const char *type_to_str(int type)
{
  if(type == INI_SECTION) {
    return "INI_SECTION";
  } else if(type == INI_VALUE) {
    return "INI_VALUE";
  } else {
    return "UNKNOWN";
  }
}

int test_line(int i, const char *str, int exp_type, const char *exp_key, const char *exp_value)
{
  char key[512];
  char value[512];
  int type = parse_ini_str(str, &key[0], sizeof key, &value[0], sizeof value);
  /* printf("k,v = %s,%s\n", key, value); */

  int failed = 0;
  failed |= type != exp_type;
  failed |= strcmp(exp_key, key) != 0;
  failed |= strcmp(exp_value, value) != 0;

  if(failed) {
    fprintf(stderr,
      "TEST %i FAILED: '%s'\n"
      "\tExpected:\n"
      "\t\t TYPE: %s\n"
      "\t\t  KEY: '%s'\n"
      "\t\tVALUE: '%s'\n"
      "\tReceived:\n"
      "\t\t TYPE: %s\n"
      "\t\t  KEY: '%s'\n"
      "\t\tVALUE: '%s'\n",
      i+1, str,
      type_to_str(exp_type), exp_key, exp_value,
      type_to_str(type), key, value);
    return 0;
  } else {
    fprintf(stdout, "TEST %i PASSED: |%s| -> %s ('%s','%s')\n",
      i+1, str, type_to_str(type), key, value);
  }

  return 1;
}

struct test_case {
  const char *input;
  int type;
  const char *key;
  const char *value;
};

struct test_case tests[] = {
  {"key=value", INI_VALUE, "key", "value"},
  {"key= value", INI_VALUE, "key", "value"},
  {"key =value", INI_VALUE, "key", "value"},
  {"key = value", INI_VALUE, "key", "value"},
  {"key = 'value'", INI_VALUE, "key", "value"},
  {"key = \"value\"", INI_VALUE, "key", "value"},
  {"key='value'", INI_VALUE, "key", "value"},
  {"key=\"value\"", INI_VALUE, "key", "value"},
  {NULL, NULL, NULL, NULL}
};

int main()
{
  int passed = 0, failed = 0;

  for(size_t i = 0; tests[i].input; ++i) {
    int ok = test_line(i, tests[i].input, tests[i].type, tests[i].key, tests[i].value);
    if(ok) {
      passed += 1;
    } else {
      failed += 1;
    }
  }

  fprintf(stdout, "%i tests run\n%i tests passed\n%i tests failed\n",
    passed+failed, passed, failed);

  return failed == 0;
}

#include "ini_parser.h"

#include <string.h>

const char *type_to_str(int type)
{
  if(type == INI_SECTION) {
    return "INI_SECTION";
  } else if(type == INI_VALUE) {
    return "INI_VALUE";
  } else if(type == INI_UNKNOWN) {
    return "INI_UNKNOWN";
  } else {
    return "INVALID VALUE";
  }
}

int test_line(int i, const char *str, int exp_type, const char *exp_key, const char *exp_value)
{
  char key[512];
  char value[512];
  int type = parse_ini_str(str, &key[0], sizeof key, &value[0], sizeof value);

  int failed = 0;
  failed |= type != exp_type;
  failed |= strcmp(exp_key, key) != 0;
  failed |= strcmp(exp_value, value) != 0;

  if(failed) {
    fprintf(stderr,
      "TEST %i FAILED: |%s|\n"
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
  /* test basic key value pairs */
  {"key=value", INI_VALUE, "key", "value"},
  {"key= value", INI_VALUE, "key", "value"},
  {"key =value", INI_VALUE, "key", "value"},
  {"key = value", INI_VALUE, "key", "value"},
  {"key = 'value'", INI_VALUE, "key", "value"},
  {"key = \"value\"", INI_VALUE, "key", "value"},
  {"key='value'", INI_VALUE, "key", "value"},
  {"key=\"value\"", INI_VALUE, "key", "value"},
  {"a=\"some long value\"", INI_VALUE, "a", "some long value"},
  {"b=\"weird'quote\"", INI_VALUE, "b", "weird'quote"},
  {"c='weird\"quote'", INI_VALUE, "c", "weird\"quote"},
  {"a=b=c", INI_VALUE, "a", "b=c"},

  /* test sections */
  {"[SectionName]", INI_SECTION, "SectionName", ""},
  {"[User 'test']", INI_SECTION, "User", "test"},
  {"[Section \"test\"]", INI_SECTION, "Section", "test"},

  /* test comments */
  {"; a = b", INI_UNKNOWN, "", ""},
  {"; [section]", INI_UNKNOWN, "", ""},
  {" ; c = d", INI_UNKNOWN, "", ""},
  {" ; [sect]", INI_UNKNOWN, "", ""},

  /* test some invalid input is rejected */
  {"===--=-", INI_UNKNOWN, "", ""},
  {"=a=c", INI_UNKNOWN, "", ""},
  {"b='=''3", INI_UNKNOWN, "", ""},
  {"", INI_UNKNOWN, "", ""},

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

  return failed != 0;
}

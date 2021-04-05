#!/bin/bash

ALL_TESTS_OK=1
RESULT_STRING=""

for TEST_BINARY in `ls bin/*_tests`; do
  RESULT="$TEST_BINARY"
  ./$TEST_BINARY
  if [ $? -eq 0 ]; then
    RESULT="${RESULT}: OK"
  else
    ALL_TESTS_OK=0
    RESULT="${RESULT}: NOT OK"
  fi
  RESULT_STRING+="$RESULT"$'\n'
done
echo $'\n\n'"$RESULT_STRING"
if [ $ALL_TESTS_OK -eq 1 ]; then
  echo "All tests from all test binaries passed."
  exit 0
else
  echo "Some tests failed."
  exit 1
fi

#! /bin/bash

set -e -x


./build/test/test-+
./build/test/test-histogram
#./build/test/test-line-spin
./build/test/test-rnd10000
./build/test/test-rnd10000-bar
./build/test/test-rnd10000-line
./build/test/test-simple
./build/test/test-trig
#./build/test/test-trig-loop
./build/test/test-x

echo "all tests completed"

#!/bin/sh
#
# diagnostic_test: Run a basic diagnostic test of the disksearch program.
# The test involves running disksearch on simple, medium, and large
# disk images and making sure the program generate the correct output.

#TDIR=/usr/class/cs110/samples/assign2/testdisks
TDIR=testdisks
TMP_FILE=`mktemp "/tmp/tp110-XXXXXX"`

echo "This test may take a minute to complete..."

./disksearch -l 0 -q -f $TDIR/simple.dat $TDIR/simple.img > $TMP_FILE
if [ ${?} -ne "0" ]; then
  echo "Running disksearch on simple.dat failed."
  rm -f $TMP_FILE
  exit 1
fi

diff -q $TMP_FILE $TDIR/simple.out
if [ ${?} -ne "0" ]; then
  echo "Compare on run on simple.dat failed."
  rm -f $TMP_FILE
  exit 1
fi

echo "Output for words in simple.dat on simple.img is correct."

./disksearch -l 0 -q -f $TDIR/medium.dat $TDIR/medium.img > $TMP_FILE
if [ ${?} -ne "0" ]; then
  echo "Running disksearch on medium.dat failed."
  rm -f $TMP_FILE
  exit 1
fi


diff -q $TMP_FILE $TDIR/medium.out
if [ ${?} -ne "0" ]; then
  echo "Compare on run on medium.dat failed."
  rm -f $TMP_FILE
  exit 1
fi

echo "Output for words in medium.dat on medium.img is correct."

./disksearch -l 0 -q -f $TDIR/large.dat $TDIR/large.img > $TMP_FILE
if [ ${?} -ne "0" ]; then
  echo "Running disksearch on large.dat failed."
  rm -f $TMP_FILE
  exit 1
fi

diff -q $TMP_FILE $TDIR/large.out
if [ ${?} -ne "0" ]; then
  echo "Compare on run on large.dat failed."
  rm -f $TMP_FILE
  exit 1
fi
rm -f $TMP_FILE

echo "Output for words in large.dat on large.img is correct."
echo "It appears to work!"
exit 0


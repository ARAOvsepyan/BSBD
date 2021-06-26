#!/bin/sh

APP=$1
$APP 1>stdout 2>stderr 
echo $? > exitcode

cat stdout stderr exitcode

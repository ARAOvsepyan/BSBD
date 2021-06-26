#!/bin/sh

if grep "$1" /etc/paswd
then 
echo "The user $1 Exist"
else
echo "Error! The user $1 dosen't exist" 1>&1
exit 1
fi

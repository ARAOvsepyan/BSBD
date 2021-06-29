#!/bin/bash

if [ $# -eq 2 ]
then
    re='^[0-9]+$'
    if ! [[ $1 =~ $re ]] || ! [[ $2 =~ $re ]]
    then
        echo "Wrong Arguments Type">&2;
        exit 1;
    fi

    res=1
    base=$1
    power=$2
    for ((i=0;i<power;i++))
    {
        res=$((res * base))
    }
    echo "$1^$2 = $res">&1
    exit 0
fi

echo "Wrong Arguments Amount">&2
exit 1;
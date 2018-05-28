#!/bin/sh

i=1
while [ $i -le 10 ]
do
    ./a.out &
    let i++
done

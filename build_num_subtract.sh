#!/bin/bash
num=`cat ./build.num`

let num=num-1

echo $num > ./build.num
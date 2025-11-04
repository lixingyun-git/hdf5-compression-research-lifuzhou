#!/bin/bash

bash test.sh > test.log

less test.log | grep cost | sed 's/compress_leve//g' > result.txt

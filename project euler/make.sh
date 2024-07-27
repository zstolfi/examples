#! /usr/bin/bash
for i in "$@"; do
	echo "Compiling problem $i..."
	g++ -std=c++20 -o solution$i "$(echo $i.*.cc)"
done

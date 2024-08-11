#! /usr/bin/bash
debug=false

for i in "$@"; do
	echo "Compiling problem $i..."
	if [ "$debug" = true ] ; then
		g++ -std=c++20 -g -O0 -o solution$i "$(echo $i.*.cc)"
	else
		g++ -std=c++20 -o solution$i "$(echo $i.*.cc)"
	fi
done

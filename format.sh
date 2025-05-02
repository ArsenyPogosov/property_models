#!/bin/bash

git ls-files | 
while read FILE
do
	if [[ "$FILE" == *.cpp || "$FILE" == *.h ]]
	then
		clang-format -i "$FILE"
	fi
	
	if [ $(basename "$FILE") = "CMakeLists.txt" ]
	then
		cmake-format -i "$FILE"
	fi

	# REMOVE BRICKS!
	if [ -n "$(tail -n 1 "$FILE")" ]
	then
		echo >> "$FILE"
	fi
done


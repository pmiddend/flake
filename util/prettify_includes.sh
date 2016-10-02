#!/bin/bash

if [[ $1 == 'run' ]]; then
	shift
	prettify_includes.py --reserved-prefix flake --reserved-prefix flakelib --reserved-prefix sge --reserved-prefix awl --reserved-prefix mizuiro --reserved-prefix fcppt  "$@" 2>>/tmp/prettify_errors
else
	find flake/src flake/include flakelib/src flakelib/include tests/include tests/src \( -name '*.hpp' -o -name '*.cpp' \) | xargs $0 run
fi

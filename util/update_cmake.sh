#!/bin/sh

set -e -u

update_cmake \
	flakelib/CMakeLists.txt \
	FLAKELIB_FILES \
	flakelib/src \
	flakelib/include/flakelib

update_cmake \
	flake/CMakeLists.txt \
	FLAKE_FILES \
	flake/src \
	flake/include

#!/bin/bash

function die()
{
	exit -1
}

function update_cmake_file()
{
	local cmakefile="$1"

	update_cmake \
		"${cmakefile}" \
		"${@:2}" \
		|| die

	mv "${cmakefile}".new "${cmakefile}" || die
}

update_cmake_file \
	flakelib/CMakeLists.txt \
	FLAKELIB_FILES \
	flakelib/src \
	flakelib/include/flakelib

update_cmake_file \
	flake/CMakeLists.txt \
	FLAKE_FILES \
	flake/src \
	flake/include

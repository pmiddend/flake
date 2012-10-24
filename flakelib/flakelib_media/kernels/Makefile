CL_FILES = $(shell find . -type f -name '*.cl' | sed -e 's|\.cl|.clc|')

.PHONY: all
.PHONY: clean

%.clc: %.cl
	sgeopencl_syntax_checkerexample $< -Werror -cl-nv-verbose -I .

all: $(CL_FILES)

clean:
	rm -rf ~/.nv/ComputeCache

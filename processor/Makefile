CC = g++
CFLAGS = -Wall -std=c++17 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations   \
		 -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy     \
		 -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2     \
		 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith       \
		 -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo           \
		 -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef   \
		 -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-range 			     \
		 -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast 			 \
		 -Wno-varargs -Wstack-protector -Wsuggest-override -Wbounds-attributes-redundant \
		 -Wlong-long -Wopenmp -fcheck-new -fsized-deallocation -fstack-protector 		 \
		 -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-protector  \
		 -fPIE -Werror=vla

LDFLAGS =

BUILD_DIR = build
INCLUDE_DIR = include
LIBS_DIR = ../build/libs

INCLUDES_STACK = ../stack/include
INCLUDES_COMMON = ../common/logger ../common/parse_arguments
SOURCES = main.cpp proc_arg_parser.cpp processor.cpp proc_verify.cpp
OBJECTS = $(addprefix $(BUILD_DIR)/src/, $(SOURCES:%.cpp=%.o))
DEPS = $(OBJECTS:%.o=%.d)

EXECUTABLE = build/proc
CFLAGS += -I$(INCLUDE_DIR) -I$(INCLUDES_STACK) $(addprefix -I, $(INCLUDES_COMMON))
LDFLAGS = -L$(LIBS_DIR) -lstack -lcommon


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $(LDFLAGS) $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o:%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MP -MMD -c $< -o $@

.PHONY: clean

clean:
	@rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)

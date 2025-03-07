PROJECT_NAME = libtransistor
PROJECT_VERSION = 0.1.0

LIB = $(PROJECT_NAME).a

all: $(LIB) libtransistor-tests

include contrib/pastel-base/mk/config.mk

CPPFLAGS += -Isrc -Icontrib/stb -Icontrib/pastel-base/pl_log -Icontrib/doctest $(shell pkg-config --cflags luajit)
LDFLAGS += $(shell pkg-config --libs luajit)

# library

OBJ = \
	src/basic/pos_ds.o \
	src/basic/rect.o \
	src/basic/position.o \
	src/basic/direction.o \
	src/board/board.o \
	src/board/wire.o \
	src/compiler/compiler.o \
	src/compiler/connectedwires.o \
	src/component/componentdb.o \
	src/component/component.o \
	src/component/componentdef.o \
	src/cursor/cursor.o \
	src/sandbox/sandbox.o \
	src/simulation/simulation.o \
	src/transistor-sandbox.o \
	src/component/component_mt.o \
	src/simulation/componentsim.o

$(LIB): $(OBJ)
	ar rcs $@ $^

# tests

TEST_OBJ = \
	tests/position.o \
	tests/pinpositions.o \
	tests/connected_wires.o \
	tests/placement.o \
	tests/compilation.o \
	tests/serialization.o \
	tests/simulation.o \
	tests/wrapper.o \
	tests/custom_ic.o \
	tests/_implementation.o

libtransistor-tests: $(TEST_OBJ) $(LIB)
	$(CXX) -o $@ $^

check: libtransistor-tests
	./$^

# clean

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJ) $(TEST_OBJ) $(CLEANFILES) libtransistor-tests
# Makefile for CS110 Assignment 2

MAKEFLAGS += -j10
PROG = disksearch

ARCHIVE_OBJ = index.o scan.o fileops.o pathstore.o cachemem.o diskimg.o disksim.o debug.o 
ARCHIVE_OBJ += assign1/inode.o assign1/unixfilesystem.o assign1/directory.o
ARCHIVE_OBJ += assign1/pathname.o assign1/chksumfile.o assign1/file.o

ARCHIVE_DEP = $(patsubst %.o,%.d,$(ARCHIVE_OBJ))
ARCHIVE = indexlib.a

PROG_OBJ = disksearch.o
PROG_DEP = $(patsubst %.o,%.d,$(PROG_OBJ))

DEPS = -MMD -MF $(@:.o=.d)

WARNINGS = -W -Wall -Wno-deprecated-declarations -Wno-unused-variable
CFLAGS += -fstack-protector -g $(WARNINGS) $(DEPS) -std=gnu99
LDFLAGS += -g $(WARNINGS)
LIBS += -lssl -lcrypto

TMP_PATH := /usr/bin:$(PATH)
export PATH = $(TMP_PATH)

default:
	$(error Must specify a BUILD: debug, valgrind, gprof, perf, or opt)

debug: CFLAGS += -O0
debug: LDFLAGS += -O0
debug: $(PROG)

valgrind: CFLAGS += -O1
valgrind: LDFLAGS += -O1
valgrind: $(PROG)

gprof: CFLAGS += -O2 -pg
gprof: LDFLAGS += -O2 -pg
gprof: $(PROG)

perf: CFLAGS += -O2 -fno-omit-frame-pointer
perf: LDFLAGS += -O2 -fno-omit-frame-pointer
perf: $(PROG)

opt: CFLAGS += -O2 -fomit-frame-pointer
opt: LDFLAGS += -O2 -fomit-frame-pointer
opt: $(PROG)

$(PROG): $(PROG_OBJ) $(ARCHIVE)
	$(CC) $(LDFLAGS) $(PROG_OBJ) $(ARCHIVE) $(LIBS) -o $@

$(ARCHIVE): $(ARCHIVE_OBJ)
	rm -f $@
	ar rs $@ $^

clean::
	rm -f $(PROG) $(PROG_OBJ) $(PROG_DEP)
	rm -f $(ARCHIVE) $(ARCHIVE_DEP) $(ARCHIVE_OBJ)

spartan:: clean
	rm -f *~
	rm -f assign1/*~

.PHONY: default clean debug valgrind gprof opt

-include $(ARCHIVE_DEP) $(PROG_DEP)


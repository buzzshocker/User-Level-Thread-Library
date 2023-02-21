# Target library
lib := libuthread.a
objs    := queue.o uthread.o preempt.o sem.o
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -MMD

ifneq ($(V),1)
Q = @
endif

all: $(lib)

## TODO: Phase 1
# Dep tracking *must* be below the 'all' rule
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

$(lib): $(objs)
	@echo "CC $@"
	$(Q) ar rcs $(lib) $(objs)

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)

TARGETS=client serverC serverCS serverEE serverM

CC=gcc
CCOPTS=-Wall -Wextra

.PHONY: all clean pristine

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

pristine: clean

%: %.c
	$(CC) $(CCOPTS) -o $@ $<
bin_PROGRAMS = test_routing

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src

test_routing_SOURCES = routing/test_routing.c
test_routing_CFLAGS = -std=c89 -Werror
test_routing_LDADD = -lcunit

test: $(bin_PROGRAMS)
	for t in $(bin_PROGRAMS); do ./$$t; done

.PHONY: test

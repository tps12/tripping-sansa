bin_PROGRAMS = test_routing test_server

AM_CFLAGS = -std=c89 -Werror
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src
LDADD = -lcunit

test_routing_SOURCES = routing/test_routing.c

test_server_SOURCES = server/test_server.c test_helpers.c

test: $(bin_PROGRAMS)
	for t in $(bin_PROGRAMS); do ./$$t; done

.PHONY: test

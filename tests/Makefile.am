bin_PROGRAMS = test_resource test_routing test_result test_server

AM_CFLAGS = -std=c89 -Werror
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src
LDADD = -lcunit

test_resource_SOURCES = resource/test_resource.c

test_routing_SOURCES = routing/test_routing.c

test_result_SOURCES = server/test_result.c
test_server_SOURCES = server/test_server.c test_helpers.c

test: $(bin_PROGRAMS)
	for t in $(bin_PROGRAMS); do ./$$t; done

.PHONY: test

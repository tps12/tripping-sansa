bin_PROGRAMS = bin/test_cookie bin/test_resource bin/test_routing bin/test_result bin/test_server bin/test_mongo_uri

AM_CFLAGS = -std=c89 -Werror
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src
LDADD = -lcunit

bin_test_mongo_uri_SOURCES = mongo/test_uri.c test_helpers.c

bin_test_resource_SOURCES = resource/test_resource.c

bin_test_routing_SOURCES = routing/test_routing.c

bin_test_cookie_SOURCES = server/test_cookie.c

bin_test_result_SOURCES = server/test_result.c

bin_test_server_SOURCES = server/test_server.c test_helpers.c

test: $(bin_PROGRAMS)
	for t in $(bin_PROGRAMS); do ./$$t; done

.PHONY: test

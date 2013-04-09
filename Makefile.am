heroku_app = cervice

bin_PROGRAMS = $(heroku_app)
AM_CPPFLAGS = -I$(top_srcdir)/include

cervice_SOURCES = src/app.c src/adapters/libmicrohttpd.c src/resource/resource.c src/routing/routing.c src/server/result.c src/server/server.c
cervice_CFLAGS = $(DEPS_CFLAGS) -std=c89 -Werror
cervice_LDADD = $(DEPS_LIBS)

revision = $(shell git rev-parse HEAD)
vulcan_dir = _vulcan/$(revision)

$(vulcan_dir):
	mkdir -p $(vulcan_dir)

vulcan: $(vulcan_dir)/bin

$(vulcan_dir)/bin: $(vulcan_dir)
	vulcan build -v -c "./configure --prefix=/app/vendor/$(heroku_app) PKG_CONFIG_PATH=vendor/lib/pkgconfig && make install" && tar xvzf /tmp/$(heroku_app).tgz -C $(vulcan_dir)

$(vulcan_dir)/lib: $(vulcan_dir)
	mkdir -p $(vulcan_dir)/lib && cp -r vendor/lib/* $(vulcan_dir)/lib

procfile: $(vulcan_dir)/Procfile

$(vulcan_dir)/Procfile: $(vulcan_dir)
	echo 'web: /app/bin/$(heroku_app) $$PORT' > $(vulcan_dir)/Procfile

deploy_repo: $(vulcan_dir)/.git

$(vulcan_dir)/.git: $(vulcan_dir)/Procfile $(vulcan_dir)/bin $(vulcan_dir)/lib
	cd $(vulcan_dir) && git init && git add Procfile bin/ lib/ && git commit -m 'deploy $(revision)'

deploy: $(vulcan_dir)/.git
	cd $(vulcan_dir) && git push -f git@heroku.com:$(HEROKU_APP_NAME).git master

test:
	cd tests && make test

.PHONY: deploy test

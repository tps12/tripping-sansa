# Deploying to Heroku

    $ ./configure HEROKU_APP_NAME=sleepy-hollow-31337
    $ make deploy

This builds the main app using the dependencies in vendor/, putting the resulting binaries in \_vendor/ under a directory named for the git commit hash.

This is then initialized as a new git repo and force-pushed to the heroku app specified in the configure step.

The targeted heroku remote must have already been set up with

    $ heroku config:add BUILDPACK_URL=https://github.com/ryandotsmith/null-buildpack
    $ heroku config:add LD_LIBRARY_PATH=/app/lib

and everything should work.

## To Build
```
To build this package, do the following steps:

    1. ./bootstrap.sh
    2. ./configure ${CONFIGURE_FLAGS} --prefix=/usr
    3. make

To full clean the repository again run `./bootstrap.sh clean`.
```

```
For local build, pass --prefix=/usr option to the configure script
to let the Makefile use /usr/share value over /usr/local/share for ${datadir}
variable. The error yaml files and elog parser are stored in /usr/share
location in the SDK.
```

```
For CI build, ${datadir} is expanded to /usr/local/share to which the
dependent files and parser are exported, so don't add the prefix option.
```

```
For recipe build, ${datadir} is expanded to /usr/share to which the
dependent files and parser are exported, so don't add the prefix option.
```

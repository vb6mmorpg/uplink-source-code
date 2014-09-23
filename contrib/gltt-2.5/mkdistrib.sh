#!/bin/sh -ex

source VERSION

make distclean || true

cd ..

V=${VERSION_MAJOR}.${VERSION_MINOR}
F=gltt-$V

rm -f $F.tar $F.tar.gz $F.zip

tar cfv $F.tar $F/
gzip -9 $F.tar

zip -r -9 $F.zip $F/
ls -l $F.tar.gz $F.zip

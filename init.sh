PYTHON=

if [ ! -d udis86 ] || [ ! -f udis86/autogen.sh ]; then
	echo -e "Error: the udis86 directory doesn't exist.\nDid you clone this project with --recursive?" 1>&2
	exit 1
fi

cd udis86
./autogen.sh || exit 1
mkdir out -p

CONFIGURE_OPTS=--prefix=`pwd`/out
if [ " $PYTHON" != " " ]; then
	CONFIGURE_OPTS+=" --with-python=$PYTHON"
fi
./configure $CONFIGURE_OPTS || exit 1
make || exit 1
make install
cp out/bin/libudis86-0.dll ..

cd ..
make

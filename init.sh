PYTHON=

if [ ! -d udis86 ]; then
	echo -e "Error: the udis86 directory doesn't exist.\nDid you clone this project with --recursive?" 1>&2
	exit 1
fi

cd udis86
./autogen.sh
mkdir out

CONFIGURE_OPTS=--prefix=`pwd`/out
if [ " $PYTHON" -ne " " ]; then
	CONFIGURE_OPTS+=" --with-python=$PYTHON"
fi
./configure $CONFIGURE_OPTS
make
make install

cd ..
make

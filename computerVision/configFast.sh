export INSTALLDIR="/var/tmp"
export COMMONFLAGS="-Wall -Werror -Wextra -Wshadow -O2"

export CPPFLAGS="-I$INSTALLDIR/include"
export LDFLAGS="-L$INSTALLDIR/lib"
export CFLAGS="$COMMONFLAGS"
export CXXFLAGS="$COMMONFLAGS -std=c++11"

./configure --prefix=$INSTALLDIR

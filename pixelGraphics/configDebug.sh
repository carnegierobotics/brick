export INSTALLDIR="/var/tmp"
export COMMONFLAGS="-Wall -Wextra -Werror -Wshadow -g"

export CPPFLAGS="-I$INSTALLDIR/include"
export LDFLAGS="-L$INSTALLDIR/lib"
export CFLAGS="$COMMONFLAGS"
export CXXFLAGS="$COMMONFLAGS -std=c++11"

./configure --prefix=$INSTALLDIR

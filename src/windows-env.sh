CONFIG_GUESS=`./config.guess`

case $CONFIG_GUESS in
  i686-pc-cygwin)
  export PKG_CONFIG_PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig
  export PATH=$PATH:/usr/i686-w64-mingw32/sys-root/mingw/bin/
  echo "32-bit mingw32 build environment configured"
  ;;
  x86_64-pc-cygwin)
  export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig
  export PATH=$PATH:/usr/x86_64-w64-mingw32/sys-root/mingw/bin/
  echo "64-bit mingw32 build environment configured"
	;;
	*)
	echo "Sorry, this script is only required for Microsoft Windows running a Cygwin environment."
  ;;
esac


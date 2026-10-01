SYSTEM_HEADER_PROJECTS="libc kernel"
PROJECTS="libc kernel"

if [ -z "${MAKE:-}" ]; then
  if command -v gmake >/dev/null 2>&1; then
    MAKE=gmake
  else
    MAKE=make
  fi
fi

# macOS commonly exports HOST as the machine hostname.  Only accept it as a
# toolchain target when it looks like an actual cross-compiler triplet.
case "${HOST:-}" in
  *-elf) ;;
  *) HOST=$(./default-host.sh) ;;
esac

export MAKE HOST

export AR=${HOST}-ar
export AS=${HOST}-as
export CC=${HOST}-gcc

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-O2 -g'
export CPPFLAGS=''

# Configure the cross-compiler to use the desired system root.
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi

#! /bin/sh

# checklist for building skeleton
#
# 1. remove project dependencies on Niche Lite and StickOS
# 2. rebuild all
# 3. DEBUG=1, SECURE=0, EXTRACT=1
# 4. extract.sh
# ...
# 10. remove all non-critical files from zip!
# 11. extract zip and build Skeleton's, Flasher

extract()
{
  find . -name \*.c -o -name main.h |
    grep -vEi "nichelite|pict-o-crypt" |
    xargs cat |
    grep "\<MCF_" |
    sed 's!\<MCF_!|MCF_!g' | tr '|' "\012" |
    grep MCF_  | sed 's![^0-9A-Z_].*!!' | sort -u |
    while read d; do
      grep "#define *$d[ (]" headers/*${1}*.h || echo $d not found >&2
    done | sed 's!   *! !g;s!^[^:]*:!!'
}

(
cat <<EOF
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;
typedef signed char int8;
typedef signed short int int16;
typedef signed long int int32;
typedef volatile uint8 vuint8;
typedef volatile uint16 vuint16;
typedef volatile uint32 vuint32;
#if ! GCC
#pragma define_section system ".system" far_absolute RW
extern __declspec(system) uint8 __IPSBAR[];
extern __declspec(system) uint8 __RAMBAR[];
extern __declspec(system) uint8 __RAMBAR_SIZE[];
extern __declspec(system) uint8 __FLASHBAR[];
extern __declspec(system) uint8 __FLASHBAR_SIZE[];
#define IPSBAR_ADDRESS  (uint32)__IPSBAR
#define RAMBAR_ADDRESS  (uint32)__RAMBAR
#define RAMBAR_SIZE  (uint32)__RAMBAR_SIZE
#define FLASHBAR_ADDRESS  (uint32)__FLASHBAR
#define FLASHBAR_SIZE  (uint32)__FLASHBAR_SIZE
#endif
EOF

echo "#if MCF52221"
extract 52221
echo "#endif"

echo "#if MCF52233"
extract 52235
echo "#endif"

echo "#if MCF52259"
extract 52259
echo "#endif"
) >extract.h


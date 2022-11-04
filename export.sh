VER=v1.92c
mkdir -p export

for i in pic32.X/dist/*.hex; do
  FLA=${i#*MX}; FLA=MX${FLA%.*}
  cp $i export/StickOS.PIC32.$FLA.$VER.elf.hex
done

for i in pic32.X/dist/*/*/*.hex; do
  FLA=${i#*/dist/}; FLA=${FLA%%/*}
  cp $i export/StickOS.PIC32.$FLA.$VER.elf.hex
done

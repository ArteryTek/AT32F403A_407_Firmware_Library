#!/bin/bash -
echo "Extract fir_filter symbols"
arm-none-eabi-nm $1 > nm.txt
case "$(uname -s)" in
    Linux*|Darwin*)
      tr -d '\015' <../keep_sym.txt >../keep_sym_unix.txt
      grep -F -f ../keep_sym_unix.txt nm.txt > symbol.list
      rm ../keep_sym_unix.txt
      ;;
    *)
      grep -F -f ../keep_sym.txt nm.txt > symbol.list
      ;;
esac
wc -l symbol.list
cat symbol.list | awk '{split($0,a,/[ \r]/); print a[3]" = 0x"a[1]";"}' > keep_sym_app.ld
rm nm.txt
rm symbol.list

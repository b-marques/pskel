MPPADIR=/usr/local/k1tools

$MPPADIR/bin/k1-jtag-runner --multibinary=output/bin/multibin_bin.mpk --exec-multibin=IODDR0:io_bin -- $1 $2 $3 $4 $5 $6 $7 $8

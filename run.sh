#/bin/sh

make clean && make
./cs_ssa --type=cfg tests/0.cfg
./cs_ssa --type=cfg tests/1.cfg
./cs_ssa --type=cfg tests/2.cfg
./cs_ssa --type=cfg tests/3.cfg
./cs_ssa --type=cfg tests/4.cfg
./cs_ssa --type=cfg tests/5.cfg
./cs_ssa --type=ssa tests/0.ssa

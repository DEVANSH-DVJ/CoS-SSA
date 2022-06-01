#/bin/sh

make clean && make

if [[ -f cs_ssa ]]; then
  # CFG Tests
  ./cs_ssa --type=cfg tests/0.cfg
  ./cs_ssa --type=cfg tests/1.cfg
  ./cs_ssa --type=cfg tests/2.cfg
  ./cs_ssa --type=cfg tests/3.cfg
  ./cs_ssa --type=cfg tests/4.cfg
  ./cs_ssa --type=cfg tests/5.cfg
  ./cs_ssa --type=cfg tests/6.cfg

  # SSA Tests
  ./cs_ssa --type=ssa tests/0.ssa
  ./cs_ssa --type=ssa tests/6.ssa
else
  echo "Compilation Failed!"
fi

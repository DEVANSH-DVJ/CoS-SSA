#/bin/sh

make DEBUG=0

if [[ -f cs_ssa ]]; then
    # CFG Tests
    ./cs_ssa --tool=cfg tests/0.cfg
    ./cs_ssa --tool=cfg tests/1.cfg
    ./cs_ssa --tool=cfg tests/2.cfg
    ./cs_ssa --tool=cfg tests/3.cfg
    ./cs_ssa --tool=cfg tests/4.cfg
    ./cs_ssa --tool=cfg tests/5.cfg
    ./cs_ssa --tool=cfg tests/6.cfg
    ./cs_ssa --tool=cfg tests/7_orig.cfg
    ./cs_ssa --tool=cfg tests/7_dupl.cfg
    ./cs_ssa --tool=cfg tests/8.cfg
    ./cs_ssa --tool=cfg tests/9.cfg
    ./cs_ssa --tool=cfg tests/10.cfg

    # SSA Tests
    ./cs_ssa --tool=ssa tests/0.ssa
    ./cs_ssa --tool=ssa tests/6.ssa
    ./cs_ssa --tool=ssa tests/7_orig.ssa
    ./cs_ssa --tool=ssa tests/7_exp.ssa
else
    echo "Compilation Failed!"
fi

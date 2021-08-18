#!/bin/bash


# hardcode cache params to test
mr=8
nr=16
declare -a MC_ARR=(256 384 512)
declare -a KC_ARR=(896 3328)
declare -a NC_ARR=(2048 3072 4096)


# auto generate
# bigger fractions give to bigger kc
# bigger kc is good for the microkernel
# if we use a big kc for a problem that doesn't require multiple iterations of the packing routines then the microkernel is able to accelerate towards peak performance
# therefore a big MC and NC are also good since it lessens the number of times the packing routines will be called
# however a big MC and NC mean that the packingn routines should blow out the cache
# if i pack B into NC/NR micropanels such that each micropanels is KC x NR
# these micropanels are also packed into column major order
# how can packing be improved?
# jagged-ness for square sizes happens at about 2000
#declare -a frac_arr=(0.875 1 1.5 1.75 2)
declare -a frac_kc_arr=(1 1.25 1.5 1.75 2 2.25 2.5 2.75)
declare -a frac_mc_arr=(0.25 0.5 0.75 1 1.25 1.5 1.75 2)
declare -a frac_nc_arr=(0.25 0.5 0.75 1 1.25 1.5 1.75 2)
dt_size=2
l1_s=$((32*(2**10)))
l2_s=$((2*(2**20)))
l3_s=$((8*(2**20)))
for i in "${!frac_kc_arr[@]}"
do

    kc=$( bc <<< "($l1_s*${frac_kc_arr[$i]})/($nr*$dt_size)")
    mc=$( bc <<< "($l2_s*${frac_mc_arr[$i]})/($kc*$dt_size)")
    nc=$( bc <<< "($l3_s*${frac_nc_arr[$i]})/($kc*$dt_size)")
    mc=$((mc - mc%mr))
    nc=$((nc - nc%nr))
    
    make clean;
    make MACROS="-DMC=$mc -DKC=$kc -DNC=$nc"
    ./test_blocks.x > ./results/${mc}_${kc}_${nc}_fp16gemm    
done

: <<'END'
# this is a hardcoded varison of the sweep
# test a range of sizes that can fit the model
for mc in "${MC_ARR[@]}"
    do
    # ensure they are multiples
    if ((mc % mr)); then
        continue
    fi
    for kc in "${KC_ARR[@]}"
    do
        for nc in "${NC_ARR[@]}"
        do
            # ensure they are multiples
            if ((nc % nr)); then
                continue
            fi
            make clean;
            make MC=$mc KC=$kc NC=$nc
            ./test_blocks.x > ./results/${mc}_${kc}_${nc}_fp16gemm
        done
    done
done
END

make clean;

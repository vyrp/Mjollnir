#!/bin/bash
RUNTIME_DIR=/Mjollnir/yggdrasil/tests/runtime
SANITY_DIR=/Mjollnir/yggdrasil/tests/sanity
RESULTS=/Mjollnir/yggdrasil/tests/results
#RESULTS=/dev/null

red='\e[1;31m'
green='\e[1;32m'
NC='\e[0m'

if [[ ($# -eq  1) && ($1 == "--clean" || $1 == "-c") ]]; then
    echo -e "=== Cleaning ===\n"
    make -f $RUNTIME_DIR/makefile clean MAKEFILE_DIR=$RUNTIME_DIR
    make -f $SANITY_DIR/makefile clean MAKEFILE_DIR=$SANITY_DIR
    echo "rm -f $RESULTS"
    rm -f $RESULTS
    echo ""
    exit 0
fi

echo -e "=== Compilation Tests ===\n"
echo -e "<><>\n"


echo -e "=== Runtime Tests ===\n"
echo -e " * Compiling...\n"
if !(make -f $RUNTIME_DIR/makefile all MAKEFILE_DIR=$RUNTIME_DIR); then
    echo -e "\nERROR => Compilation failed.\n"
    exit 1
fi

echo -e "\n * Running...\n"
echo -e "== RUNTIME TESTS ==" > $RESULTS

passed=0
failed=0
for exe in $(ls $RUNTIME_DIR/bin/*); do
    echo -e "\n$exe\n" >> $RESULTS
    if $exe >> $RESULTS 2>> $RESULTS; then
        echo -e " >> $exe >> ${red}FAILED${NC}"
        let "failed += 1"
    else
        echo -e " >> $exe >> ${green}PASSED${NC}"
        let "passed += 1"
    fi
done

echo -e "\n * $passed tests passed"
echo -e " * $failed tests failed\n"

echo -e "=== Sanity Tests ===\n"
echo -e " * Compiling...\n"
if !(make -f $SANITY_DIR/makefile all MAKEFILE_DIR=$SANITY_DIR); then
    echo -e "\nERROR => Compilation failed.\n"
    exit 1
fi

echo -e "\n * Running...\n"
echo -e "\n== SANITY TESTS ==" >> $RESULTS

passed=0
failed=0
for exe in $(ls $SANITY_DIR/bin/*); do
    echo -e "\n$exe\n" >> $RESULTS
    if $exe >> $RESULTS 2>> $RESULTS; then
        echo -e " >> $exe >> ${green}PASSED${NC}"
        let "passed += 1"
    else
        echo -e " >> $exe >> ${red}FAILED${NC}"
        let "failed += 1"
    fi
done

echo -e "\n * $passed tests passed"
echo -e " * $failed tests failed\n"

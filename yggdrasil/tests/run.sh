#!/bin/sh
RUNTIME_DIR=/Mjollnir/yggdrasil/tests/runtime
RESULTS=$RUNTIME_DIR/../results

if [[ ($# -eq  1) && ($1 == "--clean" || $1 == "-C") ]]; then
    echo -e "=== Cleaning ===\n"
    make -f $RUNTIME_DIR/makefile clean MAKEFILE_DIR=$RUNTIME_DIR
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

echo ""
if [[ ($# -eq 1) && ($1 == "--compile" || $1 == "-c") ]]; then
    exit 0
fi

echo -e " * Running...\n"
echo -e "== TESTS ==" > $RESULTS

passed=0
failed=0
for exe in $(ls $RUNTIME_DIR/bin/*); do
    echo -e "\n$exe\n" >> $RESULTS
    if $exe >> $RESULTS 2>> $RESULTS; then
        echo -e " >> $exe >> PASSED"
        let "passed += 1"
    else
        echo -e " >> $exe >> FAILED"
        let "failed += 1"
    fi
done

echo -e "\n * $passed tests passed"
echo -e " * $failed tests failed\n"

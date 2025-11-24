#change current directory to this file
SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
    cd $SCRIPT_PATH
fi

#~/Downloads/cmake-4.2.0-linux-x86_64/bin/

cmake \
    -G "Ninja" \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
	-B _build/AxEngine-x64-linux \
	.

#change current directory to this file
SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
    cd $SCRIPT_PATH
fi

cmake --build _build/AxEngine-x64-linux
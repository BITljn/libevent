pkill -f myapp

# build libevent in debug mode
sh build_debug.sh
# build my test main cpp
sh build_main.sh
# run myapp
#LD_LIBRARY_PATH=./tmp/lib/ ./myapp

echo "\n\n\n"
ls -ahlt myapp

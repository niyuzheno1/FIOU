rm -r -f ./testCan/
mkdir testCan
cp -r ./test/* ./testCan/
cp FIOU ./testCan/
cd ./testCan
./FIOU > report.txt
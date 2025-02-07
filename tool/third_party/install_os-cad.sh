
#!/bin/bash

if [ $# -eq 0 ]
then
    installPrefix="$(pwd)/"
else
    installPrefix="$1"
fi

wget --no-check-certificate https://github.com/YosysHQ/oss-cad-suite-build/releases/download/2024-12-04/oss-cad-suite-linux-x64-20241204.tgz
tar -xvf oss-cad-suite-linux-x64-20241204.tgz
rm -rf oss-cad-suite-linux-x64-20241204.tgz
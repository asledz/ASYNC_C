#!/bin/bash

function testujmacierz () {
  for i in "/tmp/tmp.7SuB7nBQlt/test/dane/$1"*.txt; do
	  echo $i
	  dn=`dirname $i`
	  bn=`basename $i .txt`
	  echo $dn $bn
    cat $i | "/tmp/tmp.7SuB7nBQlt/cmake-build-debug-remote-host/macierz" > res$bn.txt
    if diff res$bn.txt "$dn/res$bn.txt"; then
       rm res$bn.txt
       continue;
    else
       rm res$bn.txt
       return 0
    fi
  done
  return 1
}

if [ $# -le 0 ]; then
        echo "Powinien być jeden argument"
        exit 1
fi

if [ $1 -ge 1 ]; then
        if testujmacierz stud; then
                exit 1
        fi
fi

if [ $1 -ge 2 ]; then
        if testujmacierz thorough; then
                exit 1
        fi
fi

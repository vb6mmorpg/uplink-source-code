#!/bin/sh

# Function to find the real directory a program resides in.
# Feb. 17, 2000 - Sam Lantinga, Loki Entertainment Software
FindPath()
{
    fullpath="`echo $1 | grep /`"
    if [ "$fullpath" = "" ]; then
        oIFS="$IFS"
        IFS=:
        for path in $PATH
        do if [ -x "$path/$1" ]; then
            if [ "$path" = "" ]; then
                path="."
            fi
            fullpath="$path/$1"
            break
        fi
    done
    IFS="$oIFS"
    fi
    if [ "$fullpath" = "" ]; then
        fullpath="$1"
    fi

    # Is the sed/ls magic portable?
    if [ -L "$fullpath" ]; then
        #fullpath="`ls -l "$fullpath" | awk '{print $11}'`"
        fullpath=`ls -l "$fullpath" |sed -e 's/.* -> //' |sed -e 's/\*//'`
    fi
    dirname $fullpath
}


here="`FindPath $0`"
echo $here
if [ ! -f "$here/lib/patch.dat" ] ; then 
	echo "$0: Fatal error: missing data files (*.dat) in $here/lib"
	echo "If you have just installed the patch to the full version of the game"
	echo "you will need to copy *.dat from your existing uplink installation"
	exit 1
fi
export LD_LIBRARY_PATH="$here/lib:$LD_LIBRARY_PATH"
$here/lib/uplink.bin.x86 "$@"

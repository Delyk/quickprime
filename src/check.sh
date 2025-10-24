#!/bin/bash

MD5_CONTROL="c13929ee9d2aea8f83aa076236079e94"
MD5_TEST=$(./quickprime 1000000 --nostat | md5sum | awk '{print $1}')

if [ "$MD5_TEST" == "$MD5_CONTROL" ]; then
		echo "Program install successful"
else
		echo "Error"
fi


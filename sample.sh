#!/bin/bash
#
declare -i COUNTER
declare -a D=('2' '4' '8' '16' '32' '64' '128' '256' '512');
COUNTER=0
         while [  $COUNTER -lt 9 ]; do
		echo ${D[$COUNTER]} 
		let COUNTER=COUNTER+1 
         done
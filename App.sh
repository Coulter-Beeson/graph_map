#!/bin/bash
#
# This script is used for running main application with different .g files 
# passsed as parameters
#
# Usage:
#chmod +x App.sh
#./App.sh
#


DIR= "$PWD"

GFILES=Gfiles # folder which has all the gfiles
OUTPUT=output #where main will write the file,need to clear it before running 
mkdir -p $OUTPUT #make output dir if it doesn exist
RESULT_FILE="plotdata.txt"


###############################################################################
# clean_up(): Clean up any files created during testing.
###############################################################################
clean_up(){
	#make -s clean
for file in $OUTPUT/*.txt;do
	rm -f $file
done

}


clean_up

###############################################################################
# Run the main prog. app_type for BFT is 1, DFT is 2
# ./bfsp <graph.g> <app_type> <node_num>
# Eg:
# ./bfsp "my_graph.g" 1 467
###############################################################################

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./bfsp "$file" 1 210 
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./bfsp "$file" 2 210
done
echo "###### Done.. exiting ####"


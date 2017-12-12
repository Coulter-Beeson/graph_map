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
make app
###############################################################################
# Run the main prog. app_type for BFT is 1, DFT is 2
# ./app <graph.g> <app_type> <node_num> <output_filename.txt>
# Eg:
# ./app "my_graph.g" 1 467 <plot_dft.txt>
###############################################################################

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 1 210 "$OUTPUT/plots_bfs_bfspart.txt"
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 2 210 "$OUTPUT/plots_dfs_bfspart.txt"
done
echo "###### Done.. exiting ####"


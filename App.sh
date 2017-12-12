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
mkdir -p $GFILES


###############################################################################
# clean_up(): Clean up any files created during testing.
###############################################################################
clean_up(){
	#make -s clean
for file in $OUTPUT/*.txt;do
	rm -f $file
done

for file in $GFILES/*.g;do
	rm -f $file
done
}

##################################################
# call make on all required files
#################################################

make app
make bfpart
make dfpart
make rand


##################################################
# create .g files and put then Gfiles directory
#################################################

#fb- dataset




clean_up
###############################################################################
# Run the main prog. app_type for BFT is 1, DFT is 2
# ./app <graph.g> <app_type> <node_num> <output_filename.txt>
# Eg:
# ./app "my_graph.g" 1 467 <plot_dft.txt>
###############################################################################
declare -i NODE
NODE= "$((1+ RANDOM %4039))"
echo $((1+ RANDOM %4039))

echo $NODE

##################################################
# Run bfs and dfs with bfs_partition
##################################################
for file in $GFILES/*.g;do
	echo "changing to bfs partition: $file " 
	 ./bfpart "$file" $NODE 
done
#bfs
for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 1 $NODE "$OUTPUT/plots_bfs_bfspart.txt"
done

#dfs
for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 2 $NODE "$OUTPUT/plots_dfs_bfspart.txt"
done


##################################################
# Run bfs and dfs with dfs_partition
##################################################
for file in $GFILES/*.g;do
	echo "changing to dfs partition: $file " 
	 ./dfpart "$file" "$NODE"  
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 1 "$NODE" "$OUTPUT/plots_bfs_dfspart.txt"
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 2 "$NODE" "$OUTPUT/plots_dfs_dfspart.txt"
done


##################################################
# Run with random_partition
##################################################
for file in $GFILES/*.g;do
	echo "changing to bfs partition: $file " 
	 ./rand "$file" "$NODE"  
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 1 "$NODE" "$OUTPUT/plots_bfs_randpart.txt"
done

for file in $GFILES/*.g;do
	echo "running $file " 
	 ./app "$file" 2 "$NODE" "$OUTPUT/plots_dfs_randpart.txt"
done

echo "###### Done.. exiting ####"


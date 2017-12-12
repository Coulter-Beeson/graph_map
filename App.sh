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
mkdir -p "$GFILES/fb"
mkdir -p "$OUTPUT/fb"

###############################################################################
# clean_up(): Clean up any files created during testing.
###############################################################################
clean_up(){
	#make -s clean
for file in $OUTPUT/*.txt;do
	rm -f $file
done

for file in "$OUTPUT/fb"/*.txt;do
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
make e2g

clean_up
##################################################
# create .g files and put them in Gfiles directory
#################################################

#fb- dataset
build_fb(){
./e2g edgeLists/fb_new2.txt 3997 85096 346 Gfiles/fb/fb_graph.g
}

#star and complete

declare -i COUNTER
declare -a D=('2' '4' '8' '16' '32' '64' '128' '256' '512');

COUNTER=0
         while [  $COUNTER -lt 9 ]; do
             #D=$((1+ RANDOM %300))
			 ./write ${D[$COUNTER]} 1
			 ./write ${D[$COUNTER]} 2
             let COUNTER=COUNTER+1 
         done


###############################################################################
# Run the main prog. app_type for BFT is 1, DFT is 2
# ./app <graph.g> <app_type> <node_num> <output_filename.txt>
# Eg:
# ./app "my_graph.g" 1 467 <plot_dft.txt>
###############################################################################

#For Star and Complete graphs:
#########################################################################################
		##################################################
		# Run bfs and dfs with bfs_partition
		##################################################
		
		for file in $GFILES/*.g;do
			echo "changing to bfs partition: $file " 
			 ./bfpart "$file" 1
		done
		#bfs

				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 1 1 "$OUTPUT/plots_bfs_bfspart.txt"
				done

				#dfs
				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 2 1 "$OUTPUT/plots_dfs_bfspart.txt"
				done


		##################################################
		# Run bfs and dfs with dfs_partition
		##################################################
		for file in $GFILES/*.g;do
			echo "changing to dfs partition: $file " 
			 ./dfpart "$file" 1  
		done


				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 1 1 "$OUTPUT/plots_bfs_dfspart.txt"
				done

				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 2 1 "$OUTPUT/plots_dfs_dfspart.txt"
				done


		##################################################
		# Run with random_partition
		##################################################
		for file in $GFILES/*.g;do
			echo "changing to bfs partition: $file " 
			 ./rand "$file" 100  
		done


				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 1 1 "$OUTPUT/plots_bfs_randpart.txt"
				done

				for file in $GFILES/*.g;do
					echo "running $file " 
					 ./app "$file" 2 1 "$OUTPUT/plots_dfs_randpart.txt"
				done
#######################################################################################

###################################
# Run fb dataset from different nodes
###################################

declare -i NODE

file="$GFILES/fb/fb_graph.g"

COUNTER=0
while [  $COUNTER -lt 10 ]; do
	NODE=$((1+ RANDOM %1000))
	#with random partition
		build_fb
        ./rand "$file" 100   
		./app "$file" 1 $NODE "$OUTPUT/fb/plots_bfs_randpart.txt"
		./app "$file" 2 $NODE "$OUTPUT/fb/plots_dfs_randpart.txt"
	#with bfs partition
		build_fb
        ./bfpart "$file" $NODE   
		./app "$file" 1 $NODE "$OUTPUT/fb/plots_bfs_bfpart.txt"
		./app "$file" 2 $NODE "$OUTPUT/fb/plots_dfs_bfpart.txt"
	#with dfs partition
		build_fb
        ./dfpart "$file" $NODE   
		./app "$file" 1 $NODE "$OUTPUT/fb/plots_bfs_dfpart.txt"
		./app "$file" 2 $NODE "$OUTPUT/fb/plots_dfs_dfpart.txt"
	let COUNTER=COUNTER+1
done


echo "###### Done.. exiting ####"


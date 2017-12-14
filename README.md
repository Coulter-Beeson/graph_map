# graph_map
Project : Custom graph data-structure implementation to improve locality in graph applications, that uses userfaultfd API for on-demand paging in user space </br>
Authors: Coulter Beeson, Laura Cang, Meghana Venkataswamy </br>
For more details see:  <a href="https://github.com/Coulter-Beeson/graph_map/blob/master/Report%20and%20Presentation/CS508_Project_graphmap_report.pdf">Project Report</a> and class-project <a href="https://github.com/Coulter-Beeson/graph_map/blob/master/Report%20and%20Presentation/CS508%20-%20Graph%20Map_presentation.pdf">Presentation </a>

<h2>Requires: </h2>
<ul>
 <li> A linux system with kernel newer than 2.4 (due to compatibility issues with userfaultfd API) </li>
<li> About 5 Gb of space </li>
<li> gcc to compile the code </li>
</ul>


<h2>Application : </h2>
<p><b>To run </b> the application simply run the bash file <b>App.sh.</b> 
It will compile, generate our custom graph files (with an extention .g) and run the different traversals(i.e an example graph application) .  </p>

<h2> Brief description of importatnt files </h2>
<p>
<ul>
<li> <b>graph.c : </b>  our custom graph API code</li>
<li> <b>traversals.c : </b> Contains code for BFS and DFS traversals using the graph API </li>
<li> <b>app_paging.c ; </b> Contains the main() thread that executes the application and runs a handler thread to handle page-faults</li>
<li> <b> bf_part.c , df_part.c, random_part.c :</b> used to create different graph partitions. For Eg, bf_part will create a graph partition optimal for BFS traversal. </li>
<li> <b>edge_to_graph : </b> used to convert a txt file containing a list of vertex pairs connected by an edge, to our custom graph file. See folder <b>edgeLists</b> which contain the datasets in their raw format. The graph files created are stored in <b>Gfiles</b> folder </li>

</ul>
</p>
<h2> Output </h2>
<p>
The folder output contains the page faults data for different datasets. The file naming scheme followed is <b> plots_<type_of_traversal>_<type_of_partition>.txt </b> </br>
  Subfolder <b>fb</b> contains results from running the facebook dataset  downloaded from <a href="https://snap.stanford.edu/data/egonets-Facebook.html">here</a>. <b>fb_new</b> folder contains the results from running our pruned facebook dataset. The other files in this folder are the results obtained for star and complete datasets
  
</p>





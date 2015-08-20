#ifndef HIER_H_
#define HIER_H_

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/foreach.hpp>
#include "grc/network.h"
#include "grc/network.cpp"

#include <algorithm>
//#include <gsl/gsl_math.h>
//#include <gsl/gsl_eigen.h>
//#include <gsl/gsl_matrix.h>
//#include <gsl/gsl_blas.h>
using namespace std;

namespace hierar{
template<typename G>
inline float  Get_Local_Reaching_Centrality(G& g,typename boost::graph_traits<G>::vertex_descriptor v){
	 using namespace boost;
	 using namespace std;
            typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;
                             
            float  Reaching = 0.0;
            float  maxweight=1.0;//max_weight must be 0.0 for normal experiments0

            std::map<Vertex,int> distance;
            std::map<Vertex,float> max_weight;

         BGL_FORALL_VERTICES_T(v, g, G)
            {
               distance[v] = -1;
               max_weight[v] = 0.0;
            }
       vector<Vertex> current_shell;
       vector<Vertex> next_shell;
      distance[v] = 0;
      max_weight[v] = 0.0;
      Vertex current_node ;
      double current_weight = 0.0;
     
	  // find the max weight in the graph g 
	  // uncomment this for normal experiment 
	  // for no weight experiment we don't need it

      /*BGL_FORALL_EDGES_T(e, g, G)
       {
          float weight = g[e].get_weight().data(0);
            if (abs(weight)> maxweight)
                maxweight=abs(weight);
       }*/

      current_shell.push_back(v);
      do {
            while ( !current_shell.empty() )
            {
              current_node = current_shell.back();
              BGL_FORALL_OUTEDGES_T(current_node,e, g, G)
              {
                 Vertex src = source(e, g), targ = target(e, g);
	         //float w = g[e].get_weight().data(0);// uncomment this for normal experiment 
                 float w = 1.0; //comment this out for normal experiment. It is for without weight hierarchy 
                 current_weight = max_weight[current_node] + (abs(w)/maxweight-max_weight[current_node])/float(distance[current_node]+1);
                 
                  if( distance[targ] == -1 )
                      {
                        next_shell.push_back( targ );
                        distance[targ] = distance[current_node] + 1;
                        max_weight[targ] = current_weight;
                       }
                  else if( distance[targ] == distance[current_node] + 1)
                      {
                       if( current_weight > max_weight[v] )
                         {
                           max_weight[targ] = current_weight;
                         }
                      }
              }
          current_shell.pop_back();
          }

        current_shell.swap( next_shell );
          } while ( !current_shell.empty() );
         // calculate reaching
            BGL_FORALL_VERTICES_T(v, g, G)
            {
              if( distance[v] > 0 )
              {
                Reaching += max_weight[v];
                //printf("%s\t%i\t%lg\n", network->Nodes[i].Label.c_str(), distance[i], max_weight[i]);
              }
            }
             Reaching /= (double)(num_vertices(g)-1);
           // cout<<g[v].Reaching<<endl;
       return Reaching;

 }
template<typename G>
inline float Get_Avg_GRC(G& g){
        float  max_reaching = -1.0;
        float  sum_reaching = 0.0;
        float  weight_mean = 1.0;
        float  grc = 0.0;
        float total_grc = 0.0;
        float reaching_v;
        std::vector<double> lrc;
  NETWORK net;
  std::map<std::pair<std::string,std::string>,float> network;
  std::pair<std::string,std::string> src_trg;
typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;
std::vector<Vertex> outputs;
         // go through network nodes and get local reaching centarlities
        // record maximum reaching and sum of reachings too
//std::cout<<"preparing the reversed network for GRC computation\n";
//you can set the all weights here, for the weight experiment
//int outputs = g.get_outputs().size();
//cout<<"the number of output nodes is :"<<outputs<<"\n";

// first let's find out the outputs
BGL_FORALL_VERTICES_T(v, g, G)
{
	if (g[v].get_out() != -1)
	{
		std::cout<<"output :"<<g[v].get_id()<<" ";
		outputs.push_back(v);
	}
}
std::cout<<"\n";
std::vector<Vertex> current_shell;
std::vector<Vertex> next_shell;
Vertex  current_node;
typedef typename boost::graph_traits<G>::in_edge_iterator in_edge_it_t;
in_edge_it_t in, in_end;
//std::vector<Vertex>::iterator it;
for (int i =0;i<outputs.size();i++)
{
	network.clear();
	next_shell.clear();
    current_shell.push_back(outputs[i]);
    do
    {
    	while( !current_shell.empty() )
    	{
    		current_node = current_shell.back();
    		for (tie(in, in_end) = in_edges(current_node, g); in != in_end; ++in)
			{
				 Vertex src = source(*in, g), targ = current_node;
				 src_trg = std::make_pair(g[targ].get_id(),g[src].get_id());
				 network[src_trg]= 1.0;
				 //if not in the next shell add it
				 if (find (next_shell.begin(), next_shell.end(), src) ==next_shell.end())
				 {
				     next_shell.push_back(src);
				     std::cout<<" "<<g[source(*in, g)].get_id()<<" ";
				 }
			}
			current_shell.pop_back();
    	}
    	current_shell.swap( next_shell );

    }while (!current_shell.empty());

//BGL_FORALL_EDGES_T(e, g, G)
//         {
//           Vertex src = source(e, g), targ = target(e, g);
//           src_trg = std::make_pair(g[targ].get_id(),g[src].get_id());
//           network[src_trg]= 1.0; // this where all weights are set to 1
//         //network[src_trg]= g[e].get_weight().data(0);// uncommetn this for normal experiment
//           //std::cout<<"source :"<<g[src].get_id()<<" target: "<<g[targ].get_id()<<std::endl;
//         //std::cout<<g[e].get_weight().data(0)<<std::endl;
//         }
/*std::exit(1);
BGL_FORALL_VERTICES_T(v, g, G)
        {
           reaching_v = Get_Local_Reaching_Centrality(g,v);     
           if( reaching_v > max_reaching )
                  max_reaching = reaching_v;
           sum_reaching += reaching_v;
        }

   grc = ( (float)num_vertices(g)*max_reaching - sum_reaching )/(float)(weight_mean*(num_vertices(g)-1));*/
  net.load_network2(network,true,num_edges(g));
  net.get_weighted_local_reaching_centrality(lrc);
  net.get_global_reaching_centrality(lrc,grc);
  network.clear();
  lrc.clear();
  net.clear_network();
  std::cout<<"GRC for  "<<g[outputs[i]].get_id()<<" is :"<<grc<<" The LRC is "<<*std::max_element(lrc.begin(),lrc.end())<<"\n";
  total_grc+=grc;
}
cout<<"average grc is :"<<total_grc/outputs.size()<<"\n";
return total_grc/outputs.size();
 }
template<typename G>
inline float Get_GRC(G& g){
        float  max_reaching = -1.0;
        float  sum_reaching = 0.0;
        float  weight_mean = 1.0;
        float  grc = 0.0;
        float reaching_v;
        std::vector<double> lrc;
  NETWORK net;
  std::map<std::pair<std::string,std::string>,float> network;
  std::pair<std::string,std::string> src_trg;
typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;
std::vector<Vertex> outputs;
         // go through network nodes and get local reaching centarlities
        // record maximum reaching and sum of reachings too
//std::cout<<"preparing the reversed network for GRC computation\n";
//you can set the all weights here, for the weight experiment
//int outputs = g.get_outputs().size();
//cout<<"the number of output nodes is :"<<outputs<<"\n";
BGL_FORALL_EDGES_T(e, g, G)
         {
           Vertex src = source(e, g), targ = target(e, g);
           src_trg = std::make_pair(g[targ].get_id(),g[src].get_id());
           network[src_trg]= 1.0; // this where all weights are set to 1
         //network[src_trg]= g[e].get_weight().data(0);// uncommetn this for normal experiment
           //std::cout<<"source :"<<g[src].get_id()<<" target: "<<g[targ].get_id()<<std::endl;
         //std::cout<<g[e].get_weight().data(0)<<std::endl;
         }
/*std::exit(1);
BGL_FORALL_VERTICES_T(v, g, G)
        {
           reaching_v = Get_Local_Reaching_Centrality(g,v);
           if( reaching_v > max_reaching )
                  max_reaching = reaching_v;
           sum_reaching += reaching_v;
        }

   grc = ( (float)num_vertices(g)*max_reaching - sum_reaching )/(float)(weight_mean*(num_vertices(g)-1));*/
  net.load_network2(network,true,num_edges(g));
  net.get_weighted_local_reaching_centrality(lrc);
  net.get_global_reaching_centrality(lrc,grc);
  network.clear();
  lrc.clear();
  //std::cout<<"reversed: \n"<<grc<<"\n";
  net.clear_network();
  return grc;
 }
template<typename G>
inline float get_coords(G& g,std::vector<double>& _x, std::vector<double>& _y,std::map<std::string, std::pair<double,int> >& neuron_coords)
{
	std::cout<<"i am getting coordinates \n";
	std::vector<double> lrc;
	std::string output ="coords";//output file
	NETWORK net;
	std::map<std::pair<std::string,std::string>,float> network;
	std::pair<std::string,std::string> src_trg;
	std::vector<double> distribution;
	//some visualization parameres
	double z = 0.01; //the z-value of the visualization
	double dx = 1.5 ;//minimum horizontal distance
	double dy = 1.5;//minimum vertical distance

	//std::vector<double> x, y;
	typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;

	BGL_FORALL_EDGES_T(e, g, G)
	 {
	   Vertex src = source(e, g), targ = target(e, g);
	   src_trg = std::make_pair(g[targ].get_id(),g[src].get_id());
	   network[src_trg]= 1.0; // this where all weights are set to 1
	  //network[src_trg]= g[e].get_weight().data(0);// uncommetn this for normal experiment
	   //std::cout<<"source :"<<g[targ].get_id()<<" target: "<<g[src].get_id()<<std::endl;
	 //std::cout<<g[e].get_weight().data(0)<<std::endl;
	 }
// calculating lrc
	net.load_network2(network,true,num_edges(g));
	net.get_weighted_local_reaching_centrality(lrc);
	//getting the coordinates

	net.print_local_reaching_centrality( lrc);
	net.get_coordinates( z, dx, dy, lrc, _x, _y );
	net.get_neuron_coords(_x,_y,neuron_coords);
	net.print_coordinates( _x, _y, output );
	net.print_image( _x, _y, output );
	return 0.0;
}
}

#endif

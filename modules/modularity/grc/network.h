/*
 Header for the network class
 Author: Enys Mones, 2013
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "stdlib.h"
#include "node.h"
#include "file_handler.h"
#include <map>
#include <string>
#include "math.h"
#include "mother.h"
#include "time.h"

void shuffle_vector( std::vector<double> &vec );

class NETWORK
{
private:
    int Order;
    int Size;
    std::vector<NODE*> Nodes;
    CRandomMother Ran;
public:
    // constructors
    NETWORK();
    NETWORK( const NETWORK *_other );

    // structure
    int order() const;
    int size() const;
    void add_node();
    void add_node( const char *_label );
    void remove_node( int _id );
    bool is_there_edge( int _source, int _target ) const;
    void add_edge( int _source, int _target, double _weight );
    void remove_edge( int _source, int _target );
    double get_edge_weight( int _source, int _target ) const;
    void generate_random_graph( int _order, int _size, std::vector<double> &_weights );
    void load_network( std::string& _input_file, bool _weighted, std::string& _output, int _size_limit );
    void load_network2( std::map<std::pair<std::string , std::string>,float> network, bool _weighted, int _size_limit );
   void clear_network(); 
   void rewire( int _laps );

    // calculations
    void get_weights( std::vector<double> &_weights ) const;
    void get_default_local_reaching_centrality( std::vector<double>& _lrc ) const;
    void get_weighted_local_reaching_centrality( std::vector<double>& _lrc ) const;
    void get_global_reaching_centrality( std::vector<double>& _lrc, float& _grc ) const;
    void get_distribution( std::vector<double>& _lrc, std::vector<double>& _dist ) const;
    void get_coordinates( double _z, double _dx, double _dy, std::vector<double>& _lrc, std::vector<double>& _x, std::vector<double>& _y ) const;
    void get_neuron_coords(std::vector<double> _x, std::vector<double> _y,std::map<std::string, std::pair<double,int> >& neuron_coords);

    // output
    void print_local_reaching_centrality( std::vector<double> &_lrc) const;
    void print_distribution( std::vector<double> &_dist ) const;
    void print_coordinates( std::vector<double> &_x, std::vector<double> &_y, std::string &_output_name ) const;
    void print_image( std::vector<double>& _x, std::vector<double>& _y, std::string &_output_name ) const;
};

//void get_ER_randomized_grc( NETWORK *_network, int _samples, std::string &_output_name );
//void get_RW_randomized_grc( NETWORK *_network, int _samples, std::string &_output_name );

#endif // NETWORK_H

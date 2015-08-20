/*
 The header for the node class
 Author: Enys Mones, 2013
 */

#ifndef NODE_H
#define NODE_H

#include "stdlib.h"
#include <vector>
#include <string>

class NODE
{
private:
    int Id;
    std::string Label;
    std::vector<NODE*> OutNeighbors;
    std::vector<NODE*> InNeighbors;
    std::vector<double> OutWeights;
public:
    // constructors
    NODE( const NODE& _other );
    NODE( int _id, const char *_label );

    // setters and getters
    void setID( int _id );
    int id() const;
    std::string label() const;

    // structure manipulators
    // out
    void add_outneighbor( NODE *_node_ptr, double _weight );
    void remove_outneighbor( NODE *_node_ptr );
    NODE* outneighbor( int _pos );
    double outweight( int _pos ) const;
    void setOutWeight( int _pos, double _weight );
    int outdegree() const;
    // in
    void add_inneighbor( NODE *_node_ptr );
    void remove_inneighbor( NODE *_node_ptr );
    NODE* inneighbor( int _pos );
    int indegree() const;
};

#endif // NODE_H

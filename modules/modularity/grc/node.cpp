#include "node.h"

NODE::NODE( const NODE &_other )
{
  Id = _other.Id;
  Label = _other.Label;
  OutNeighbors = _other.OutNeighbors;
  InNeighbors = _other.InNeighbors;
  OutWeights = _other.OutWeights;
}

NODE::NODE( int _id, const char *_label )
{
  Id = _id;
  Label = std::string( _label );
  OutNeighbors.clear();
  InNeighbors.clear();
  OutWeights.clear();
}

void NODE::setID( int _id )
{
  Id = _id;
}

int NODE::id() const
{
  return Id;
}

std::string NODE::label() const
{
  return Label;
}

void NODE::add_outneighbor( NODE *_node_ptr, double _weight )
{
  OutNeighbors.push_back( _node_ptr );
  OutWeights.push_back( _weight );
}

void NODE::remove_outneighbor( NODE *_node_ptr )
{
  int n_size = (int)OutNeighbors.size();
  for(int i=0; i<n_size; i++)
  {
    if( OutNeighbors[i] == _node_ptr )
    {
      OutNeighbors[i] = OutNeighbors.back();
      OutNeighbors.pop_back();
      OutWeights[i] = OutWeights.back();
      OutWeights.pop_back();
      break;
    }
  }
}

NODE *NODE::outneighbor( int _pos )
{
  return OutNeighbors[_pos];
}

double NODE::outweight( int _pos ) const
{
  return OutWeights[_pos];
}

void NODE::setOutWeight( int _pos, double _weight )
{
  OutWeights[_pos] = _weight;
}

int NODE::outdegree() const
{
  return (int)OutNeighbors.size();
}

void NODE::add_inneighbor( NODE *_node_ptr )
{
  InNeighbors.push_back( _node_ptr );
}

void NODE::remove_inneighbor( NODE *_node_ptr )
{
  int n_size = (int)InNeighbors.size();
  for(int i=0; i<n_size; i++)
  {
    if( InNeighbors[i] == _node_ptr )
    {
      InNeighbors[i] = InNeighbors.back();
      InNeighbors.pop_back();
      break;
    }
  }
}

NODE *NODE::inneighbor( int _pos )
{
  return InNeighbors[_pos];
}

int NODE::indegree() const
{
  return (int)InNeighbors.size();
}

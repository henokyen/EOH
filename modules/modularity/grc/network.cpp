#include "network.h"
#include "node.cpp"
#include "file_handler.cpp"
#include "mother.cpp"
void shuffle_vector( std::vector<double> &vec )
{
  int len = (int)vec.size();
  int v1, v2;
  double elem;
  CRandomMother ran( rand()%1000 );

  for(int i=0; i<10*len; i++)
  {
    v1 = vec[ran.IRandom(0, len-1)];
    v2 = vec[ran.IRandom(0, len-1)];
    elem = vec[v1];
    vec[v1] = vec[v2];
    vec[v2] = elem;
  }
}

NETWORK::NETWORK()
{
  Order = 0;
  Size = 0;
  Nodes.clear();
  Ran.RandomInit( rand()%1000 );
}

NETWORK::NETWORK( const NETWORK *_other )
{
  Order = 0;
  Size = 0;
  Nodes.clear();
  Ran.RandomInit( rand()%1000 );

  // add nodes
  int _order = _other->order();
  for(int i=0; i<_order; i++)
    add_node( _other->Nodes[i]->label().c_str() );

  // add edges
  int _deg;
  for(int i=0; i<_order; i++)
  {
    _deg = _other->Nodes[i]->outdegree();
    for(int j=0; j<_deg; j++)
      add_edge( i, _other->Nodes[i]->outneighbor(j)->id(), _other->Nodes[i]->outweight(j) );
  }
}

int NETWORK::order() const
{
  return Order;
}

int NETWORK::size() const
{
  return Size;
}

void NETWORK::add_node()
{
  char _label[32] = {""};
  sprintf( _label, "%i", Order );
  Nodes.push_back( new NODE(Order, _label) );
  Order++;
}

void NETWORK::add_node( const char *_label )
{
  Nodes.push_back( new NODE(Order, _label) );
  Order++;
}

void NETWORK::remove_node( int _id )
{
  // remove as out neighbor
  int indeg = Nodes[_id]->indegree();
  for(int i=0; i<indeg; i++)
    Nodes[_id]->inneighbor(i)->remove_outneighbor( Nodes[_id] );
  // remove as in neighbor
  int outdeg = Nodes[_id]->outdegree();
  for(int i=0; i<outdeg; i++)
    Nodes[_id]->outneighbor(i)->remove_inneighbor( Nodes[_id] );

  // replace with last and pop last
  NODE* node_ptr = Nodes[_id];
  Nodes[_id] = Nodes.back();
  Nodes[_id]->setID( _id );
  Nodes.pop_back();
  delete node_ptr;
  Order--;
}

bool NETWORK::is_there_edge( int _source, int _target ) const
{
  int outdeg = Nodes[_source]->outdegree();
  for(int i=0; i<outdeg; i++)
  {
    if( Nodes[_source]->outneighbor(i) == Nodes[_target] )
      return true;
  }
  return false;
}

void NETWORK::add_edge( int _source, int _target, double _weight )
{
  Nodes[_source]->add_outneighbor( Nodes[_target], _weight );
  Nodes[_target]->add_inneighbor( Nodes[_source] );
  Size++;
}

void NETWORK::remove_edge( int _source, int _target )
{
  Nodes[_source]->remove_outneighbor( Nodes[_target] );
  Nodes[_target]->remove_inneighbor( Nodes[_source] );
  Size--;
}

double NETWORK::get_edge_weight( int _source, int _target ) const
{
  int deg = Nodes[_source]->outdegree();
  for(int j=0; j<deg; j++)
  {
    if( Nodes[_source]->outneighbor(j)->id() == _target )
      return Nodes[_source]->outweight(j);
  }
  return 0.0;
}

void NETWORK::generate_random_graph( int _order, int _size, std::vector<double> &_weights )
{
  for(int i=0; i<Order; i++)
    delete Nodes[i];
  Nodes.clear();
  Order = 0;
  Size = 0;

  for(int i=0; i<_order; i++)
    add_node();
  int source, target;
  int edges_left = _size;
  while( edges_left > 0 )
  {
    source = Ran.IRandom( 0, _order-1 );
    target = Ran.IRandom( 0, _order-1 );
    if( !is_there_edge(source, target) && source != target )
    {
      add_edge( source, target, _weights.back() );
      edges_left--;
      _weights.pop_back();
    }
  }
}

void NETWORK::load_network( std::string &_input_file, bool _weighted, std::string &_output, int _size_limit )
{
  // message file
  std::string msg_file = _output + ".msg";
  FILE *pMsg = NULL;

  // reset network
  for(int i=0; i<Order; i++)
    delete Nodes[i];
  Nodes.clear();
  Order = 0;
  Size = 0;

  // read edges
  FILE_HANDLER f( _input_file.c_str(), "r" );
  int edges = f.rows();
  char line[1024] = {""};
  std::map<std::string, int> hash_table;
  int source, target;
  double weight;
  char source_label[128] = {""};
  char target_label[128] = {""};
  int source_id = 0;
  int target_id = 0;
  int columns;
  for(int n=0; n<edges; n++)
  {
    f.get_text_line( line, sizeof(line) );
    // check if line is valid (two or threee columns)
    weight = 1.0;
    columns = sscanf( line, "%s %s %lf", source_label, target_label, &weight );
    if( (!_weighted && columns < 2) || (_weighted && columns != 3) )
    {
      pMsg = fopen( msg_file.c_str(), "w" );
      fprintf( pMsg, "Error\nMissing column in line %i.", n+1 );
      fclose( pMsg );
      exit( 0 );
    }
    std::pair< std::map<std::string, int>::iterator, bool> added;

    // source node
    added = hash_table.insert( std::pair<std::string, int>(std::string(source_label), (int)Nodes.size()) );
    if( added.second )
    {
      source_id = (int)Nodes.size();
      add_node( source_label );
    }
    else
      source_id = added.first->second;

    // target node
    added = hash_table.insert( std::pair<std::string, int>(std::string(target_label), (int)Nodes.size()) );
    if( added.second )
    {
      target_id = (int)Nodes.size();
      add_node( target_label );
    }
    else
      target_id = added.first->second;

    // add edge
    if( is_there_edge(source_id, target_id) )
    {
      weight += get_edge_weight( source_id, target_id );
      remove_edge(source_id, target_id);
      add_edge( source_id, target_id, weight );
    }
    else
      add_edge( source_id, target_id, weight );
  }

  // if size limitazion is on, check network size
  if( _size_limit != 0 && Order > _size_limit )
  {
    pMsg = fopen( msg_file.c_str(), "w" );
    fprintf( pMsg, "Error\nNetwork is too large." );
    fclose( pMsg );
    exit(0);
  }

  // normalize weights
  /*int deg;
  double max_weight = Nodes[0]->outweight(0);
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
    {
      if( Nodes[i]->outweight(j) > max_weight )
        max_weight = Nodes[i]->outweight(j);
    }
  }
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
      Nodes[i]->setOutWeight( j, Nodes[i]->outweight(j)/max_weight );
  }*/

  // print network properties
  //pMsg = fopen( msg_file.c_str(), "w" );
  //fprintf( pMsg, "Success\n%i\n%i", Order, Size );
  //fclose( pMsg );
}

void NETWORK::load_network2(std::map<std::pair<std::string , std::string>,float> network, bool _weighted, int edges )
{
  // message file
  //std::string msg_file = _output + ".msg";
  //FILE *pMsg = NULL;

  // reset network
  for(int i=0; i<Order; i++)
    delete Nodes[i];
  Nodes.clear();
  Order = 0;
  Size = 0;

  // read edges
 // FILE_HANDLER f( _input_file.c_str(), "r" );
  //int edges = f.rows();
  //char line[1024] = {""};
  std::map<std::string, int> hash_table;
  int source, target;
  double weight;
  //char source_label[128] = {""};
  //char target_label[128] = {""};
  int source_id = 0;
  int target_id = 0;
  int columns;
  if (network.size() !=0)
  {
  std::map<std::pair<std::string , std::string>,float>::iterator it;
  for (std::map<std::pair<std::string , std::string>,float>::iterator it=network.begin(); it!=network.end(); ++it)
   {
    //f.get_text_line( line, sizeof(line) );
    //// check if line is valid (two or threee columns)
    //weight = 1.0;
    //columns = sscanf( line, "%s %s %lf", source_label, target_label, &weight );
    //if( (!_weighted && columns < 2) || (_weighted && columns != 3) )
    //{
    //  pMsg = fopen( msg_file.c_str(), "w" );
    //  fprintf( pMsg, "Error\nMissing column in line %i.", n+1 );
    //  fclose( pMsg );
    //  exit( 0 );
    //}
    std::pair< std::map<std::string, int>::iterator, bool> added;

    // source node
    added = hash_table.insert( std::pair<std::string, int>(std::string(it->first.first), (int)Nodes.size()) );
    if( added.second )
    {
      source_id = (int)Nodes.size();
	  add_node( it->first.first.c_str() );
    }
    else
      source_id = added.first->second;

    // target node
    added = hash_table.insert( std::pair<std::string, float>(std::string(it->first.second), (int)Nodes.size()) );
    if( added.second )
    {
      target_id = (int)Nodes.size();
      add_node( it->first.second.c_str());
    }
    else
      target_id = added.first->second;

    // add edge
    if( is_there_edge(source_id, target_id) )
    {
      weight += get_edge_weight( source_id, target_id );
      remove_edge(source_id, target_id);
      add_edge( source_id, target_id, abs(it->second)  );
    }
    else
      add_edge( source_id, target_id, abs(it->second) );
  }
}

  // if size limitazion is on, check network size
  //if( _size_limit != 0 && Order > _size_limit )
  //{
  //  pMsg = fopen( msg_file.c_str(), "w" );
  //  fprintf( pMsg, "Error\nNetwork is too large." );
  //  fclose( pMsg );
  //  exit(0);
  //}

  // normalize weights
  // uncomment this part for normal hierarchy computation
  // this commented out for no weight hierarchy computation
  /*int deg;
  double max_weight = Nodes[0]->outweight(0);
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
    {
      if( Nodes[i]->outweight(j) > max_weight )
        max_weight = Nodes[i]->outweight(j);
    }
  }
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
      Nodes[i]->setOutWeight( j, Nodes[i]->outweight(j)/max_weight );
  }*/

  // print network properties
  //pMsg = fopen( msg_file.c_str(), "w" );
  //fprintf( pMsg, "Success\n%i\n%i", Order, Size );
  //fclose( pMsg );
}

void NETWORK::rewire( int _laps )
{
  // weights
  std::vector<double> weights;
  get_weights( weights );
  shuffle_vector( weights );

  // structure
  int wirings_left = _laps;
  int s1, s2, deg1, deg2, p1, p2, t1, t2;
  while( wirings_left > 0 )
  {
    // choose two edge randomly
    s1 = Ran.IRandom( 0, Order-1 );
    deg1 = Nodes[s1]->outdegree();
    s2 = Ran.IRandom( 0, Order-1 );
    deg2 = Nodes[s2]->outdegree();
    if( s1 != s2 && deg1 != 0 && deg2 != 0 )
    {
      p1 = Ran.IRandom( 0, deg1-1 );
      t1 = Nodes[s1]->outneighbor(p1)->id();
      p2 = Ran.IRandom( 0, deg2-1 );
      t2 = Nodes[s2]->outneighbor(p2)->id();
      if( t1 != t2 && t1 != s2 && t2 != s1 )
      {
        remove_edge( s1, t1 );
        remove_edge( s2, t2 );
        add_edge( s1, t2, 1.0 );
        add_edge( s2, t1, 1.0 );
        wirings_left--;
      }
    }
  }

  // set weights
  int deg;
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
    {
      Nodes[i]->setOutWeight( j, weights.back() );
      weights.pop_back();
    }
  }
}

void NETWORK::get_weights( std::vector<double> &_weights ) const
{
  _weights.clear();
  int deg = 0;

  // get weights
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
      _weights.push_back( Nodes[i]->outweight(j) );
  }
}

void NETWORK::get_default_local_reaching_centrality( std::vector<double>& _lrc ) const
{
  _lrc.assign( Order, 0.0 );

  std::vector<int> distance;
  std::vector<int> node_stack;
  int current = 0;
  int outdeg = 0;
  int reachable_set;
  for(int i=0; i<Order; i++)
  {
    // init stack and unvisited arrays
    distance.assign( Order, -1 );
    distance[i] = 0;
    outdeg = Nodes[i]->outdegree();
    node_stack.clear();
    for(int j=0; j<outdeg; j++)
    {
      node_stack.push_back( Nodes[i]->outneighbor(j)->id() );
      distance[Nodes[i]->outneighbor(j)->id()] = 1;
    }
    reachable_set = 0;

    // perform search
    while( !node_stack.empty() )
    {
      current = node_stack.back();
      outdeg = Nodes[current]->outdegree();
      node_stack.pop_back();
      reachable_set++;

      for(int j=0; j<outdeg; j++)
      {
        if( distance[Nodes[current]->outneighbor(j)->id()] == -1 )
        {
          node_stack.push_back( Nodes[current]->outneighbor(j)->id() );
          distance[Nodes[current]->outneighbor(j)->id()] = distance[current] + 1;
        }
      }
    }
    _lrc[i] = (double)reachable_set / double(Order-1);
  }
}

void NETWORK::get_weighted_local_reaching_centrality( std::vector<double>& _lrc ) const
{
  _lrc.assign( Order, 0.0 );

  std::vector<int> distance;
  std::vector<float> max_weight;
  std::vector<int> current_shell;
  std::vector<int> next_shell;
  int current_node;
  float current_weight;
  int deg;

  for(int i=0; i<Order; i++)
  {
    // init arrays and indices
    distance.assign( Order, -1 );
    distance[i] = 0;
    max_weight.assign( Order, 0.0 );
    max_weight[i] = 0.0;
    current_shell.assign( 1, i );
    next_shell.clear();

    // go through on different distance-shells
    do
    {
      while( !current_shell.empty() )
      {
        current_node = current_shell.back();
        deg = (int)Nodes[current_node]->outdegree();
        for(int j=0; j<deg; j++)
        {
          current_weight = max_weight[current_node] + (Nodes[current_node]->outweight(j)-max_weight[current_node])/float(distance[current_node]+1);
          if( distance[Nodes[current_node]->outneighbor(j)->id()] == -1 )
          {
            next_shell.push_back( Nodes[current_node]->outneighbor(j)->id() );
            distance[Nodes[current_node]->outneighbor(j)->id()] = distance[current_node] + 1;
            max_weight[Nodes[current_node]->outneighbor(j)->id()] = current_weight;
          }
          else if( distance[Nodes[current_node]->outneighbor(j)->id()] == distance[current_node] + 1)
          {
            if( current_weight > max_weight[Nodes[current_node]->outneighbor(j)->id()] )
              max_weight[Nodes[current_node]->outneighbor(j)->id()] = current_weight;
          }
        }
        current_shell.pop_back();
      }
      current_shell.swap( next_shell );
    } while( !current_shell.empty() );

    // calculate reaching
    for(int j=0; j<Order; j++)
    {
      if( distance[j] > 0 )
        _lrc[i] += max_weight[j];
    }
    _lrc[i] /= double(Order-1);
  }
}

void NETWORK::get_global_reaching_centrality( std::vector<double>& _lrc, float &_grc ) const
{
  _grc = 0.0;
  float max_lrc = 0.0;
  for(int i=0; i<Order; i++)
  {
    if( _lrc[i] > max_lrc )
        max_lrc = _lrc[i];
  }
  for(int i=0; i<Order; i++)
    _grc += max_lrc - _lrc[i];

  _grc /= float(Order-1);
}
void NETWORK::clear_network(){
 for(int i=0; i< Order; i++)
    delete Nodes[i];
  Nodes.clear();
  Order = 0;
  Size = 0;
}

void NETWORK::get_distribution( std::vector<double>& _lrc, std::vector<double>& _dist ) const
{
  _dist.assign( 201, 0.0 );
  for(int i=0; i<Order; i++)
    _dist[ int(_lrc[i]*200) ]++;
  for(int i=0; i<=200; i++)
    _dist[i] /= (double)Order;
}

void NETWORK::get_coordinates( double _z, double _dx, double _dy, std::vector<double>& _lrc, std::vector<double>& _x, std::vector<double>& _y ) const
{
  /// create reachable set sizes
  std::vector<double> rss( Order, 0.0 );
  for(int i=0; i<Order; i++)
    rss[i] = (double)Order * _lrc[i];

  /// make levels
  // sort local reaching centralities
  std::vector<int> ids( Order, 0 );
  for(int i=0; i<Order; i++)
    ids[i] = i;
  int len = Order;
  double rss_elem;
  int id;
  bool swapped = false;
  do
  {
    swapped = false;
    for(int i=1; i<len; i++)
    {
      if( rss[i-1] > rss[i] )
      {
        rss_elem = rss[i-1];
        rss[i-1] = rss[i];
        rss[i] = rss_elem;
        id = ids[i-1];
        ids[i-1] = ids[i];
        ids[i] = id;
        swapped = true;
      }
    }
    len--;
  } while( swapped );

  // network reaching centrality mean and variance
  double global_av = 0.0;
  double global_sd = 0.0;
  double threshold = 0.0;
  for(int i=0; i<Order; i++)
  {
    global_av += rss[i];
    global_sd += rss[i]*rss[i];
  }
  global_av /= (double)Order;
  global_sd = sqrt( fabs(global_sd/(double)Order - global_av*global_av) );
  threshold = _z * global_sd + 0.00001; // in case of GRC = 0

  // sort nodes in levels
  int number_of_levels = 0;
  int level_first_node = 0;
  int current_node = 1;
  std::vector<int> levels( Order, -1 );
  levels[0] = 0;
  // start putting nodes in level until we reach last node
  while( current_node < Order )
  {
    // pick the next node and calculate level average and standard deviation
    double level_av = 0.0;
    double level_sd = 0.0;
    for(int i=level_first_node; i<=current_node; i++)
    {
      level_av += rss[i];
      level_sd += rss[i]*rss[i];
    }
    level_av /= 1 + current_node - level_first_node;
    level_sd = sqrt( fabs(level_sd/double(1+current_node-level_first_node) - level_av*level_av) );

    // if level deviation is lower than the threshold, keep node, otherwise start new level and put node there
    if( level_sd < threshold )
    {
      levels[current_node] = number_of_levels;
      current_node++;
    }
    else
    {
      levels[current_node] = number_of_levels + 1;
      number_of_levels++;
      level_first_node = current_node;
      current_node++;
    }
  }
  number_of_levels++;

  /// set y coordinates
  std::vector<int> nodes_per_level( number_of_levels, 0 );
  // calculate level averages
  std::vector<double> averages( number_of_levels, 0.0 );
  for(int i=0; i<Order; i++)
  {
    averages[levels[i]] += rss[i];
    nodes_per_level[levels[i]]++;
  }
  for(int i=0; i<number_of_levels; i++)
    averages[i] /= nodes_per_level[i];

  std::vector<double> y_differences( number_of_levels, 0.0 );
  std::vector<double> y_coordinates( number_of_levels, 0.0 );
  y_differences[0] = 0.0;
  y_coordinates[0] = 0.0;
  double minimum_difference = _dy * (double)Order;
  for(int i=1; i<number_of_levels; i++)
  {
    y_differences[i] = log( averages[i]-averages[i-1] + 1.0 );

    if( y_differences[i] < minimum_difference )
      minimum_difference = y_differences[i];
  }
  for(int i=1; i<number_of_levels; i++)
    y_coordinates[i] = y_coordinates[i-1] + _dy*y_differences[i]/minimum_difference;

  _y.assign( Order, 0.0 );
  for(int i=0; i<Order; i++)
    _y[ids[i]] = y_coordinates[levels[i]];

  /// set x coordinates
  std::vector<double> x_coordinate_starts( number_of_levels, 0.0 );
  for(int i=0; i<number_of_levels; i++)
  {
    x_coordinate_starts[i] = -0.5 * _dx * (nodes_per_level[i] - 1);
    nodes_per_level[i] = 0;
  }
  _x.assign( Order, 0.0 );
  for(int i=0; i<Order; i++)
  {
    _x[ids[i]] = x_coordinate_starts[levels[i]] + _dx * nodes_per_level[levels[i]];
    nodes_per_level[levels[i]]++;
  }
}

void NETWORK::print_local_reaching_centrality( std::vector<double> &_lrc) const
{
  std::string file_name = "lrc.dat";
  FILE_HANDLER f( file_name.c_str(), "w" );
  char line[1024] = {"#Label\tId\tReaching"};
  f.print_text_line( line );
  for(int i=0; i<Order; i++)
  {
    sprintf( line, "%s\t%i\t%lg", Nodes[i]->label().c_str(), i, _lrc[i] );
    f.print_text_line( line );
  }
}

void NETWORK::print_distribution( std::vector<double> &_dist ) const
{
  std::string file_name = "_dist.dat";
  FILE_HANDLER f( file_name.c_str(), "w" );
  std::vector<double> cols( 2, 0.0 );

  char line[1024] = {"#C\tP(C)"};
  f.print_text_line( line );

  for(int i=0; i<=200; i++)
  {
    cols[0] = (double)i/200.0;
    cols[1] = _dist[i];
    f.print_data_line( cols );
  }
}

void NETWORK::print_coordinates( std::vector<double> &_x, std::vector<double> &_y, std::string &_output_name ) const
{
  std::string file_name = _output_name + "_coords.dat";
  FILE_HANDLER f( file_name.c_str(), "w" );

  char line[1024] = {"#Label\tId\tX\tY"};
  f.print_text_line( line );

  for(int i=0; i<Order; i++)
  {
    sprintf( line, "%s\t%i\t%lg\t%lg", Nodes[i]->label().c_str(), i, _x[i], _y[i] );
    f.print_text_line( line );
  }

}
void NETWORK::get_neuron_coords(std::vector<double> _x, std::vector<double> _y,std::map<std::string, std::pair<double,int> >& neuron_coords)
{
  std::map<double, int> levels;
  std::vector<double> _temp_y = _y;
  sort(_temp_y.begin(),_temp_y.end());

	int i =0;
	int j = 0;
	while (i<_y.size())
	{
		if(levels.find(_temp_y[i])== levels.end())
		{
			levels[_temp_y[i]] = j;
			std::cout<<_temp_y[i] <<" is level "<<j<<"\n";
			j=j+1;
		}
		i++;
	}
  for(int i=0; i<Order; i++)
  {
	//std::pair<double,double> coords = std::make_pair(_x[i], _y[i]);// Nodes[i]->label().c_str(), i, _x[i], _y[i] );
	std::pair<double,int> coords = std::make_pair(_x[i], levels[_y[i]]);
	neuron_coords[Nodes[i]->label().c_str()] = coords;
  }
  for (	 std::map<std::string, std::pair<double,int> >::iterator it = neuron_coords.begin();it!=neuron_coords.end();++it)
  			std::cout<<it->first<<" => "<<it->second.first<<" "<<it->second.second<<'\n';
}

//
void NETWORK::print_image( std::vector<double>& _x, std::vector<double>& _y, std::string &_output_name ) const
{
   //write tikz file
  std::string file_name = _output_name + ".tex";
  FILE* pFile = fopen( file_name.c_str(), "w" );
  fprintf( pFile, "\\documentclass[a4paper,landscape]{scrartcl}\n\n" );
  fprintf( pFile, "\\usepackage{amsmath}\n" );
  fprintf( pFile, "\\usepackage{verbatim}\n" );
  fprintf( pFile, "\\usepackage[active,tightpage]{preview}\n" );
  fprintf( pFile, "\\usepackage{fancybox}\n" );
  fprintf( pFile, "\\usepackage{tikz}\n" );
  fprintf( pFile, "\\usetikzlibrary{arrows,positioning}\n\n" );
  fprintf( pFile, "\\setlength\\PreviewBorder{20pt}%%\n\n" );
  fprintf( pFile, "\\begin{document}\n" );
  fprintf( pFile, "\\begin{preview}\n" );
  fprintf( pFile, "\\begin{tikzpicture}[\n" );
  fprintf( pFile, "invisible/.style={circle,inner sep=0,minimum size=1cm},\n" );
  fprintf( pFile, "visible/.style={circle,very thick,fill=red!10!green!10!blue!40,draw=blue!70,inner sep=0,minimum size=1cm},\n" );
  fprintf( pFile, "edge/.style={->,thin,black,opacity=0.6,>=triangle 45}]\n" );
  fprintf( pFile, "%% invisible nodes\n" );
  for(int i=0; i<Order; i++)
    fprintf( pFile, "\\node[invisible] (n%i) at (%.3fcm,%.3fcm) {};\n", i, _x[i], _y[i] );
  fprintf( pFile, "%% edges\n" );
  int deg;
  for(int i=0; i<Order; i++)
  {
    deg = Nodes[i]->outdegree();
    for(int j=0; j<deg; j++)
      fprintf( pFile, "\\draw[edge] (n%i) edge (n%i);\n", i, Nodes[i]->outneighbor(j)->id() );
  }
  fprintf( pFile, "%% visible nodes\n" );
  for(int i=0; i<Order; i++)
    fprintf( pFile, "\\node[visible] (n%i) at (%.3fcm,%.3fcm) {%s};\n", i, _x[i], _y[i], Nodes[i]->label().c_str() );
  fprintf( pFile, "\n\\end{tikzpicture}\n" );
  fprintf( pFile, "\\end{preview}\n" );
  fprintf( pFile, "\\end{document}\n" );
  fclose( pFile );

   //compile
  //std::string command = "pdflatex " + file_name;
  //system( command.c_str() );
  //command = "rm " + _output_name + ".aux " + _output_name + ".log " + _output_name + ".tex";
 // system( command.c_str() );
}

//void get_ER_randomized_grc( NETWORK *_network, int _samples, std::string &_output_name )
//{
//  int order = _network->order();
//  int size = _network->size();
//  std::string file_name = _output_name + "_rand_ER.dat";
//  FILE *pOut = NULL;
//  std::vector<double> weights;
//  _network->get_weights( weights );
//
//  std::vector<double> lrc;
//  double grc;
//  pOut = fopen( file_name.c_str(), "w" );
//  fprintf( pOut, "# Desc: GRC of the ER random graphs.\n" );
//  fclose( pOut );
//  for(int s=0; s<_samples; s++)
//  {
//    NETWORK rand_net;
//    std::vector<double> w = weights;
//    shuffle_vector( w );
//    rand_net.generate_random_graph( order, size, w );
//    rand_net.get_default_local_reaching_centrality( lrc );
//    rand_net.get_global_reaching_centrality( lrc, grc );
//
//    pOut = fopen( file_name.c_str(), "a" );
//    fprintf( pOut, "%lg\n", grc );
//    fclose( pOut );
//  }
//}
//
//void get_RW_randomized_grc( NETWORK *_network, int _samples, std::string &_output_name )
//{
//  int order = _network->order();
//  int _size = _network->size();
//  std::string file_name = _output_name + "_rand_RW.dat";
//  FILE *pOut = NULL;
//
//  std::vector<double> lrc;
//  double grc;
//  pOut = fopen( file_name.c_str(), "w" );
//  fprintf( pOut, "# Desc: GRC of the rewired graphs\n# In each case, the number of rewires is the number of edges,\n# meaning that in average, every edge is replaced.\n" );
//  fclose( pOut );
//  for(int s=0; s<_samples; s++)
//  {
//    NETWORK rand_net( _network );
//    rand_net.rewire( _size );
//    rand_net.get_default_local_reaching_centrality( lrc );
//    rand_net.get_global_reaching_centrality( lrc, grc );
//
//    pOut = fopen( file_name.c_str(), "a" );
//    fprintf( pOut, "%lg\n", grc );
//    fclose( pOut );
//  }
//}

#include "file_handler.h"

FILE_HANDLER::FILE_HANDLER()
{
  P = NULL;
  State = EMPTY;
  FileName = "_UNDEFINED_";
}

FILE_HANDLER::FILE_HANDLER( const char *_file_name, const char *_mode )
{
  // set file name
  FileName = std::string( _file_name );

  // set state
  State = READ;
  if( strcmp( _mode, "w" ) == 0 )
    State = WRITE;

  // try to open depending on _state
  P = NULL;
  switch( State )
  {
    case READ:
    {
      P = fopen( FileName.c_str(), "r" );
      break;
    }
    case WRITE:
    {
      P = fopen( FileName.c_str(), "w" );
      break;
    }
  }

  // if failed, print error
  if( P == NULL )
    perror( "constructor error: file not found" );
}

FILE_HANDLER::~FILE_HANDLER()
{
  if( State != EMPTY )
    fclose( P );
}

void FILE_HANDLER::perror( const char *_error_message ) const
{
  if( FileName == "_UNDEFINED_" )
    printf( "%s\n", _error_message );
  else
    printf( "%s (\"%s\").\n", _error_message, FileName.c_str() );
  exit(0);
}

void FILE_HANDLER::check_state( const char *_function, FILE_STATE _required_state ) const
{
  if( State == _required_state )
    return;
  else
  {
    std::string err_msg = std::string( _function ) + " error: ";
    switch( State )
    {
      case EMPTY:
      {
        err_msg += "file is not open";
        break;
      }
      case WRITE:
      {
        err_msg += "file is open for writing";
        break;
      }
      case READ:
      {
        err_msg += "file is open for reading";
        break;
      }
    }
    perror( err_msg.c_str() );
  }
}

void FILE_HANDLER::open( const char *_file_name, const char *_mode )
{
  // set file name
  FileName = std::string( _file_name );

  // set state
  State = READ;
  if( strcmp( _mode, "w" ) == 0 )
    State = WRITE;

  // try to open depending on _state
  P = NULL;
  switch( State )
  {
    case READ:
    {
      P = fopen( FileName.c_str(), "r" );
      break;
    }
    case WRITE:
    {
      P = fopen( FileName.c_str(), "w" );
      break;
    }
  }

  // if failed, print message and exit
  if( P == NULL )
    perror( "open error: file not found" );
}

void FILE_HANDLER::close()
{
  switch( State )
  {
    case EMPTY:
      perror( "close error: file is not opened" );

    case READ:
    case WRITE:
    {
      fclose( P );
      State = EMPTY;
      FileName = "_UNDEFINED_";
    }
  }
}

void FILE_HANDLER::go_to_top()
{
  check_state( "go_to_top", READ );

  fseek( P, 0, SEEK_SET );
}

int FILE_HANDLER::rows() const
{
  check_state( "rows", READ );

  char line[1024] = {""};
  int _rows = 0;
  fseek( P, 0, SEEK_SET );
  while( !feof(P) )
  {
    fgets( line, sizeof(line), P );
    _rows++;
  }
  fseek( P, 0, SEEK_SET );

  return _rows-1;
}

int FILE_HANDLER::columns() const
{
  check_state( "columns", READ );

  char line[1024] = {""};
  char *line_ptr = fgets( line, sizeof(line), P );
  char col[1024] = {""};
  int _columns = 0, shift = 0;
  fseek( P, 0, SEEK_SET );
  while( sscanf(line_ptr, "%s%n", col, &shift) == 1 )
  {
    _columns++;
    line_ptr += shift;
  }
  fseek( P, 0, SEEK_SET );

  return _columns;
}

void FILE_HANDLER::print_data_line( std::vector<double> &_data_vec )
{
  check_state( "print_data_line", WRITE );

  int len = (int)_data_vec.size();
  fprintf( P, "%lg", _data_vec[0] );
  for(int i=1; i<len; i++)
    fprintf( P, " %lg", _data_vec[i] );
  fprintf( P, "\n" );
}

void FILE_HANDLER::print_text_line( char *_text )
{
  check_state( "print_text_line", WRITE );

  fprintf( P, "%s\n", _text );
}

int FILE_HANDLER::get_data_line( std::vector<double> &_data_vec )
{
  check_state( "get_data_line", READ );

  int num_data_element = 0;
  int shift = 0;
  double data_element = 0.0;
  char *data_line, line[1024] = {""};

  _data_vec.clear();
  data_line = fgets( line, sizeof(line), P );
  while( sscanf(data_line, "%lf%n", &data_element, &shift) == 1 )
  {
    _data_vec.push_back( data_element );
    num_data_element++;
    data_line += shift;
  }

  return num_data_element;
}

void FILE_HANDLER::get_text_line( char *_text, int _length )
{
  check_state( "get_text_line", READ );

  fgets( _text, _length, P );
  _text[strlen(_text)-1] = '\0';
}

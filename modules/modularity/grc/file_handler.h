/*
 * FILE_HANDLER:
 * An intelligent file handling class
 * that handles errors and takes care
 * about closing the file.
 */

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include <vector>
#include <string>

class FILE_HANDLER
{
private:
  FILE *P;
  int State;
  std::string FileName;
public:
  enum FILE_STATE{ EMPTY, WRITE, READ };

  FILE_HANDLER();
  FILE_HANDLER( const char *_file_name, const char *_mode );
  ~FILE_HANDLER();
  void perror( const char *_error_message ) const;
  void check_state( const char *_function, FILE_STATE _required_state ) const;
  void open( const char *_file_name, const char *_mode );
  void close();
  void go_to_top();
  int rows() const;
  int columns() const;
  void print_data_line( std::vector<double> &_data_vec );
  void print_text_line( char *_text );
  int get_data_line( std::vector<double> &_data_vec );
  void get_text_line( char *_text, int _length );
};

#endif // FILE_HANDLER_H

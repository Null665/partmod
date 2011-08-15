#ifndef DISK_EXCEPTION_H
#define DISK_EXCEPTION_H

#include <string>
#include <exception>
#include <sstream>

// For throwing and catching exeptions
class DiskException:public std::exception
{
public:

 int error_code; // error codes are defined in definitions.h
 std::string message;


 DiskException(int error_code)
  {
    this->error_code=error_code;
  }
 DiskException(int error_code,std::string message)
  {
    this->error_code=error_code;
    this->message=message;
  }
 DiskException(std::string message)
  {
    this->error_code=0;
    this->message=message;
  }
 virtual const char* what() const throw()
  {
    std::stringstream ss;
    if(error_code!=0)
        ss <<"Error code: "<<error_code<<"("<<error_table[error_code]<<")"<<'\n';
    if(!message.empty())
        ss <<"Message: "<<message<<'\n';

    return ss.str().c_str();

  }
  ~DiskException() throw(){}

};

#endif

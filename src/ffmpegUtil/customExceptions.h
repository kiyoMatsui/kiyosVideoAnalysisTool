#ifndef CUSTOMEXCEPTIONS_H
#define CUSTOMEXCEPTIONS_H

#include <exception>
#include <type_traits>



struct mediaSourceNotSetException : virtual public std::exception {
  const char* what() const noexcept {return "mediaSrc !Set";}
};


struct mediaSourceWrongException : virtual public std::exception {
  const char* what() const noexcept {return "mediaSrc !OK";}
};

struct shaderCompileException : virtual public std::exception {
  const char* what() const noexcept {return "shader !OK";}
};


#endif

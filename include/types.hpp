#pragma once
#include "common.hpp"

enum DataType { TYPE_INT, TYPE_FLOAT, TYPE_STRING };

class varValue {
public:
  DataType type;
  int i_val;
  float f_val;
  string s_val;

  varValue() {
     type = TYPE_INT;
     i_val = 0;
     f_val = 0.0f;
     s_val = "";
  }
};

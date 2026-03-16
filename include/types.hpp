#pragma once

enum DataType { TYPE_INT, TYPE_FLOAT, TYPE_STRING };

struct varValue {
    DataType type = TYPE_INT;
    int i_val = 0;
    float f_val = 0.0f;
    string s_val = "";
};

#pragma once

#include <cstring>

template<class T>
static T bitcast(char* data) {
    T result;
    std::memcpy(&result, data, sizeof(T));
    return result;
}
#ifndef EWOMS_ICD_CONVERT_H
#define EWOMS_ICD_CONVERT_H

namespace Ewoms {

template<typename T>
T from_int(int int_status);

template<typename T>
int to_int(T status);

}

#endif

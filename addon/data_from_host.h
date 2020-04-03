#include <node_api.h>

/**
 * Method that will be exposed to electron and provides the ability to get a
 * string value from the native C++ code
 */
 napi_value GetDataFromHost(napi_env env, napi_callback_info info);

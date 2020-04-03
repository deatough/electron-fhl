#include "data_from_host.h"
#include "napi_utils.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>

/**
 * Method that will be exposed to electron and provides the ability to get a
 * string value from the native C++ code
 */
napi_value GetDataFromHost(napi_env env, napi_callback_info /*info*/)
{
    std::stringstream buffer;
    buffer << __FILE__ << " (" << __LINE__ << ")";

    napi_value result;
    if (!CheckNApiCallResult(env, napi_create_string_utf8(env, buffer.str().c_str(), NAPI_AUTO_LENGTH, &result)))
    {
        return nullptr;
    }

    return result;
}

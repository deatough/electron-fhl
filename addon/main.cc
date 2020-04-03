#include <node_api.h>
#include "napi_utils.h"
#include "data_from_host.h"
#include "start_clock.h"

/**
 * Helper method that adds a function to the specified object.
 */
bool AddFunctionToObject(napi_env env, napi_value object, const char* name, napi_callback function)
{
    napi_value napiFunction;

    if (!CheckNApiCallResult(env, napi_create_function(env, name, NAPI_AUTO_LENGTH, function, nullptr, &napiFunction)))
    {
        return false;
    }

    return CheckNApiCallResult(env, napi_set_named_property(env, object, name, napiFunction));
}

/**
 * We enable JavaScript to call into C++ by creating an object using the N-Api that will be
 * returned (via the napi_value) and then adding the methods that we wish to expose to JavaScript.
 */
napi_value create_addon(napi_env env) {
    napi_value addOnObject = nullptr;
    if (CheckNApiCallResult(env, napi_create_object(env, &addOnObject)))
    {
        if (AddFunctionToObject(env, addOnObject, "startClock", StartClock)
            && AddFunctionToObject(env, addOnObject, "getDataFromHost", GetDataFromHost))
        {
            return addOnObject;
        }
    }

    // something went wrong exporting the function
    return nullptr;
}

NAPI_MODULE_INIT()
{
    return create_addon(env);
}
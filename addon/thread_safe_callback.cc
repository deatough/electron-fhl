#include "thread_safe_callback.h"
#include "napi_utils.h"

/**
 * Called when the the ThreadSafeFunction has been finalized
 */
__declspec(noinline) /*static*/ void ThreadSafeCallback::NApiFinalizeCallback(napi_env /*env*/, void* finalize_data, void* /*finalize_hint*/)
{
    ThreadSafeCallback* spThis = reinterpret_cast<ThreadSafeCallback*>(finalize_data);

    std::lock_guard<std::mutex> lockForUpdate(spThis->lockObject);
    spThis->function = nullptr;
}

/**
 * Called to initialize the thread safe function, when invoked the napi_threadsafe_function_call_js
 * will be called to marshal the arguments for the JavaScript call.
 */
__declspec(noinline) bool ThreadSafeCallback::InitializeThreadSafeFunction(napi_env env, const char* javaScriptFunction, napi_threadsafe_function_call_js call_js_cb)
{
    std::lock_guard<std::mutex> lockForUpdate(lockObject);
    if (function != nullptr)
    {
        // the thread safe function has already been initialized
        return true;
    }

    // The JavaScript function will be loacated on the global object, so we need to first
    // get the global object.
    napi_value global;
    if (!CheckNApiCallResult(env, napi_get_global(env, &global)))
    {
        return false;
    }

    // Then get the function via the name property and confirm that it is in fact a function
    napi_value functionToCall;
    if (!CheckNApiCallResult(env, napi_get_named_property(env, global, javaScriptFunction, &functionToCall)))
    {
        return false;
    }

    napi_valuetype functionType;
    if (!CheckNApiCallResult(env, napi_typeof(env, functionToCall, &functionType)))
    {
        return false;
    }

    if (functionType != napi_valuetype::napi_function)
    {
        return false;
    }

    // In order to create a thread safe function we need to supply an async resouce name, if
    // we don't we will get an invalid argument result when creating the thread safe function.
    napi_value asyncResourceName;
    if (!CheckNApiCallResult(env, napi_create_string_utf8(env, javaScriptFunction, NAPI_AUTO_LENGTH, &asyncResourceName)))
    {
        return false;
    }

    napi_value noAsycnResource = nullptr;
    size_t unlimitedQueueSize = 0;
    size_t initialThreadCount = 1;
    void* finalizeData = this;
    void* noContext = nullptr;
    return CheckNApiCallResult(env, napi_create_threadsafe_function(env, functionToCall, noAsycnResource, asyncResourceName,
        unlimitedQueueSize, initialThreadCount, finalizeData, NApiFinalizeCallback, noContext, call_js_cb, &function));
}

/**
 * Calls the thread safe function, returns false if unable to call the function
 */
__declspec(noinline) bool ThreadSafeCallback::CallThreadSafeFunction(void* data)
{
    std::lock_guard<std::mutex> lockForUpdate(lockObject);
    if (function == nullptr)
    {
        return false;
    }

    napi_status status = napi_call_threadsafe_function(function, data, napi_threadsafe_function_call_mode::napi_tsfn_blocking);
    return (status == napi_ok);
}

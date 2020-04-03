#include <node_api.h>

#include <memory>
#include <mutex>

/**
 * In order to communciate with JavaScript from a background thread we need to use a
 * napi_threadsafe_function to make the call on the background thread. This structure
 * provides support functions and the management of the lifecyle for working with
 * the napi_threadsafe_function.
 */
struct ThreadSafeCallback
{
    std::mutex lockObject;
    napi_threadsafe_function function = nullptr;

    /**
     * Called when the the ThreadSafeFunction has been finalized
     */
    __declspec(noinline) static void NApiFinalizeCallback(napi_env env, void* finalize_data, void* /*finalize_hint*/);

    /**
     * Called to initialize the thread safe function, when invoked the napi_threadsafe_function_call_js
     * will be called to marshal the arguments for the JavaScript call.
     */
    __declspec(noinline) bool InitializeThreadSafeFunction(napi_env env, const char* javaScriptFunction, napi_threadsafe_function_call_js call_js_cb);

    /**
     * Calls the thread safe function, returns false if unable to call the function
     */
    template<typename TDataType>
    __declspec(noinline) bool CallThreadSafeFunction(std::unique_ptr<TDataType>&& data)
    {
        if (CallThreadSafeFunction(static_cast<void*>(data.get())))
        {
            // the data has been queued and the receiving function will handle deleting it
            (void) data.release();
            return true;
        }

        // failed to call the funciton, when this returns the unique_ptr will delete the memory
        return false;
    }

private:
    /**
     * Calls the thread safe function, returns false if unable to call the function
     */
    __declspec(noinline) bool CallThreadSafeFunction(void* data);
};

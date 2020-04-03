#include "napi_utils.h"

/**
 * Helper function for making a N-Api call, if the call fails the last error
 * and pending exception information will be set so that JavaScript sees
 * the errror.
 */
__declspec(noinline) bool CheckNApiCallResult(napi_env env, napi_status status) noexcept
{
    if (status == napi_ok)
    {
        return true;
    }

    // get the error information
    const napi_extended_error_info* error_info = nullptr;
    napi_get_last_error_info(env, &error_info);

    bool is_exception_pending;
    napi_is_exception_pending(env, &is_exception_pending);
    if (!is_exception_pending)
    {
        // there is not currently an exception pending, we should create one
        const char* message = (error_info->error_message == nullptr)
            ? "empty error message"
            : error_info->error_message;

        napi_throw_error(env, nullptr, message);
    }

    return false;
}

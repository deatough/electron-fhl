#include <node_api.h>

/**
 * Helper function for making a N-Api call, if the call fails the last error
 * and pending exception information will be set.
 */
__declspec(noinline) bool CheckNApiCallResult(napi_env env, napi_status status) noexcept;

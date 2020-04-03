#include "start_clock.h"
#include "napi_utils.h"
#include "thread_safe_callback.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

static ThreadSafeCallback s_ClockCallbackFunction;

/**
 * The background thread that will run and call the specified JavaScript function with
 * the current time of the system.
 */
static void ClockBackgroundThread()
{
    for(;;)
    {
        std::chrono::seconds sleepTime(1);
        std::this_thread::sleep_for(sleepTime);

        std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
        std::time_t currentTimeAsTimeT = std::chrono::system_clock::to_time_t(currentTime);
        std::tm currentTimeAsTM = *std::localtime(&currentTimeAsTimeT);

        std::stringstream timeStream;
        timeStream << currentTimeAsTM.tm_hour << ":";
        timeStream << std::setw(2) << std::setfill('0') << currentTimeAsTM.tm_min << ":";
        timeStream << std::setw(2) << std::setfill('0') << currentTimeAsTM.tm_sec;

        std::unique_ptr<std::string> currentTimeAsString = std::make_unique<std::string>(timeStream.str());
        bool wasCalled = s_ClockCallbackFunction.CallThreadSafeFunction(std::move(currentTimeAsString));
    }
}

/**
 * Callback function for invoking the clock update
 */
static void ClockCallbackFunction(napi_env env, napi_value functionToCall, void* /*contextNotUsed*/, void *data)
{
    std::unique_ptr<std::string> currentTimeAsString(reinterpret_cast<std::string*>(data));
    if (currentTimeAsString != nullptr)
    {
        napi_value argument;
        if (CheckNApiCallResult(env, napi_create_string_utf8(env, currentTimeAsString->c_str(), NAPI_AUTO_LENGTH, &argument)))
        {
            if (functionToCall != nullptr)
            {
                // the function we are calling is on the global object
                napi_value global;
                if (CheckNApiCallResult(env, napi_get_global(env, &global)))
                {
                    napi_value returnNotUsed = nullptr;
                    (void) CheckNApiCallResult(env, napi_call_function(env, global, functionToCall, 1, &argument, &returnNotUsed));
                }
            }
        }
    }
}

/**
 * Starts a background function that will call the specified JavaScript function with the
 * time on a periodic basis.
 */
napi_value StartClock(napi_env env, napi_callback_info info)
{
    constexpr size_t expectedArgs = 1;
    size_t argCount = expectedArgs;
    napi_value arguments[expectedArgs] = { nullptr };
    napi_value thisNotUsed = nullptr;
    void* dataNotUsed = nullptr;
    bool clockStarted = false;

    if (CheckNApiCallResult(env, napi_get_cb_info(env, info, &argCount, arguments, &thisNotUsed, &dataNotUsed)))
    {
        if (argCount >= expectedArgs)
        {
            napi_value functionToCall;
            if (CheckNApiCallResult(env, napi_coerce_to_string(env, arguments[0], &functionToCall)))
            {
                constexpr size_t maxFunctionNameSize = 256;
                char functionName[maxFunctionNameSize];
                functionName[0] = L'\0';
                size_t actualSize = 0;
                if (CheckNApiCallResult(env, napi_get_value_string_utf8(env, functionToCall, functionName, maxFunctionNameSize, &actualSize)))
                {
                    (void) s_ClockCallbackFunction.InitializeThreadSafeFunction(env, functionName, ClockCallbackFunction);
                    std::thread clockThread(ClockBackgroundThread);
                    clockStarted = true;
                }
            }
        }
    }

    // Return whether or not the clock was started
    napi_value result = nullptr;
    if (!CheckNApiCallResult(env, napi_get_boolean(env, clockStarted, &result)))
    {
        return nullptr;
    }

    return result;
}

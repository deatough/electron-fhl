#include <node_api.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

/**
 * Starts a background function that will call the specified JavaScript function with the
 * time on a periodic basis.
 */
napi_value StartClock(napi_env env, napi_callback_info info);

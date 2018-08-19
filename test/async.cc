#include "napi.h"
#include <thread>

using namespace Napi;

void sleepMillis(long millis) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

struct cancel_wrapper_t {
  std::function<void()> cancel;

  static void Cancel(const CallbackInfo& info) {
    auto&& wrapper = static_cast<cancel_wrapper_t*>(info.Data());
    wrapper->cancel();
    
    // called twice would crash
    // remove `delete` would leak -.-
    delete wrapper;
  }
};

Value SleepAndReturnAsync(const CallbackInfo& info) {
  int32_t millis = info[0].ToNumber().Int32Value();

  auto&& deferred = Promise::Deferred::New(info.Env());
  auto&& promise = deferred.Promise();

  auto&& cancel = Async(info.Env(), [millis](auto cancelled) -> std::string {
    sleepMillis(millis);

    if (cancelled()) {
      return "cancelled";
    }

    return "test";
  }, [deferred](Env env, std::string& result) {
    deferred.Resolve(String::New(env, result));
  });

  // how to make Function not leak? where is the Finalizer?
  promise["cancel"] = Function::New(info.Env(), cancel_wrapper_t::Cancel, "cancel", new cancel_wrapper_t{std::move(cancel)});  
  return promise;
}

Value SleepAndReturnSync(const CallbackInfo& info) {
  int32_t millis = info[0].ToNumber().Int32Value();
  sleepMillis(millis);
  return String::New(info.Env(), "test");
}

Object InitAsync(Env env) {
  Object exports = Object::New(env);

  exports["sleepAndReturnAsync"] = Function::New(env, SleepAndReturnAsync);
  exports["sleepAndReturnSync"] = Function::New(env, SleepAndReturnSync);

  return exports;
}

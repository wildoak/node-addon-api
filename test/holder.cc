
#include "napi.h"

using namespace Napi;

void TestCallback(const CallbackInfo& info) {

  HandleScope scope(info.Env());

  Async(info.Env(),
    [](auto&& cancelled) -> std::string {
      return "test";
    },
    // captures must be copyable, thus this would not work with Reference<T>
    [hold_cb = Hold(info[0].As<Function>())](Env env, auto& result) {
      hold_cb->Value().Call({
        String::New(env, result)
      });
    }
  );
}

Value TestSwap(const CallbackInfo& info) {
  EscapableHandleScope scope(info.Env());

  bool valid = true;

  auto&& object = Object::New(info.Env());
  auto&& holder2 = Hold<Object>();

  valid &= !!holder2; // is shared_ptr
  valid &= holder2->operator!(); // holder is empty

  auto&& swap = [env = info.Env(), holder1 = Hold(object), &holder2, &valid]() {
    HandleScope scope(env);

    valid &= !!holder1; // shared_ptr
    valid &= !holder1->operator!(); // not empty

    auto&& object = holder1->Release();
    valid &= holder1->operator!(); // now empty

    holder2->Reset(std::move(object));
    valid &= !holder2->operator!(); // not empty
  };

  swap();

  valid &= !holder2->operator!(); // not empty
  valid &= holder2->Value() == object;

  return scope.Escape(Boolean::New(info.Env(), valid));
}

Object InitHolder(Env env) {
  Object exports = Object::New(env);

  exports["testCallback"] = Function::New(env, TestCallback);
  exports["testSwap"] = Function::New(env, TestSwap);

  return exports;
}

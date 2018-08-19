'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const async_hooks = require('async_hooks');
const common = require('./common');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));


function test(binding) {

  // sync
  {
    const before = Date.now();
    const result = binding.async.sleepAndReturnSync(200);
    assert.ok(Date.now() - before >= 200);
    assert.strictEqual(result, 'test');
  }

  // async
  {
    const before = Date.now();
    binding.async.sleepAndReturnAsync(200).then(common.mustCall(result => {
      assert.ok(Date.now() - before >= 200);
      assert.strictEqual(result, 'test');
    }));
    assert.ok(Date.now() - before < 200);
  }

  // async cancel
  {
    const before = Date.now();
    const promise = binding.async.sleepAndReturnAsync(200)
    promise.then(common.mustNotCall());
    promise.cancel();
    assert.ok(Date.now() - before < 200);
  }
}

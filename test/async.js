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
    const promise = binding.async.sleepAndReturnAsync(400)
    promise.then(common.mustCall(result => {
      // this is correct, if you only run this part with no other tests
      // tests are not running in serial, so it will take a few seconds until main loop events are processed
      // assert.ok(Date.now() - before < 100);

      assert.strictEqual(result, 'cancelled');
    }));

    promise.cancel();

    assert.ok(Date.now() - before < 200);
  }
}

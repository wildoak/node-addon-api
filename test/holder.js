'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const common = require('./common');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {

  binding.holder.testCallback(common.mustCall(value => {
    assert.strictEqual(value, 'test');
  }));

  assert.ok(binding.holder.testSwap());
}

#include <assert.h>
#include <bare.h>
#include <js.h>
#include <portmidi.h>

static js_value_t *
create_int32_result(js_env_t *env, int value) {
  js_value_t *result;
  int err = js_create_int32(env, value, &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_midi_initialize(js_env_t *env, js_callback_info_t *info) {
  return create_int32_result(env, Pm_Initialize());
}

static js_value_t *
bare_midi_terminate(js_env_t *env, js_callback_info_t *info) {
  Pm_Terminate();
  return create_int32_result(env, 0);
}

static js_value_t *
bare_midi_count_devices(js_env_t *env, js_callback_info_t *info) {
  return create_int32_result(env, Pm_CountDevices());
}

static js_value_t *
bare_midi_exports(js_env_t *env, js_value_t *exports) {
  int err;
  js_value_t *val;

  err = js_create_function(env, "initialize", -1, bare_midi_initialize, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "initialize", val);
  assert(err == 0);

  err = js_create_function(env, "terminate", -1, bare_midi_terminate, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "terminate", val);
  assert(err == 0);

  err = js_create_function(env, "countDevices", -1, bare_midi_count_devices, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "countDevices", val);
  assert(err == 0);

  return exports;
}

BARE_MODULE(bare_midi, bare_midi_exports)

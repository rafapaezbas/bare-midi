#include <assert.h>
#include <bare.h>
#include <js.h>
#include <portmidi.h>

static js_value_t *
bare_midi_initialize(js_env_t *env, js_callback_info_t *info) {
  js_value_t *result;
  int err = js_create_int32(env, Pm_Initialize(), &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_midi_terminate(js_env_t *env, js_callback_info_t *info) {
  Pm_Terminate();
  js_value_t *result;
  int err = js_create_int32(env, 0, &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_midi_count_devices(js_env_t *env, js_callback_info_t *info) {
  js_value_t *result;
  int err = js_create_int32(env, Pm_CountDevices(), &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_midi_get_device_info(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  uint32_t index;
  err = js_get_value_uint32(env, argv[0], &index);
  assert(err == 0);

  const PmDeviceInfo *device_info = Pm_GetDeviceInfo(index);

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

  js_value_t *val;

  err = js_create_string_utf8(env, (utf8_t *) device_info->interf, -1, &val);
  assert(err == 0);
  err = js_set_named_property(env, result, "interface", val);
  assert(err == 0);

  err = js_create_string_utf8(env, (utf8_t *) device_info->name, -1, &val);
  assert(err == 0);
  err = js_set_named_property(env, result, "name", val);
  assert(err == 0);

  err = js_create_int32(env, device_info->input, &val);
  assert(err == 0);
  err = js_set_named_property(env, result, "input", val);
  assert(err == 0);

  err = js_create_int32(env, device_info->output, &val);
  assert(err == 0);
  err = js_set_named_property(env, result, "output", val);
  assert(err == 0);

  err = js_create_int32(env, device_info->opened, &val);
  assert(err == 0);
  err = js_set_named_property(env, result, "opened", val);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_midi_open_output(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  uint32_t index;
  err = js_get_value_uint32(env, argv[0], &index);
  assert(err == 0);

  PmStream *stream;
  Pm_OpenOutput(&stream, index, NULL, 512, NULL, NULL, 0);

  js_value_t *obj;
  err = js_create_object(env, &obj);
  assert(err == 0);

  uintptr_t addr = (uintptr_t) stream;
  js_value_t *result;
  err = js_create_bigint_uint64(env, addr, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_midi_write_sys_ex(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  uintptr_t stream;
  bool lossless = true;
  err = js_get_value_bigint_uint64(env, argv[0], &stream, &lossless);
  assert(err == 0);

  uint32_t note;
  err = js_get_value_uint32(env, argv[1], &note);
  assert(err == 0);

  uint8_t msg[] = {240, 0, 32, 41, 2, 24, 10, note, 50, 247}; // TODO move this
  Pm_WriteSysEx((PmStream *) stream, 0, msg);

  js_value_t *result;
  err = js_create_int32(env, 1, &result);
  assert(err == 0);

  return result;
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

  err = js_create_function(env, "getDeviceInfo", -1, bare_midi_get_device_info, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "getDeviceInfo", val);
  assert(err == 0);

  err = js_create_function(env, "openOutput", -1, bare_midi_open_output, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "openOutput", val);
  assert(err == 0);

  err = js_create_function(env, "writeSysEx", -1, bare_midi_write_sys_ex, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "writeSysEx", val);
  assert(err == 0);

  return exports;
}

BARE_MODULE(bare_midi, bare_midi_exports)

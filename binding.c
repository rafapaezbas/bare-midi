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
bare_midi_open_input(js_env_t *env, js_callback_info_t *info) {
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
  Pm_OpenInput(&stream, index, NULL, 512, NULL, NULL);

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
bare_midi_read_message(js_env_t *env, js_callback_info_t *info) {
  int err;
  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  uintptr_t stream;
  bool lossless = true;
  err = js_get_value_bigint_uint64(env, argv[0], &stream, &lossless);
  assert(err == 0);

  PmEvent events[64];
  int32_t count = Pm_Read((PmStream *) stream, events, 64); // Read 64 events max

  if (count < 0) {
    err = js_throw_error(env, NULL, Pm_GetErrorText((PmError) count));
    assert(err == 0);
    return NULL;
  }

  js_value_t *result;
  err = js_create_array_with_length(env, count * 4, &result);
  assert(err == 0);

  for (int32_t i = 0; i < count; i++) {
    uint32_t m = (uint32_t) events[i].message;

    for (uint32_t j = 0; j < 4; j++) {
      js_value_t *v;
      err = js_create_uint32(env, (m >> (j * 8)) & 0xff, &v);
      assert(err == 0);
      err = js_set_element(env, result, i * 4 + j, v);
      assert(err == 0);
    }
  }

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

  uint32_t msg_len;
  err = js_get_array_length(env, argv[1], &msg_len);
  assert(err == 0);

  uint8_t msg[msg_len];
  for (uint32_t i = 0; i < msg_len; i++) {
    js_value_t *value;
    err = js_get_element(env, argv[1], i, &value);
    assert(err == 0);

    uint32_t byte;
    err = js_get_value_uint32(env, value, &byte);
    assert(err == 0);

    msg[i] = (uint8_t) byte;
  }

  Pm_WriteSysEx((PmStream *) stream, 0, msg);

  js_value_t *result;
  err = js_create_int32(env, 1, &result);
  assert(err == 0);

  return result;
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

  err = js_create_function(env, "getDeviceInfo", -1, bare_midi_get_device_info, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "getDeviceInfo", val);
  assert(err == 0);

  err = js_create_function(env, "openOutput", -1, bare_midi_open_output, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "openOutput", val);
  assert(err == 0);

  err = js_create_function(env, "openInput", -1, bare_midi_open_input, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "openInput", val);
  assert(err == 0);

  err = js_create_function(env, "readMessage", -1, bare_midi_read_message, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "readMessage", val);
  assert(err == 0);

  err = js_create_function(env, "writeSysEx", -1, bare_midi_write_sys_ex, NULL, &val);
  assert(err == 0);
  err = js_set_named_property(env, exports, "writeSysEx", val);
  assert(err == 0);

  return exports;
}

BARE_MODULE(bare_midi, bare_midi_exports)

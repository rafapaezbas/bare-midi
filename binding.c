#include <assert.h>
#include <bare.h>
#include <js.h>
#include <portmidi.h>

static js_value_t *
bare_midi_initialize(js_env_t *env, js_callback_info_t *info) {
    int err;

    uv_loop_t *loop;
    err = js_get_env_loop(env, &loop);
    assert(err == 0);

    js_value_t *result;
    err = js_create_int32(env, Pm_Initialize(), &result);
    assert(err == 0);

    return result;
}

static js_value_t *
bare_midi_exports(js_env_t *env, js_value_t *exports) {
    int err;
    {
        js_value_t *val;
        err = js_create_function(env, "initialize", -1, bare_midi_initialize, NULL, &val);
        assert(err == 0);
        err = js_set_named_property(env, exports, "initialize", val);
        assert(err == 0);
    }
    return exports;
}

BARE_MODULE(bare_midi, bare_midi_exports)

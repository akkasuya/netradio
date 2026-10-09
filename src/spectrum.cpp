// =====================================================
//  I2S PCM TAP for the spectrum visualizer.
//
//  IMPORTANT: this file must NOT include Audio.h — that
//  header declares audio_process_i2s() as __weak__, and
//  any definition in a TU that saw the weak declaration
//  inherits weak linkage (override would silently fail).
//  Declared manually with the exact same signature so
//  this STRONG definition replaces the library default.
//
//  Runs in the audio task AFTER volume/gain/EQ. Keeps
//  it lock-free and fast: no Serial/LVGL/alloc in here.
//  Buffer is stereo int32 interleaved (L/R frames).
// =====================================================
#include "spectrum.h"
#include <string.h>

static int32_t spec_window[SPEC_N];
static volatile uint16_t spec_count = 0;
static volatile bool spec_ready = false;

void audio_process_i2s(int32_t* outBuff, int16_t validSamples, bool* continueI2S) {
    (void)continueI2S;
    if (spec_ready) return; // previous window not consumed yet — skip this chunk
    for (int i = 0; i < validSamples && spec_count < SPEC_N; i++) {
        int32_t mono = (outBuff[i * 2] >> 1) + (outBuff[i * 2 + 1] >> 1); // stereo -> mono
        spec_window[spec_count++] = mono;
        if (spec_count >= SPEC_N) { spec_ready = true; break; }
    }
}

bool spectrum_fetch(int32_t* dest) {
    if (!spec_ready) return false;
    memcpy(dest, spec_window, sizeof(spec_window)); // 1 KB copy; a torn sample is invisible
    spec_count = 0;
    spec_ready = false;
    return true;
}

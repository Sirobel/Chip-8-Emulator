//
// Created by finns on 21.11.26.
//

#ifndef CHIP_8_EMULATOR_BEEPER_H
#define CHIP_8_EMULATOR_BEEPER_H

#pragma once
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <atomic>
#include <cstdint>


class Beeper {
public:
    bool init() {
        SDL_AudioSpec spec{};
        spec.format = SDL_AUDIO_S16;
        spec.channels = 1;
        spec.freq = SAMPLE_RATE;

        stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, callback, this);
        if (!stream) {
            SDL_Log("Audio init failed: %s", SDL_GetError());
            return false;
        }

        SDL_ResumeAudioStreamDevice(stream);
        return true;
    }

    void set(const bool on) {
        beeping = on;
    }

    void shutdown() {
        if (stream) {
            SDL_DestroyAudioStream(stream);
        }
        stream = nullptr;
    }

private:
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr float FREQ = 440.0f;
    static constexpr int16_t AMPLITUDE = 6000;

    SDL_AudioStream *stream = nullptr;
    std::atomic<bool> beeping{false};
    float phase = 0.0f;
    float gain = 0.0f;

    static void SDLCALL callback(void *userdata, SDL_AudioStream *s, int additional, int total) {
        auto *self = static_cast<Beeper *>(userdata);
        int samples = additional / static_cast<int>(sizeof(int16_t));

        while (samples > 0) {
            int16_t buf[512];
            const int n = std::min(samples, 512);
            const float target = self->beeping ? 1.0f : 0.0f;

            for (int i = 0; i < n; i++) {
                if (self->gain < target) {
                    self->gain = std::min(target, self->gain+0.002f);
                } else if (self->gain > target) {
                    self->gain = std::max(target, self->gain-0.002f);
                }

                self->phase += FREQ/SAMPLE_RATE;
                if (self->phase >= 1.0f) {
                    self->phase -= 1.0f;
                }

                const float square = self->phase < 0.5f ? 1.0f: -1.0f;
                buf[i] = static_cast<int16_t>(square * self->gain*AMPLITUDE);
            }
            SDL_PutAudioStreamData(s,buf,n*static_cast<int16_t>(sizeof(int16_t)));
            samples -=n;
        }
    }
};


#endif //CHIP_8_EMULATOR_BEEPER_H

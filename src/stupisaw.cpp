//
// Created by Paul Walker on 7/17/21.
//

#include "stupisaw.h"
#include <iostream>
#include <cmath>

namespace BaconPaul
{

StupiSaw::StupiSaw(const clap_host *host)
    : clap::Plugin(&desc, host)
{
    std::cout << "Creating a stupisaw" << std::endl;
}

clap_plugin_descriptor StupiSaw::desc = {
    CLAP_VERSION,
    "org.baconpaul.stupisaw",
    "StupiSaw",
    "BaconPaul",
    "https://baconpaul.org",
    "https://baconpaul.org/no-manual",
    "https://baconpaul.org/no-support",
    "0.9.0",
    "It's called StupiSaw. What do you expect?",
    "aliasing;example-code;dont-use",
    CLAP_PLUGIN_INSTRUMENT
};


bool StupiSaw::implementsParams() const noexcept { return true; }
uint32_t StupiSaw::paramsCount() const noexcept { return nParams; }
bool StupiSaw::isValidParamId(clap_id paramId) const noexcept
{
    // This is obviously a bit odd
    auto res = (paramId == pmUnisonCount || paramId == pmUnisonSpread || paramId == pmCutoff ||
                paramId == pmResonance);
    return res;
}
bool StupiSaw::paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept
{
    if (paramIndex >= nParams) return false;
    info->module[0] = 0;
    switch( paramIndex )
    {
    case 0:
        info->id = pmUnisonCount;
        strncpy(info->name, "Unison Count", CLAP_NAME_SIZE);
        info->min_value = 1;
        info->max_value = 7;
        info->default_value = 3;
        break;
    case 1:
        info->id = pmUnisonSpread;
        strncpy(info->name, "Unison Spread in Cents", CLAP_NAME_SIZE);
        info->min_value = 0;
        info->max_value = 100;
        info->default_value = 10;
        break;
    case 2:
        info->id = pmCutoff;
        strncpy(info->name, "Cutoff in Keys", CLAP_NAME_SIZE);
        info->min_value = 1;
        info->max_value = 127;
        info->default_value = 69;
        break;
    case 3:
        info->id = pmResonance;
        strncpy(info->name, "Resonance", CLAP_NAME_SIZE);
        info->min_value = 0.0;
        info->max_value = 1.0;
        info->default_value = 0.7;
        break;
    }
    return true;
}
bool StupiSaw::paramsValue(clap_id paramId, double *value) noexcept
{
    std::cout << "paramsValue " << paramId << std::endl;
    switch (paramId)
    {
    case pmUnisonCount:
        *value = unisonCount;
        break;
    case pmUnisonSpread:
        *value = unisonSpread;
        break;
    case pmCutoff:
        *value = cutoff;
        break;
    case pmResonance:
        *value = resonance;
        break;
    }

    return true;
}

clap_process_status StupiSaw::process(const clap_process *process) noexcept
{
    auto ev = process->in_events;
    auto sz = ev->size(ev);

    if (sz != 0)
    {
        std::cout << "SIZE is " << sz << std::endl;
        for (auto i=0; i<sz; ++i)
        {
            auto evt = ev->get(ev, i);
            switch (evt->type)
            {
            case CLAP_EVENT_NOTE_ON:
            {
                auto n = evt->note;
                std::cout << "NOte On " << n.key << " " << n.velocity << std::endl;
            }
                break;
            case CLAP_EVENT_NOTE_OFF:
            {
                auto n = evt->note;
                std::cout << "Note Off " << n.key << " " << n.velocity << std::endl;
            }
                break;
            case CLAP_EVENT_PARAM_VALUE:
            {
                auto v = evt->param_value;
                std::cout << "Param Value " << v.param_id << " " << v.value << std::endl;
            }
                break;
            }
        }
    }

    float **out = process->audio_outputs[0].data32;
    float rate = 440.0 / 44100 ;
    for (int i=0; i<process->frames_count; ++i)
    {
        for (int c = 0; c < 2; ++c)
        {
            out[c][i] = 0.01 * sin(2.0 * 3.14159265 * phase);
        }
        phase += rate;
    }
    if (phase > 1) phase -= 1;
    return CLAP_PROCESS_CONTINUE;
}
bool StupiSaw::startProcessing() noexcept { return Plugin::startProcessing(); }
uint32_t StupiSaw::audioPortsCount(bool isInput) const noexcept
{
    return isInput ? 0 : 1;
}
bool StupiSaw::audioPortsInfo(uint32_t index, bool isInput,
                              clap_audio_port_info *info) const noexcept
{
    if (isInput || index != 0)
        return false;

    info->id = 0;
    strncpy(info->name, "main", sizeof(info->name));
    info->is_main = true;
    info->is_cv = false;
    info->sample_size = 32;
    info->in_place = true;
    info->channel_count = 2;
    info->channel_map = CLAP_CHMAP_STEREO;
    return true;
}

}
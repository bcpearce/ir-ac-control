#include "lirc_payload.h"

LircPayload LircPayload::Decode(uint32_t val) {
    return LircPayload(static_cast<LircMode2>(LIRC_MODE2(val)), LIRC_VALUE(val));
}

LircPayload::LircPayload(LircMode2 mode, int value) : mode_(mode) {
    switch (mode_) {
    case LircMode2::Pulse:
    case LircMode2::Space:
        pulseWidth_ = value;
        break;
    case LircMode2::Frequency:
        frequency_ = value;
        break;
    case LircMode2::Timeout:
        timeout_ = value;
        break;
    }
}

std::ostream& operator<<(std::ostream& _stream, const LircPayload& _lp) {
    switch (_lp.mode_) {
    case LircMode2::Pulse:
        _stream << "Pulse: " << _lp.pulseWidth_ << "us";
        break;
    case LircMode2::Space:
        _stream << "Space: " << _lp.pulseWidth_ << "us";
        break;
    case LircMode2::Frequency:
        _stream << "Frequency: " << _lp.frequency_ << "us";
        break;
    case LircMode2::Timeout:
        _stream << "Timeout: " << _lp.frequency_ << "us";
        break;
    }
    return _stream;
}
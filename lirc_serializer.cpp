#include "lirc_serializer.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "lirc_payload.h"

bool PulseDistanceEncoding::IsFrameStart(LircPayload a, LircPayload b) {
    return 
        a.GetMode() == LircMode2::Pulse && 
        b.GetMode() == LircMode2::Space &&
        (a.GetPulseWidth() >= frameStart.pulse - tolerance) &&
        (a.GetPulseWidth() <= frameStart.pulse + tolerance) &&
        (b.GetPulseWidth() >= frameStart.space - tolerance) &&
        (b.GetPulseWidth() <= frameStart.space + tolerance);
}

bool PulseDistanceEncoding::IsBitOne(LircPayload a, LircPayload b) {
    return 
        a.GetMode() == LircMode2::Pulse && 
        b.GetMode() == LircMode2::Space &&
        (a.GetPulseWidth() >= bitOne.pulse - tolerance) &&
        (a.GetPulseWidth() <= bitOne.pulse + tolerance) &&
        (b.GetPulseWidth() >= bitOne.space - tolerance) &&
        (b.GetPulseWidth() <= bitOne.space + tolerance);
}

bool PulseDistanceEncoding::IsBitZero(LircPayload a, LircPayload b) {
    return 
        a.GetMode() == LircMode2::Pulse && 
        b.GetMode() == LircMode2::Space &&
        (a.GetPulseWidth() >= bitZero.pulse - tolerance) &&
        (a.GetPulseWidth() <= bitZero.pulse + tolerance) &&
        (b.GetPulseWidth() >= bitZero.space - tolerance) &&
        (b.GetPulseWidth() <= bitZero.space + tolerance);
}

bool PulseDistanceEncoding::IsEndRx(LircPayload a, LircPayload b) {
    return a.GetMode() == LircMode2:Pulse && b.GetMode(LircMode2::Timeout);
}

LircSerializer::LircSerializer(const PulseDistanceEncoding& enc) 
    : encoder_(enc) {}

void LircSerializer::Add(const LircPayload& payload) {
    switch(payload.GetMode()) {
    case LircMode2::Frequency:
        throw std::invalid_argument("Frequency not supported.");
    case LircMode2::Timeout:
        return;
    case LircMode2::Pulse:
    case LircMode2::Space:
        if (pPendingBit_) {
            // check for one of the various bit types
            if (encoder_.IsFrameStart(*pPendingBit_, payload)) {
                packets_.push_back({});
                ResetPendingByte();
            } else if (packets_.size() > 0) {
                if (encoder_.IsBitZero(*pPendingBit_, payload)) {
                    bitsBuffered_++;
                } else if (encoder_.IsBitOne(*pPendingBit_, payload)) {
                    pendingByte_ |= 0b1 << (bitsBuffered_++);
                }
                // if we have a full byte, shift it in
                if (bitsBuffered_ == 8) {
                    packets_.back().push_back(pendingByte_);
                    ResetPendingByte();
                }   
            }
        }
        pPendingBit_ = std::make_unique<LircPayload>(payload);
        break;
    }
}

void LircSerializer::Clear() {
    packets_.clear(); 
    ResetPendingByte();
    pPendingBit_.reset();
}

std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls) {
    _stream << " | ";
    for(const auto& packet : _ls.packets_) {
        for(const auto _c : packet) {
            _stream << std::hex << std::setfill('0') << std::setw(2) << +_c << " ";
        }
        _stream << "| ";
    }
    return _stream;
}

void LircSerializer::ResetPendingByte() {
    pendingByte_ = 0u;
    bitsBuffered_ = 0u;
}

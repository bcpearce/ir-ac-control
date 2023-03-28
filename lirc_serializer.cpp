#include "lirc_serializer.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "lirc_payload.h"
#include "util.h"

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
    return a.GetMode() == LircMode2::Pulse && b.GetMode() == LircMode2::Timeout;
}

LircSerializer::LircSerializer(const PulseDistanceEncoding& enc) 
    : encoder_(enc) {}

void LircSerializer::Add(const LircPayload& payload) {
    switch(payload.GetMode()) {
    case LircMode2::Frequency:
        throw std::invalid_argument("Frequency not supported.");
    case LircMode2::Timeout:
    case LircMode2::Pulse:
    case LircMode2::Space:
        if (pPendingBit_) {
            // check for one of the various bit types
            if (encoder_.IsEndRx(*pPendingBit_, payload)) {
                received_ = true;
                ResetPendingByte();
            } else if (encoder_.IsFrameStart(*pPendingBit_, payload)) {
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

std::vector<std::vector<uint8_t>> LircSerializer::WaitForRx(const char* lircDev) {
    Clear();
    std::array<uint32_t, 256> buf;
    int fd{0};
    try {
        fd = SYSTEM_WRAPPER(open, lircDev, O_RDONLY);
        {
            uint32_t features{0};
            SYSTEM_WRAPPER(ioctl, fd, LIRC_GET_FEATURES, &features);
            if (!(features & LIRC_CAN_REC_MODE2)) {
                throw std::runtime_error("Device does not support LIRC Mode2");
            }
        }
        for (;;) {
            ssize_t ct = SYSTEM_WRAPPER(read, fd, buf.data(), sizeof(uint32_t) * buf.size());
            
            for(ssize_t i = 0; i < ct / sizeof(uint32_t); ++i) {
                auto pl = LircPayload::Decode(buf.at(i));
                this->Add(pl);
                if (received_) {
                    return packets_;
                }
            }
        }
    } 
    catch (...) {
        close(fd);
        throw;
    }
    throw std::runtime_error(std::string{"Unexpected exit from "} + std::string{__func__});
}

void LircSerializer::Clear() {
    packets_.clear(); 
    ResetPendingByte();
    pPendingBit_.reset();
    received_ = false;
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

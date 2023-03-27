#include "lirc_serializer.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "lirc_payload.h"

LircSerializer::LircSerializer(const std::vector<PulseWidthId>& pulseIds) 
    : pulseIds_(pulseIds) {
        if (pulseIds_.size() > 127) {
            throw std::invalid_argument("Too many potential IDs");
        }
    }

bool LircSerializer::Add(const LircPayload& payload) {

    switch(payload.GetMode()) {
    case LircMode2::Frequency:
        throw std::invalid_argument("Frequency not supported.");
    case LircMode2::Timeout:
        isComplete_ = true; // mark it ready
    }

    if (isComplete_) {
        return isComplete_;
    }

    auto it = std::find_if(pulseIds_.cbegin(), pulseIds_.cend(), 
        [pw = payload.GetPulseWidth()](const auto& pulseId) {
            return (pw >= pulseId.min && pw <= pulseId.max);
        });

    if (it != pulseIds_.cend()) {
        serialized_ << static_cast<unsigned char>(
            char(std::distance(pulseIds_.cbegin(), it)) + 
                (payload.GetMode() == LircMode2::Space ? 0 : 127));
    }
    else {
        serialized_ << static_cast<unsigned char>(0xFF);
    }
    return isComplete_;
}

std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls) {
    if (_ls.isComplete_) {
        int i = 0;
        for(auto c : _ls.serialized_.str()) {
            if (i++ % 20 == 0) {
                _stream << "   -" << std::dec << (i-1) << " - \n";
            }

            switch(c) {
            case 0x81:
                _stream << "\033[1;31m";
                break;
            case 0x7f:
                _stream << "\033[1;32m";
                break;
            case 0x00:
                _stream << "\033[1;33m";
                break;
            case 0x01:
                _stream << "\033[1;36m";
                break;
            }
            _stream << std::setfill('0') << std::setw(2) << std::hex;
            _stream << +c << " " << "\033[0m";
        }
    }
    return _stream;
}

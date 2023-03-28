#include "lirc_daikin.h"
#include <cmath>
#include <iomanip>

LircDaikin::LircDaikin(const std::vector<std::vector<uint8_t>>& frames) {
    if (!ValidateHeader(frames)) return;
    if (!ValidatePayload(frames)) return;

    validated_ = true;

    mode_ = static_cast<daikin::Mode>(frames.at(1).at(5) & 0xF0);
    temperatureF_ = CalcTemp(frames.at(1).at(6));
    fan_ = static_cast<daikin::Fan>(frames.at(1).at(8) & 0xF0);
    swingHorizontalOn_ = bool(static_cast<daikin::SwingHorizontal>(frames.at(1).at(9) & 0x0F));
    swingVerticalOn_ = bool(static_cast<daikin::SwingVertical>(frames.at(1).at(8) & 0x0F));
    powerOn_ = bool(static_cast<daikin::OnOff>(frames.at(1).at(5) & 0x01));
    followMe_ = bool(static_cast<daikin::FollowMe>(frames.at(1).at(16) & 0x0F));
    quiet_ = bool(static_cast<daikin::Quiet>(frames.at(1).at(13) & 0xF0));

    header_ = frames.at(1);
    raw_ = frames.at(1);
}

bool LircDaikin::ValidateHeader(const std::vector<std::vector<uint8_t>>& frames) {
    if (frames.size() != 2) return false;
    static const std::vector<uint8_t> expVec = {0x11, 0xda, 0x27, 0xf0, 0x00, 0x00, 0x00, 0x02};
    return frames.at(0) == expVec;
}

bool LircDaikin::ValidatePayload(const std::vector<std::vector<uint8_t>>& frames) {
    if (frames.size() != 2) return false;
    if (frames.at(1).size() != 19) return false;
    static const std::vector<uint8_t> expVec = {0x11, 0xda, 0x27, 0x00, 0x00};
    for(ssize_t i = 0; i < expVec.size(); ++i) {
        if (frames.at(1).at(i) != expVec.at(i)) return false;
    }
    return true;
}

uint8_t LircDaikin::CalcTemp(uint8_t rawTemp) {
    uint8_t addTo = uint8_t(std::lround((float(rawTemp) - 20.0f) * 0.9));
    return 50 + addTo;
}

std::ostream& operator<<(std::ostream& _stream, LircDaikin& _ld) {
    if (!_ld.validated_) {
        return _stream << "Not Valid DAIKIN Message";
    }
    _stream << std::dec 
    << "Mode: " << 
        ( _ld.mode_ == daikin::Mode::Auto ? "Auto" 
        : _ld.mode_ == daikin::Mode::Dry ? "Dry" 
        : _ld.mode_ == daikin::Mode::Cold ? "Cold" 
        : _ld.mode_ == daikin::Mode::Heat ? "Heat" 
        : _ld.mode_ == daikin::Mode::Fan ? "Fan" : "")
    << "\nFan: " <<
        ( _ld.fan_ == daikin::Fan::Auto ? "Auto" 
        : _ld.fan_ == daikin::Fan::Silent ? "Silent" 
        : _ld.fan_ == daikin::Fan::_1 ? "1" 
        : _ld.fan_ == daikin::Fan::_2 ? "2" 
        : _ld.fan_ == daikin::Fan::_3 ? "3" 
        : _ld.fan_ == daikin::Fan::_4 ? "4" 
        : _ld.fan_ == daikin::Fan::_5 ? "5" : "")
    << "\nTemperature Deg F: " <<
        + _ld.temperatureF_
    << "\nSwing Vertical: " <<
        ( _ld.swingVerticalOn_ ? "On" : "Off")
    << "\nSwing Horizontal: " <<
        ( _ld.swingHorizontalOn_ ? "On" : "Off")
    << "\nPowerful: " <<
        ( _ld.powerfulOn_ ? "On" : "Off")
    << "\nPower: " << 
        ( _ld.powerOn_ ? "On" : "Off")
    << "\nFollow Me: " <<
        ( _ld.followMe_ ? "On" : "Off")
    << "\nQuiet: " <<
        ( _ld.quiet_ ? "On" : "Off")
    << "\n |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 |"
    << "\n | ";
    for(auto c : _ld.raw_) {
        _stream << std::hex << std::setw(2) << std::setfill('0') << +c << " ";
    }
    _stream << "|\n";
    return _stream;
}
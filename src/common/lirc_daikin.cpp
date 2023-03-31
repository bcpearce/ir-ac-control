#include "lirc_daikin.h"

#include <cmath>
#include <iomanip>
#include <numeric>

namespace {
    static const std::vector<uint8_t> expectedHeader = {0x11, 0xda, 0x27, 0xf0, 0x00, 0x00, 0x00, 0x02};
    static const std::vector<uint8_t> expectedPayloadHeader = {0x11, 0xda, 0x27, 0x00, 0x00};
}

LircDaikin LircDaikin::Decode(const std::vector<std::vector<uint8_t>>& frames) {
    if (!frames.size() == 2) throw std::invalid_argument("Must be 2 frames.");

    const auto& header = frames.at(0);
    const auto& payload = frames.at(1);

    if (!ValidateHeader(header)) throw std::invalid_argument("Header packet was not valid.");
    if (!ValidatePayload(payload)) throw std::invalid_argument("Payload packet was not valid.");
    if (!ValidateChecksum(payload)) throw std::invalid_argument("Checksum was not valid.");

    LircDaikin ld;

    ld.validated_ = true;

    ld.mode_ = static_cast<daikin::Mode>(payload.at(5) & 0xF0);
    ld.temperatureF_ = CalcTemp(payload.at(6));
    ld.fan_ = static_cast<daikin::Fan>(payload.at(8) & 0xF0);
    ld.swingHorizontalOn_ = bool(static_cast<daikin::SwingHorizontal>(payload.at(9) & 0x0F));
    ld.swingVerticalOn_ = bool(static_cast<daikin::SwingVertical>(payload.at(8) & 0x0F));
    ld.powerOn_ = bool(static_cast<daikin::OnOff>(payload.at(5) & 0x01));
    ld.followMe_ = bool(static_cast<daikin::FollowMe>(payload.at(16) & 0x0F));
    ld.quiet_ = bool(static_cast<daikin::Quiet>(payload.at(13) & 0xF0));

    ld.header_ = header;
    ld.raw_ = payload;

    return ld;
}

std::vector<std::vector<uint8_t>> LircDaikin::Encode() const {
    std::vector<std::vector<uint8_t>> res;
    res.push_back(expectedHeader);
    std::vector<uint8_t> payload(19);

    std::copy(expectedPayloadHeader.begin(), expectedPayloadHeader.end(), payload.begin());

    payload.at(5) = uint8_t(mode_) | uint8_t(powerOn_);
    payload.at(6) = CalcTempForSend();
    payload.at(7) = 0;
    payload.at(8) = uint8_t(swingVerticalOn_ ? daikin::SwingVertical::On : daikin::SwingVertical::Off) | uint8_t(fan_);
    payload.at(9) = uint8_t(swingHorizontalOn_ ? daikin::SwingHorizontal::On : daikin::SwingHorizontal::Off);
    payload.at(10) = 0;
    payload.at(11) = 0;
    payload.at(12) = 0;
    payload.at(13) = uint8_t(powerfulOn_ ? daikin::Powerful::On : daikin::Powerful::Off);
    payload.at(14) = 0;
    payload.at(15) = 0xc0;
    payload.at(16) = uint8_t(followMe_);
    payload.at(17) = 0;
    payload.at(18) = CalculateChecksum(payload);

    res.push_back(std::move(payload));

    return res;
}

bool LircDaikin::Validate() {
    return validated_ = (
        ValidateHeader(header_) && 
        ValidatePayload(raw_) && 
        ValidateChecksum(raw_));
}

bool LircDaikin::ValidateHeader(const std::vector<uint8_t>& header) {
    return header == expectedHeader;
}

bool LircDaikin::ValidatePayload(const std::vector<uint8_t>& payload) {
    if (payload.size() != 19) return false;
    return std::equal(expectedPayloadHeader.begin(), expectedPayloadHeader.end(), payload.begin());
}

uint8_t LircDaikin::CalculateChecksum(const std::vector<uint8_t>& payload) {
    if (payload.size() != 19) return false;
    uint32_t totalSum = std::accumulate(payload.begin(), payload.end() - 1, 0u);
    return totalSum & 0xFF;
}

bool LircDaikin::ValidateChecksum(const std::vector<uint8_t>& payload) {
    return CalculateChecksum(payload) == payload.back();
}

uint8_t LircDaikin::CalcTemp(uint8_t rawTemp) {
    return uint8_t(std::lround((float(rawTemp) - 20.0f) * 0.9)) + 50u;
}

uint8_t LircDaikin::CalcTempForSend() const {
    return uint8_t((temperatureF_ - 50.0f) / 0.9f + 20.0f);
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
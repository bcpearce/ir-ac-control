#ifndef _LIRC_DAIKIN_H
#define _LIRC_DAIKIN_H

#include <array>
#include <ostream>
#include <vector>

namespace daikin {

enum class Mode : uint8_t {
    Auto = 0x00,
    Dry = 0x20,
    Cold = 0x30,
    Heat = 0x40,
    Fan = 0x60
};

enum class OnOff : uint8_t{
    On = 0x1,
    Off = 0x0
};

enum class Fan : uint8_t {
    Auto = 0xA0,
    Silent = 0xB0,
    _1 = 0x30,
    _2 = 0x40,
    _3 = 0x50,
    _4 = 0x60,
    _5 = 0x70
};

enum class SwingHorizontal : uint8_t {
    On = 0xF,
    Off = 0x0
};

enum class SwingVertical : uint8_t {
    On = 0xF,
    Off = 0x0
};

enum class Powerful : uint8_t {
    On = 0x1,
    Off = 0x0
};

enum class FollowMe : uint8_t {
    On = 0x2,
    Off = 0x0
};

enum class Quiet : uint8_t {
    On = 0x20,
    Off = 0x0
};

}

class LircDaikin {
public:
    LircDaikin() = default;
    static LircDaikin Decode(const std::vector<std::vector<uint8_t>>& frames);
    std::vector<std::vector<uint8_t>> Encode() const;

    uint8_t temperatureF_{0};
    daikin::Mode mode_ = daikin::Mode::Auto;
    daikin::Fan fan_ = daikin::Fan::Auto;
    bool swingHorizontalOn_{false};
    bool swingVerticalOn_{false};
    bool powerfulOn_{false};
    bool powerOn_{false};
    bool followMe_{false};
    bool quiet_{false};

    friend std::ostream&  operator<<(std::ostream& _stream, LircDaikin& _ld);

    bool Validate();

    uint8_t GetTemperatureF() const { return temperatureF_; }
    void SetTemperatureF(uint8_t temperatureF) { temperatureF_ = temperatureF; };

    daikin::Mode GetMode() const { return mode_; }
    void SetMode(daikin::Mode mode) { mode_ = mode; }

private:

    static bool ValidateHeader(const std::vector<uint8_t>& frames);
    static bool ValidatePayload(const std::vector<uint8_t>& frames);
    static uint8_t CalculateChecksum(const std::vector<uint8_t>& payload);
    static bool ValidateChecksum(const std::vector<uint8_t>& payload);
    static uint8_t CalcTemp(uint8_t rawTemp);
    uint8_t CalcTempForSend() const;

    std::vector<uint8_t> header_;
    std::vector<uint8_t> raw_;
    bool validated_{false};
};

std::ostream& operator<<(std::ostream& _stream, LircDaikin& _ld);

#endif
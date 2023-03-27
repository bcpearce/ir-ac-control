#ifndef _LIRC_PAYLOAD_H
#define _LIRC_PAYLOAD_H

#include <ostream>
#include <linux/lirc.h>

enum class LircMode2 {
    Pulse = LIRC_MODE2_PULSE,
    Space = LIRC_MODE2_SPACE,
    Frequency = LIRC_MODE2_FREQUENCY,
    Timeout = LIRC_MODE2_TIMEOUT
};

class LircPayload {
public:
    static LircPayload Decode(uint32_t val);

    int GetPulseWidth() const {
        return pulseWidth_;
    }
    int GetFrequency() const {
        return frequency_;
    }
    int GetTimeout() const {
        return timeout_;
    }
    LircMode2 GetMode() const {
        return mode_;
    }

    friend std::ostream& operator<<(std::ostream& _stream, const LircPayload& _lp);

private:
    LircPayload() = default;
    LircPayload(LircMode2 mode, int value);
    LircMode2 mode_{LircMode2::Space};
    int pulseWidth_{-1};
    int frequency_{-1};
    int timeout_{-1};
};

std::ostream& operator<<(std::ostream& _stream, const LircPayload& _lp);

#endif
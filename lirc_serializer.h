#ifndef _LIRC_SERIALIZER_H
#define _LIRC_SERIALIZER_H

#include <bitset>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

class LircPayload;

struct PulseDistanceEncoding {
    struct Distance {
        int pulse{-1};
        int space{-1};
    };
    Distance frameStart;
    Distance bitOne;
    Distance bitZero;
    int tolerance{150};

    bool IsFrameStart(LircPayload a, LircPayload b);
    bool IsBitOne(LircPayload a, LircPayload b);
    bool IsBitZero(LircPayload a, LircPayload b);
    bool IsEndRx(LircPayload a, LircPayload b);
};

class LircSerializer {
public:

    LircSerializer(const PulseDistanceEncoding& enc);
    void Add(const LircPayload& payload);
    const std::vector<std::vector<uint8_t>>& Dump() const { return packets_; }
    void Clear();

    friend std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls);

private:
    LircSerializer() = default;
    PulseDistanceEncoding encoder_;
    std::vector<std::vector<uint8_t>> packets_;
    uint8_t pendingByte_{0u};
    uint8_t bitsBuffered_{0u};
    std::unique_ptr<LircPayload> pPendingBit_;

    void ResetPendingByte();
};

std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls);


#endif
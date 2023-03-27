#ifndef _LIRC_SERIALIZER_H
#define _LIRC_SERIALIZER_H

#include <vector>
#include <ostream>
#include <sstream>

class LircPayload;


struct PulseWidthId {
    int min{0};
    int max{0};
};

class LircSerializer {
public:

    LircSerializer(const std::vector<PulseWidthId>& pulseIds);
    bool Add(const LircPayload& payload);

    friend std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls);


private:
    LircSerializer() = default;
    std::vector<PulseWidthId> pulseIds_;
    bool isComplete_{false};
    std::stringstream serialized_;
};

std::ostream& operator<<(std::ostream& _stream, const LircSerializer& _ls);


#endif
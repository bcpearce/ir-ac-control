#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>

#include "lirc_payload.h"
#include "lirc_serializer.h"
#include "util.h"

int main(int argc, char* argv[]) {
    try {
        std::cout << "Opening LIRC device...\n";

        if (argc < 2) {
            throw std::invalid_argument("Specify LIRC device in ARG1");
        }
        std::string lircDev{argv[1]};

        uint32_t timeoutUs = (argc >= 3) ? std::stoul(argv[2]) : 10000;

        bool verbose = ((argc >= 4) && 
            (std::string{argv[3]} == "-v" || std::string{argv[3]} == "--verbose"));

        if (verbose) {
            std::cout << "Displaying ALL The THINGS...\n";
        }

        std::cout << "Accessing " << lircDev << "\n";

        int fd = SYSTEM_WRAPPER(open, lircDev.c_str(), O_RDONLY);

        {
            uint32_t features{0};
            SYSTEM_WRAPPER(ioctl, fd, LIRC_GET_FEATURES, &features);
            if (features & LIRC_CAN_REC_MODE2) {
                std::cout << lircDev << " supports MODE2\n";
            }
            else {
                std::cout << lircDev << " does not support MODE2\n";
                return -1;
            }
        }

        {
            std::cout << "Using timeout of " << timeoutUs << "us\n";
            SYSTEM_WRAPPER(ioctl, fd, LIRC_SET_REC_TIMEOUT, &timeoutUs);
        }
        std::vector<PulseWidthId> PulseIds;
        PulseIds.push_back({300, 600});
        PulseIds.push_back({1000, 1500});
        PulseIds.push_back({3000, 4000});
        LircSerializer ls(PulseIds);
        std::cout << "Reading in a loop...\n";
        for(ssize_t j = 0;;) {
            std::array<uint32_t, 256> buf;
            ssize_t ct = SYSTEM_WRAPPER(read, fd, buf.data(), sizeof(uint32_t) * buf.size());
            
            for(ssize_t i = 0; i < ct / sizeof(uint32_t); ++i) {
                auto pl = LircPayload::Decode(buf.at(i));

                if(ls.Add(pl)) {
                    std::cout << ls << "\n";
                    ls = LircSerializer(PulseIds);
                }

                if (pl.GetMode() == LircMode2::Timeout) {
                    std::cout << "ElemCt: " << std::dec << j << "\n";
                    std::cout << ("===========================") << "\n";
                    j = 0;
                }
                else {
                    if (verbose) {
                        std::cout << std::dec << j << ": " << pl << "\n";
                    }
                    j++;
                }
            }
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "Exception " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cout << "Unknown Error\n";
        return 1;
    }
}
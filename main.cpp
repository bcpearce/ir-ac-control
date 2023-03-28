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

#include "lirc_daikin.h"
#include "lirc_payload.h"
#include "lirc_serializer.h"
#include "util.h"

int main(int argc, char* argv[]) {
    try {
        std::cout << "Opening LIRC device...\n";

        if (argc < 2) {
            throw std::invalid_argument("Specify LIRC device in ARG1: ./ir [lirc_device] [timeoutMs]");
        }
        std::string lircDev{argv[1]};

        uint32_t timeoutUs = (argc >= 3) ? std::stoul(argv[2]) : 50000;

        std::cout << "Accessing " << lircDev << "\n";

        PulseDistanceEncoding encoder;
        encoder.frameStart = {3400, 1750};
        encoder.bitOne = {430, 1320};
        encoder.bitZero = {430, 430};
        encoder.tolerance = 200;

        LircSerializer ls(encoder);

        std::cout << "Reading in a loop...\n";
        for(ssize_t j = 1;;j++) {
            try {
                auto packets = ls.WaitForRx(lircDev.c_str());
                std::cout << j << ": " << packets.size() << " packets found\n";
                std::cout << packets.at(0).size() << ", " << packets.at(1).size() << "\n";
                LircDaikin ld(packets);
                std::cout << ld << "\n";
            } 
            catch (std::system_error& e) {
                throw; // rethrow, irrecovable
            }
            catch (std::exception& e) {
                std::cout << "Error: " << e.what();
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
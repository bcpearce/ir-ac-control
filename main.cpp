#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

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

void DisplayHelp(const std::string& lircDevRec, const std::string& lircDevSend) {
    std::cout 
        << "h - Display Help\n"
        << "1 - Read from IR (" << lircDevRec << ")\n"
        << "2 - Send IR code (" << (lircDevSend != "" ? lircDevSend : "Not Available") << ")\n"
        << "x - Exit\n";
}

void ReadFromIr(const char* lircDev, const PulseDistanceEncoding& encoder) {
    static int opCt = 0;
    LircSerializer ls(encoder);
    auto packets = ls.WaitForRx(lircDev);
    std::cout << ++opCt << ": " << packets.size() << " packets found\n";
    std::cout << packets.at(0).size() << ", " << packets.at(1).size() << "\n";
    LircDaikin ld = LircDaikin::Decode(packets);
    std::cout << ld << "\n";
}

void SendDaikinIr(const char* lircDev, const PulseDistanceEncoding& encoder) {

    {
        int fd = SYSTEM_WRAPPER(open, lircDev, O_RDONLY);
        close(fd);
    }
    LircDaikin ld;

    unsigned int temperatureF;
    std::cout << "Enter Temprature Deg F: ";
    std::cin >> temperatureF;
    ld.SetTemperatureF(temperatureF);

    std::cout << "Enter On/Off [1,0]: ";
    std::cin >> ld.powerOn_;

    std::cout << "Enter Mode [Fan,Heat,Cold,Dry,Auto]: ";
    std::string modeStr;
    std::cin >> modeStr;
    ld.mode_ = 
        modeStr == "Fan" ? daikin::Mode::Fan :
        modeStr == "Heat" ? daikin::Mode::Heat :
        modeStr == "Cold" ? daikin::Mode::Cold :
        modeStr == "Dry" ? daikin::Mode::Dry :
        modeStr == "Auto" ? daikin::Mode::Auto : throw std::invalid_argument("Unexpected Mode");

    std::cout << "Enter Fan Speed [Auto,Silent,1,2,3,4,5]: ";
    std::string fanSpeedStr;
    std::cin >> fanSpeedStr;
    ld.fan_ = 
        fanSpeedStr == "Auto" ? daikin::Fan::Auto :
        fanSpeedStr == "Silent" ? daikin::Fan::Silent :
        fanSpeedStr == "1" ? daikin::Fan::_1 :
        fanSpeedStr == "2" ? daikin::Fan::_2 :
        fanSpeedStr == "3" ? daikin::Fan::_3 :
        fanSpeedStr == "4" ? daikin::Fan::_4 :
        fanSpeedStr == "5" ? daikin::Fan::_5 : throw std::invalid_argument("Unexpected Fan");

    LircSerializer ls(encoder);
    auto packets = ld.Encode();
    std::cout << "Sending: \n";
    for (const auto& packet : packets) {
        std::cout << " | ";
        for (auto c : packet) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex  << +c << " ";
        }
        std::cout << " |\n";
    }
    ls.Tx(lircDev, packets);
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "Opening LIRC device...\n";

        if (argc < 2) {
            throw std::invalid_argument("Specify LIRC device in ARG1: ./ir [lirc_device] <lirc_send_device>");
        }
        std::string lircDevRec{argv[1]};
        std::string lircDevSend{(argc >= 3) ? argv[2] : ""};

        PulseDistanceEncoding encoder;
        encoder.frameStart = {3400, 1750};
        encoder.bitOne = {430, 1320};
        encoder.bitZero = {430, 430};
        encoder.tolerance = 200;
        encoder.gap = 35000;

        DisplayHelp(lircDevRec, lircDevSend);
        for(bool exit{false}; exit == false;) {
            try {
                std::cout << " >> ";
                std::string inputArg;
                std::cin >> inputArg;
                if (inputArg == "h" || inputArg == "H") {
                    DisplayHelp(lircDevRec, lircDevSend);
                } 
                else if (inputArg == "1") {
                    std::cout << "Waiting for IR input...\n";
                    ReadFromIr(lircDevRec.c_str(), encoder);
                } 
                else if (inputArg == "2") {
                    std::cout << "Sending IR Command...\n";
                    SendDaikinIr(lircDevSend.c_str(), encoder);
                } 
                else if (
                    inputArg == "x" || inputArg == "X" || 
                    inputArg == "Exit" || inputArg == "exit") {
                    exit = true;
                } 
                else {
                    std::cout << "Invalid Argument: \n";
                    DisplayHelp(lircDevRec, lircDevSend);
                }
            } 
            catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
                std::getchar();
            }
        }
        std::cout << "Exiting...\n";
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
#ifndef LINKING_H
#define LINKING_H
extern "C" {
#include "sdk_interface.h"
}
#include <string>
#include <thread>
#include <queue>
#include <memory>
#include <mutex>

class linking
{
public:
    linking();
    ~linking();
    int call(const std::string &callee);
    int hangup();

    std::shared_ptr<std::vector<uint8_t>> PopVideoData();
    std::shared_ptr<std::vector<uint8_t>> PopAudioData();
    void PushVideoData(uint8_t * ptr, int size);
    void PushAudioData(uint8_t * ptr, int size);
    CallStatus GetState(){return state;}

private:
    std::string sipServerHost_;
    std::string iceServerHost_;
    std::string mqttServerHost_;
    std::string accName_;
    std::string accPwd_;
    AccountID accountID_;

    std::string callee_;
    CallStatus state;
    bool isRegistered;
    int callID_;
    std::thread eventThread;
    bool quit_;
    bool sendFlag_; //没融合在

private:
    int registerAccount();
    static void eventHandler(void * opaque);

    std::mutex aqMutex_;
    std::mutex vqMutex_;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> videoQ_;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> audioQ_;
};

#endif // LINKING_H

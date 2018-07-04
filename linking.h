#ifndef LINKING_H
#define LINKING_H

#include <QObject>
extern "C" {
#include "sdk_interface.h"
}
#include <string>
#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include "ThreadCleaner.h"

class linking : public QObject, StopClass
{
    Q_OBJECT

public:
    explicit linking();
    void Stop();
    ~linking();
    int call();
    int hangup();

    std::shared_ptr<std::vector<uint8_t>> PopVideoData();
    std::shared_ptr<std::vector<uint8_t>> PopAudioData();
    void PushVideoData(uint8_t * ptr, int size);
    void PushAudioData(uint8_t * ptr, int size);
    std::string GetCallee(){return callee_;}
    void SetCallee(std::string s) {callee_ = s;}
    CallStatus GetState(){return state;}

signals:
    void registerSuccess();
    void onFirstAudio(QString timestr);
    void onFirstVideo(QString timestr);

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
    bool registerOkEmited = false;

private:
    int registerAccount();
    static void eventHandler(void * opaque);

    std::mutex aqMutex_;
    std::mutex vqMutex_;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> videoQ_;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> audioQ_;
    bool receiveFirstAudio = false;
    bool receiveFirstVideo = false;
};

#endif // LINKING_H

#include "tutkclient.h"

static const int AUDIO_BUF_SIZE = 1024*16;
static const int VIDEO_BUF_SIZE = 1024*1024;

std::mutex tutkInitMutex;
bool isTutkInited = false;

int initTutk() {
    std::unique_lock<std::mutex> lock(tutkInitMutex);
    if (isTutkInited == true) {
        return 0;
    }

}

#define os_gettime_us() std::chrono::high_resolution_clock::now().time_since_epoch().count()/1000
void PrintErrHandling (int nErr)
{
    switch (nErr)
    {
    case IOTC_ER_SERVER_NOT_RESPONSE :
        //-1 IOTC_ER_SERVER_NOT_RESPONSE
        printf ("[Error code : %d]\n", IOTC_ER_SERVER_NOT_RESPONSE );
        printf ("Master doesn't respond.\n");
        printf ("Please check the network wheather it could connect to the Internet.\n");
        break;
    case IOTC_ER_FAIL_RESOLVE_HOSTNAME :
        //-2 IOTC_ER_FAIL_RESOLVE_HOSTNAME
        printf ("[Error code : %d]\n", IOTC_ER_FAIL_RESOLVE_HOSTNAME);
        printf ("Can't resolve hostname.\n");
        break;
    case IOTC_ER_ALREADY_INITIALIZED :
        //-3 IOTC_ER_ALREADY_INITIALIZED
        printf ("[Error code : %d]\n", IOTC_ER_ALREADY_INITIALIZED);
        printf ("Already initialized.\n");
        break;
    case IOTC_ER_FAIL_CREATE_MUTEX :
        //-4 IOTC_ER_FAIL_CREATE_MUTEX
        printf ("[Error code : %d]\n", IOTC_ER_FAIL_CREATE_MUTEX);
        printf ("Can't create mutex.\n");
        break;
    case IOTC_ER_FAIL_CREATE_THREAD :
        //-5 IOTC_ER_FAIL_CREATE_THREAD
        printf ("[Error code : %d]\n", IOTC_ER_FAIL_CREATE_THREAD);
        printf ("Can't create thread.\n");
        break;
    case IOTC_ER_UNLICENSE :
        //-10 IOTC_ER_UNLICENSE
        printf ("[Error code : %d]\n", IOTC_ER_UNLICENSE);
        printf ("This UID is unlicense.\n");
        printf ("Check your UID.\n");
        break;
    case IOTC_ER_NOT_INITIALIZED :
        //-12 IOTC_ER_NOT_INITIALIZED
        printf ("[Error code : %d]\n", IOTC_ER_NOT_INITIALIZED);
        printf ("Please initialize the IOTCAPI first.\n");
        break;
    case IOTC_ER_TIMEOUT :
        //-13 IOTC_ER_TIMEOUT
        break;
    case IOTC_ER_INVALID_SID :
        //-14 IOTC_ER_INVALID_SID
        printf ("[Error code : %d]\n", IOTC_ER_INVALID_SID);
        printf ("This SID is invalid.\n");
        printf ("Please check it again.\n");
        break;
    case IOTC_ER_EXCEED_MAX_SESSION :
        //-18 IOTC_ER_EXCEED_MAX_SESSION
        printf ("[Error code : %d]\n", IOTC_ER_EXCEED_MAX_SESSION);
        printf ("[Warning]\n");
        printf ("The amount of session reach to the maximum.\n");
        printf ("It cannot be connected unless the session is released.\n");
        break;
    case IOTC_ER_CAN_NOT_FIND_DEVICE :
        //-19 IOTC_ER_CAN_NOT_FIND_DEVICE
        printf ("[Error code : %d]\n", IOTC_ER_CAN_NOT_FIND_DEVICE);
        printf ("Device didn't register on server, so we can't find device.\n");
        printf ("Please check the device again.\n");
        printf ("Retry...\n");
        break;
    case IOTC_ER_SESSION_CLOSE_BY_REMOTE :
        //-22 IOTC_ER_SESSION_CLOSE_BY_REMOTE
        printf ("[Error code : %d]\n", IOTC_ER_SESSION_CLOSE_BY_REMOTE);
        printf ("Session is closed by remote so we can't access.\n");
        printf ("Please close it or establish session again.\n");
        break;
    case IOTC_ER_REMOTE_TIMEOUT_DISCONNECT :
        //-23 IOTC_ER_REMOTE_TIMEOUT_DISCONNECT
        printf ("[Error code : %d]\n", IOTC_ER_REMOTE_TIMEOUT_DISCONNECT);
        printf ("We can't receive an acknowledgement character within a TIMEOUT.\n");
        printf ("It might that the session is disconnected by remote.\n");
        printf ("Please check the network wheather it is busy or not.\n");
        printf ("And check the device and user equipment work well.\n");
        break;
    case IOTC_ER_DEVICE_NOT_LISTENING :
        //-24 IOTC_ER_DEVICE_NOT_LISTENING
        printf ("[Error code : %d]\n", IOTC_ER_DEVICE_NOT_LISTENING);
        printf ("Device doesn't listen or the sessions of device reach to maximum.\n");
        printf ("Please release the session and check the device wheather it listen or not.\n");
        break;
    case IOTC_ER_CH_NOT_ON :
        //-26 IOTC_ER_CH_NOT_ON
        printf ("[Error code : %d]\n", IOTC_ER_CH_NOT_ON);
        printf ("Channel isn't on.\n");
        printf ("Please open it by IOTC_Session_Channel_ON() or IOTC_Session_Get_Free_Channel()\n");
        printf ("Retry...\n");
        break;
    case IOTC_ER_SESSION_NO_FREE_CHANNEL :
        //-31 IOTC_ER_SESSION_NO_FREE_CHANNEL
        printf ("[Error code : %d]\n", IOTC_ER_SESSION_NO_FREE_CHANNEL);
        printf ("All channels are occupied.\n");
        printf ("Please release some channel.\n");
        break;
    case IOTC_ER_TCP_TRAVEL_FAILED :
        //-32 IOTC_ER_TCP_TRAVEL_FAILED
        printf ("[Error code : %d]\n", IOTC_ER_TCP_TRAVEL_FAILED);
        printf ("Device can't connect to Master.\n");
        printf ("Don't let device use proxy.\n");
        printf ("Close firewall of device.\n");
        printf ("Or open device's TCP port 80, 443, 8080, 8000, 21047.\n");
        break;
    case IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED :
        //-33 IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED
        printf ("[Error code : %d]\n", IOTC_ER_TCP_CONNECT_TO_SERVER_FAILED);
        printf ("Device can't connect to server by TCP.\n");
        printf ("Don't let server use proxy.\n");
        printf ("Close firewall of server.\n");
        printf ("Or open server's TCP port 80, 443, 8080, 8000, 21047.\n");
        printf ("Retry...\n");
        break;
    case IOTC_ER_NO_PERMISSION :
        //-40 IOTC_ER_NO_PERMISSION
        printf ("[Error code : %d]\n", IOTC_ER_NO_PERMISSION);
        printf ("This UID's license doesn't support TCP.\n");
        break;
    case IOTC_ER_NETWORK_UNREACHABLE :
        //-41 IOTC_ER_NETWORK_UNREACHABLE
        printf ("[Error code : %d]\n", IOTC_ER_NETWORK_UNREACHABLE);
        printf ("Network is unreachable.\n");
        printf ("Please check your network.\n");
        printf ("Retry...\n");
        break;
    case IOTC_ER_FAIL_SETUP_RELAY :
        //-42 IOTC_ER_FAIL_SETUP_RELAY
        printf ("[Error code : %d]\n", IOTC_ER_FAIL_SETUP_RELAY);
        printf ("Client can't connect to a device via Lan, P2P, and Relay mode\n");
        break;
    case IOTC_ER_NOT_SUPPORT_RELAY :
        //-43 IOTC_ER_NOT_SUPPORT_RELAY
        printf ("[Error code : %d]\n", IOTC_ER_NOT_SUPPORT_RELAY);
        printf ("Server doesn't support UDP relay mode.\n");
        printf ("So client can't use UDP relay to connect to a device.\n");
        break;

    default :
        break;
    }
}

TutkClient::TutkClient(OutputFrameCallback cb, void *userData, std::string uid, std::string pwd):
    MediaSource (cb, userData, ""),
    uid_(uid),
    password_(pwd)
{
    avID_ = "admin";
    if (!isInited_) {
        if (Init() == 0)
            isInited_ = true;
    }

    audioBufForTutk_.resize(AUDIO_BUF_SIZE);
    videoBufForTutk_.resize(VIDEO_BUF_SIZE);
}

int TutkClient::Init()
{
    InputParam param2;

    param2.name_ = "video";
    param2.feedCbOpaqueArg_ = this;
    param2.feedRawDataWithPts_ = MediaSource::feedVideoDataToFFmpegDecoderCallback;
    param2.userData_ = pUserData;
    param2.getFrameCb_ = outputFrameCallback;
    //param2.formatHint_ = "h264";
    videoInput_ = std::make_shared<Input>(param2);
    videoInput_->Start();


    //TOTO IOTC_DeInitialize 什么时候调用问题
    int ret = IOTC_Initialize2(0);
    if(ret != IOTC_ER_NoERROR && ret != IOTC_ER_ALREADY_INITIALIZED) {
        printf("IOTCAPIs_Device exit...!!\n");
        PrintErrHandling (ret);
        return -1;
    }

    // alloc 3 sessions for video and two-way audio
    ret = avInitialize(32);
    if(ret <= 0) {
        printf("avInitialize fail. exit...!!\n");
        PrintErrHandling (ret);
        return -2;
    }

    //-------print version info-------
    unsigned int iotcVer;
    IOTC_Get_Version(&iotcVer);
    int avVer = avGetAVApiVer();
    unsigned char *p = reinterpret_cast<unsigned char *>(&iotcVer);
    unsigned char *p2 = reinterpret_cast<unsigned char *>(&avVer);
    char szIOTCVer[16], szAVVer[16];
    sprintf(szIOTCVer, "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);
    sprintf(szAVVer, "%d.%d.%d.%d", p2[3], p2[2], p2[1], p2[0]);
    printf("IOTCAPI version[%s] AVAPI version[%s]\n", szIOTCVer, szAVVer);
    //------end print version info-----


    /**********start lan search*********/
    int i;
    // alloc 12 struct st_LanSearchInfo maybe enough to contain device number on LAN
    // or you have more than 12 device on LAN, please modify this value
    struct st_LanSearchInfo psLanSearchInfo[12];
    // wait time 1000 ms to get result, if result is 0 you can extend to 2000 ms
    int nDeviceNum = IOTC_Lan_Search(psLanSearchInfo, 12, 1000);
    printf("IOTC_Lan_Search ret[%d]\n", nDeviceNum);
    for(i=0;i<nDeviceNum;i++)
    {
        printf("UID[%s] Addr[%s:%d]\n", psLanSearchInfo[i].UID, psLanSearchInfo[i].IP, psLanSearchInfo[i].port);
    }
    printf("LAN search done...\n");
    /**********end lan search*********/

    usecTimeInfo_ = os_gettime_us();
    connectCCRthread_ = std::thread(&TutkClient::connectCCRthread, this);

    isInited_ = true;
    return 0;
}

void TutkClient::connectCCRthread() {
    int SID;
    int tmpSID = IOTC_Get_SessionID();
    if(tmpSID < 0) {
        printf("IOTC_Get_SessionID error code [%d]\n", tmpSID);
        return;
    }

    SID = IOTC_Connect_ByUID_Parallel(uid_.c_str(), tmpSID);
    printf("  [] thread_ConnectCCR::IOTC_Connect_ByUID_Parallel, ret=[%d]\n", SID);
    if(SID < 0) {
        printf("IOTC_Connect_ByUID_Parallel failed[%d]\n", SID);
        return ;
    }
    SID_ = SID;

    int64_t nTimePoint = os_gettime_us();
    printf("SID[%d] Cost time %lld ms\n", SID, (nTimePoint - usecTimeInfo_)/1000);

    printf("Step 2: call IOTC_Connect_ByUID_Parallel(%s) ret(%d).......\n", uid_.c_str(), SID);

    if (isEnableSpeaker_)
        speakerChannel_ = IOTC_Session_Get_Free_Channel(SID_);
    else
        speakerChannel_ = -1;

    struct st_SInfo Sinfo;
    memset(&Sinfo, 0, sizeof(struct st_SInfo));

    const char* mode[3] = {"P2P", "RLY", "LAN"};

    int nResend;
    unsigned int srvType;
    // The avClientStart2 will enable resend mechanism. It should work with avServerStart3 in device.
    //int avIndex = avClientStart(SID, avID, avPass, 20, &srvType, 0);
    int avIndex = avClientStart2(SID, avID_.c_str(), password_.c_str(), 20, &srvType, 0, &nResend);
    if(nResend == 0){
        printf("Resend is not supported.");
    }
    printf("Step 2: call avClientStart2(%d).......\n", avIndex);
    if(avIndex < 0) {
        printf("avClientStart2 failed[%d]\n", avIndex);
        return;
    }
    avIndex_ = avIndex;

    if(IOTC_Session_Check(SID_, &Sinfo) == IOTC_ER_NoERROR) {
        if( isdigit( Sinfo.RemoteIP[0] ))
            printf("Device is from %s:%d[%s] Mode=%s NAT[%d] IOTCVersion[%X]\n",Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID, mode[(int)Sinfo.Mode], Sinfo.NatType, Sinfo.IOTCVersion);
    }
    printf("avClientStart2 OK[%d], Resend[%d]\n", avIndex, nResend);

    avClientCleanBuf(0);

    if(startIpcamStream())
    {
        printf("  [] thread_ConnectCCR::start_ipcam_stream \n");
        videoThread_ = std::thread(&TutkClient::videoThread, this);

        if (isEnableSpeaker_) {
            speakerThread_ = std::thread(&TutkClient::speakerThread, this);
        }
    }

    if (isEnableAudio_) {
        PlayAudio();
    }

    return;
}

void TutkClient::PlayAudio() {

    if (audioThread_.joinable()) {
        return;
    }

    InputParam param1;
    param1.name_ = "audio";
    param1.feedCbOpaqueArg_ = this;
    param1.feedRawDataWithPts_ = MediaSource::feedAudioDataToFFmpegDecoderCallback;
    param1.userData_ = pUserData;
    param1.getFrameCb_ = outputFrameCallback;
    //param1.formatHint_ = "mulaw";
    //param1.audioOpts.push_back("ar");
    //param1.audioOpts.push_back("8000");
    audiOInput_ = std::make_shared<Input>(param1);
    audiOInput_->Start();

    int ret = 0;
    SMsgAVIoctrlAVStream ioMsg;
    memset(&ioMsg, 0, sizeof(SMsgAVIoctrlAVStream));
    if((ret = avSendIOCtrl(avIndex_, IOTYPE_USER_IPCAM_AUDIOSTART, (char *)&ioMsg, sizeof(SMsgAVIoctrlAVStream))) < 0)
    {
        printf("start_ipcam_stream failed[%d]\n", ret);
        return;
    }
    printf("send Cmd: IOTYPE_USER_IPCAM_AUDIOSTART, OK\n");


    isEnableAudio_ = true;
    audioThread_ = std::thread(&TutkClient::audioThread, this);
    return;
}

/*********
Send IOCtrl CMD to device
***/
int TutkClient::startIpcamStream()
{
    int ret;
    unsigned short val = 0;
    if((ret = avSendIOCtrl(avIndex_, IOTYPE_INNER_SND_DATA_DELAY, (char *)&val, sizeof(unsigned short)) < 0))
    {
        printf("start_ipcam_stream failed[%d]\n", ret);
        return 0;
    }
    printf("send Cmd: IOTYPE_INNER_SND_DATA_DELAY, OK\n");

    SMsgAVIoctrlAVStream ioMsg;
    memset(&ioMsg, 0, sizeof(SMsgAVIoctrlAVStream));
    if((ret = avSendIOCtrl(avIndex_, IOTYPE_USER_IPCAM_START, (char *)&ioMsg, sizeof(SMsgAVIoctrlAVStream))) < 0)
    {
        printf("start_ipcam_stream failed[%d]\n", ret);
        return 0;
    }
    printf("send Cmd: IOTYPE_USER_IPCAM_START, OK\n");


    if (isEnableSpeaker_) {
        ioMsg.channel = speakerChannel_;
        if((ret = avSendIOCtrl(avIndex_, IOTYPE_USER_IPCAM_SPEAKERSTART, (char *)&ioMsg, sizeof(SMsgAVIoctrlAVStream))) < 0)
        {
            printf("start_ipcam_stream failed[%d]\n", ret);
            return 0;
        }
        printf("send Cmd: IOTYPE_USER_IPCAM_SPEAKERSTART, OK\n");
    }
    return 1;
}

void TutkClient::videoThread() {
    printf("[thread_ReceiveVideo] Starting....\n");
    int ret;

    FRAMEINFO_t frameInfo;
    unsigned int frmNo;
    printf("Start IPCAM video stream OK!\n");
    int /*flag = 0,*/ cnt = 0, fpsCnt = 0, round = 0, lostCnt = 0;
    int outBufSize = 0;
    int outFrmSize = 0;
    int outFrmInfoSize = 0;
    //int bCheckBufWrong;
    int bps = 0;

    usecTimeInfo_ = os_gettime_us();


    char * buf = videoBufForTutk_.data();
    while(!isQuit_)
    {
        ret = avRecvFrameData2(avIndex_, buf, videoBufForTutk_.size(), &outBufSize, &outFrmSize, (char *)&frameInfo, sizeof(FRAMEINFO_t), &outFrmInfoSize, &frmNo);

        // show Frame Info at 1st frame
        if(frmNo == 0)
        {
            const char *format[] = {"MPEG4","H263","H264","MJPEG", "HEVC", "UNKNOWN"};
            int idx = 0;
            if(frameInfo.codec_id == MEDIA_CODEC_VIDEO_MPEG4) {
                videoCodecId_ = MEDIA_CODEC_VIDEO_MPEG4;
                idx = 0;
            } else if(frameInfo.codec_id == MEDIA_CODEC_VIDEO_H263) {
                videoCodecId_ = MEDIA_CODEC_VIDEO_H263;
                idx = 1;
            } else if(frameInfo.codec_id == MEDIA_CODEC_VIDEO_H264) {
                videoCodecId_ = MEDIA_CODEC_VIDEO_H264;
                idx = 2;
            } else if(frameInfo.codec_id == MEDIA_CODEC_VIDEO_MJPEG) {
                videoCodecId_ = MEDIA_CODEC_VIDEO_MJPEG;
                idx = 3;
            } else if(frameInfo.codec_id == MEDIA_CODEC_VIDEO_HEVC) {
                videoCodecId_ = MEDIA_CODEC_VIDEO_HEVC;
                idx = 4;
            } else
                idx = 5;
            printf("--- %d Video Formate: %s ---\n", ret, format[idx]);
        }

        if(ret == AV_ER_DATA_NOREADY)
        {
            //printf("AV_ER_DATA_NOREADY[%d]\n", avIndex_);
            usleep(10 * 1000);
            continue;
        }
        else if(ret == AV_ER_LOSED_THIS_FRAME)
        {
            printf("Lost video frame NO[%d]\n", frmNo);
            lostCnt++;
            //continue;
        }
        else if(ret == AV_ER_INCOMPLETE_FRAME)
        {
#if 1
            if(outFrmInfoSize > 0)
                printf("Incomplete video frame NO[%d] ReadSize[%d] FrmSize[%d] FrmInfoSize[%u] Codec[%d] Flag[%d]\n", frmNo, outBufSize, outFrmSize, outFrmInfoSize, frameInfo.codec_id, frameInfo.flags);
            else
                printf("Incomplete video frame NO[%d] ReadSize[%d] FrmSize[%d] FrmInfoSize[%u]\n", frmNo, outBufSize, outFrmSize, outFrmInfoSize);
#endif
            lostCnt++;
        }
        else if(ret == AV_ER_SESSION_CLOSE_BY_REMOTE)
        {
            printf("[thread_ReceiveVideo] AV_ER_SESSION_CLOSE_BY_REMOTE\n");
            break;
        }
        else if(ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
        {
            printf("[thread_ReceiveVideo] AV_ER_REMOTE_TIMEOUT_DISCONNECT\n");
            break;
        }
        else if(ret == IOTC_ER_INVALID_SID)
        {
            printf("[thread_ReceiveVideo] Session cant be used anymore\n");
            break;
        }
        else
        {
            bps += outBufSize;
        }

        cnt++;

        fpsCnt++;

        if (ret > 6 * 1024)
            canPushVideo = true;
        if (!canPushVideo)
            fprintf(stderr, "drop data :%d\n", ret);
        if (ret > 0 && canPushVideo) {
            int64_t nPts = frameInfo.timestamp;
            memcpy(buf+ret, &nPts, 8);
            memcpy(buf+ret+8, &videoCodecId_, sizeof(ENUM_CODECID));
            videoCache_.Append(buf, ret+8+sizeof(ENUM_CODECID));
        }
        //统计fps信息
        int64_t nTimePoint = os_gettime_us();
        int64_t nDiff = nTimePoint - usecTimeInfo_;
        if(nDiff > 1000000) {
            round++;
            printf("[avIndex:%d] FPS=%d, LostFrmCnt:%d, TotalCnt:%d, LastFrameSize:%d Byte, Codec:%d, Flag:%d, bps:%d Kbps\n", \
                   avIndex_, fpsCnt, lostCnt, cnt, outFrmSize, frameInfo.codec_id, frameInfo.flags, (bps/1024)*8);
            usecTimeInfo_ = os_gettime_us();
            fpsCnt = 0;
            bps = 0;
        }
    }

    printf("[thread_ReceiveVideo] thread exit\n");

    return ;
}
FILE *xxfile;
void TutkClient::audioThread()
{
    printf("[thread_ReceiveAudio] Starting....\n");

    FRAMEINFO_t frameInfo;
    unsigned int frmNo;
    //int recordCnt = 0;
    int ret;
    printf("Start IPCAM audio stream OK!\n");


    char *buf = audioBufForTutk_.data();
    while(!isQuit_)
    {
        ret = avRecvAudioData(avIndex_, buf, audioBufForTutk_.size(), (char *)&frameInfo, sizeof(FRAMEINFO_t), &frmNo);

        // show Frame Info at 1st frame
        if(frmNo == 0)
        {
            const char *format[] = {"PCM","G711U","G711A","AAC-Adts","AAC+Adts","UNKNOWN"};
            int idx = 0;
            if(frameInfo.codec_id == MEDIA_CODEC_AUDIO_PCM) {
                audioCodecId_ = MEDIA_CODEC_AUDIO_PCM;
                idx = 0;
            } else if(frameInfo.codec_id == MEDIA_CODEC_AUDIO_G711U) {
                audioCodecId_ = MEDIA_CODEC_AUDIO_G711U;
                idx = 1;
            } else if(frameInfo.codec_id == MEDIA_CODEC_AUDIO_G711A) {
                audioCodecId_ = MEDIA_CODEC_AUDIO_G711A;
                idx = 2;
            } else if(frameInfo.codec_id == MEDIA_CODEC_AUDIO_AAC_RAW) {
                audioCodecId_ = MEDIA_CODEC_AUDIO_AAC_RAW;
                idx = 3;
            } else if(frameInfo.codec_id == MEDIA_CODEC_AUDIO_AAC_ADTS) {
                audioCodecId_ = MEDIA_CODEC_AUDIO_AAC_ADTS;
                idx = 4;
            } else
                idx = 5;
            printf("--- Audio Formate: %s ---\n", format[idx]);
        }

        if(ret == AV_ER_SESSION_CLOSE_BY_REMOTE)
        {
            printf("[thread_ReceiveAudio] AV_ER_SESSION_CLOSE_BY_REMOTE\n");
            break;
        }
        else if(ret == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
        {
            printf("[thread_ReceiveAudio] AV_ER_REMOTE_TIMEOUT_DISCONNECT\n");
            break;
        }
        else if(ret == IOTC_ER_INVALID_SID)
        {
            printf("[thread_ReceiveAudio] Session cant be used anymore\n");
            break;
        }
        else if(ret == AV_ER_LOSED_THIS_FRAME)
        {
            //printf("Audio AV_ER_LOSED_THIS_FRAME[%d]\n", frmNo);
            continue;
        }
        else if (ret == AV_ER_DATA_NOREADY)
        {
            usleep(40000);
            continue;
        }
        else if(ret < 0)
        {
            printf("Other error[%d]!!!\n", ret);
            continue;
        }

        if (ret > 0 ) {
            int64_t nPts = frameInfo.timestamp;
            memcpy(buf+ret, &nPts, 8);
            memcpy(buf+ret+8, &audioCodecId_, sizeof(ENUM_CODECID));
            if (xxfile == NULL) {
                xxfile = fopen("/tmp/sxt.g711", "w+");
                if (xxfile == NULL)
                    fprintf(stderr, "open xxfile fail---------\n");
            }
            if (xxfile) {
                fwrite(buf, 1, ret, xxfile);
            }
            audioCache_.Append(buf, ret+8+sizeof(ENUM_CODECID));
        }

        //*** audio recv thread stop condition if necessary ***
/*
        if(recordCnt++ > 800)
            break;
*/
    }

    printf("[thread_ReceiveAudio] thread exit\n");

    return;
}

void TutkClient::speakerThread()
{
    return;
}

void TutkClient::Stop() {
    if (isQuit_)
        return;
    isQuit_ = true;

    if( videoThread_.joinable())
        videoThread_.join();
    if( audioThread_.joinable())
        audioThread_.join();
    if( speakerThread_.joinable())
        speakerThread_.join();
    if (connectCCRthread_.joinable()) {
        connectCCRthread_.join();
    }

    videoCache_.Stop();
    if (videoInput_) {
        videoInput_->Stop();
        videoInput_.reset();
    }
    audioCache_.Stop();
    if (audiOInput_) {
        audiOInput_->Stop();
        audiOInput_.reset();
    }

    if (avIndex_ >= 0) {
        avClientStop(avIndex_);
        avIndex_ = -1;
        printf("avClientStop OK\n");
    }

    if (SID_ >= 0) {
        IOTC_Session_Close(SID_);
        printf("SID[%d] IOTC_Session_Close, OK\n", SID_);
        SID_ = -1;
    }
    isInited_ = false;
    return;
}

TutkClient::~TutkClient() {
    Stop();
}

int TutkClient::readFrame(FeedFrame *pFrame, StreamCache &cache) {
    std::vector<char> oneFrame;
    int ret = cache.TakeOne(oneFrame, nTimeout_);
    if (ret != 0) {
        return ret;
    }
    memcpy(&pFrame->nPts_, oneFrame.data() + oneFrame.size() - 8 - sizeof(ENUM_CODECID), 8);
    pFrame->nDts_ = pFrame->nPts_;

    ENUM_CODECID tutkCodecId;
    memcpy(&tutkCodecId, oneFrame.data() + oneFrame.size() - sizeof(ENUM_CODECID), sizeof(ENUM_CODECID));
    switch(tutkCodecId) {
    case MEDIA_CODEC_AUDIO_PCM:
        pFrame->type_ = ReceiverType::PCMS16E;
        break;
    case MEDIA_CODEC_AUDIO_G711U:
        pFrame->type_ = ReceiverType::G711U;
        break;
    case MEDIA_CODEC_AUDIO_G711A:
        pFrame->type_ = ReceiverType::G711A;
        break;
    case MEDIA_CODEC_AUDIO_AAC_RAW:
        pFrame->type_ = ReceiverType::AACNoAdts;
        break;
    case MEDIA_CODEC_AUDIO_AAC_ADTS:
        pFrame->type_ = ReceiverType::AACAdts;
        break;
    case MEDIA_CODEC_VIDEO_H264:
        pFrame->type_ = ReceiverType::H264;
        break;
    case MEDIA_CODEC_VIDEO_HEVC:
        pFrame->type_ = ReceiverType::H265;
        break;
    default:
        pFrame->type_ = ReceiverType::UNKNOWN;
    }
    oneFrame.resize(oneFrame.size() - 8 - sizeof(ENUM_CODECID));
    pFrame->data_.swap(oneFrame);
    //fprintf(stderr, "qml read frame:%lld\n", pFrame->nPts_);
    return 0;
}

int TutkClient::readAudioFrame(FeedFrame *pFrame){
    return readFrame(pFrame, audioCache_);
}

int TutkClient::readVideoFrame(FeedFrame *pFrame){
   return readFrame(pFrame, videoCache_);
}

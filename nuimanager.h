#ifndef NUIMANAGER_H
#define NUIMANAGER_H

#include "manager.h"
#include <QThread>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

class ScreenManager;

class NuiManager : public QThread, public Manager
{
public:
    ScreenManager* sm;

    static xn::Context g_Context;
    static xn::ScriptNode g_ScriptNode;
    static xn::DepthGenerator g_DepthGenerator;
    static xn::UserGenerator g_UserGenerator;
    static xn::Recorder* g_pRecorder;

    static XnUserID g_nPlayer;
    static XnBool g_bCalibrated;
    static XnBool g_bPause;
    bool should_run;
    bool is_hand_init;
    XnVector3D last_left_hand;
    XnVector3D last_right_hand;

    NuiManager(ScreenManager* parent);
    ~NuiManager();
    void run();

    void init();
    int init_device();
    static XnBool AssignPlayer(XnUserID);
    static void NewUser(xn::UserGenerator&, XnUserID, void*);
    static void FindPlayer();
    static void LostPlayer();
    static void LostUser(xn::UserGenerator&, XnUserID, void*);
    static void PoseDetected(xn::PoseDetectionCapability&, const XnChar*, XnUserID, void*);
    static void CalibrationStarted(xn::SkeletonCapability&, XnUserID, void*);
    //static void CalibrationEnded(xn::SkeletonCapability&, XnUserID, XnBool, void*);
    static void CalibrationCompleted(xn::SkeletonCapability&, XnUserID, XnCalibrationStatus, void*);
};

#endif // NUIMANAGER_H

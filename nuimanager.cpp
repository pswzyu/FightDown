#include "nuimanager.h"
#include "screenmanager.h"
#include "statemachine.h"

#define SAMPLE_XML_PATH "./Sample-User.xml"
#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {																\
                qDebug("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }
#define CHECK_ERRORS(rc, errors, what)		\
        if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
        XnChar strError[1024];				\
        errors.ToString(strError, 1024);	\
        qDebug("%s\n", strError);			\
        return (rc);						\
}

xn::Context NuiManager::g_Context;
xn::ScriptNode NuiManager::g_ScriptNode;
xn::DepthGenerator NuiManager::g_DepthGenerator;
xn::UserGenerator NuiManager::g_UserGenerator;
xn::Recorder* NuiManager::g_pRecorder;

XnUserID NuiManager::g_nPlayer;
XnBool NuiManager::g_bCalibrated;
XnBool NuiManager::g_bPause;


NuiManager::NuiManager(ScreenManager* parent) : sm(parent)
{
    g_nPlayer = 0;
    g_bCalibrated = false;

    g_bPause = false;
    should_run = true;
    is_hand_init = true;

    init_device();
}
NuiManager::~NuiManager()
{

}
void NuiManager::run()
{
    float l_speed_x = 0, l_speed_y = 0;
    float r_speed_x = 0, r_speed_y = 0;
    while (should_run)
    {
        // Read next available data
        g_Context.WaitOneUpdateAll(g_DepthGenerator);

        if (g_nPlayer != 0)
        {
            // TODO:获取玩家数量
            sm->state_machine->any_player = 1; // 设定为已有玩家
            // 获取用户躯干
            XnSkeletonJointPosition joint_torso;
            g_UserGenerator.GetSkeletonCap()
                    .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_TORSO, joint_torso);

            sm->state_machine->player_x = joint_torso.position.X * 0.6 + sm->width() / 2  ;
            //qDebug("player_x : %lf", sm->state_machine->player_x);

            // 获取用户左右手的位置
            XnSkeletonJointPosition joint_left, joint_right;
            g_UserGenerator.GetSkeletonCap()
                .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_LEFT_HAND, joint_left);
            g_UserGenerator.GetSkeletonCap()
                .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_RIGHT_HAND, joint_right);
//            qDebug( "user %d l:(%f,%f) r:(%f, %f)\n", g_nPlayer,
//                joint_left.position.X, joint_left.position.Y,
//                joint_right.position.X, joint_right.position.Y );
            if (is_hand_init)
            {
                last_left_hand = joint_left.position;
                last_right_hand = joint_right.position;
                is_hand_init = false;
            }else
            {
                l_speed_x = joint_left.position.X - last_left_hand.X;
                l_speed_y = joint_left.position.Y - last_left_hand.Y;
                r_speed_x = joint_right.position.X - last_right_hand.X;
                r_speed_y = joint_right.position.Y - last_right_hand.Y;

                if ( l_speed_x < 0 && l_speed_x * l_speed_x + l_speed_y * l_speed_y > 60 * 60 )
                {
                    sm->state_machine->left_dart_velocity_x = l_speed_x / 20;
                    sm->state_machine->left_dart_velocity_y = -l_speed_y / 20;
                }
                if ( r_speed_x > 0 && r_speed_x * r_speed_x + r_speed_y * r_speed_y > 60 * 60 )
                {
                    sm->state_machine->right_dart_velocity_x = r_speed_x / 20;
                    sm->state_machine->right_dart_velocity_y = -r_speed_y / 20;
                }

                last_left_hand = joint_left.position;
                last_right_hand = joint_right.position;
            }
        }else
        {
            sm->state_machine->any_player = 0; // 置为没有玩家
            is_hand_init = true;
        }
    }
}
void NuiManager::init()
{

}
int NuiManager::init_device()
{
    XnStatus rc = XN_STATUS_OK;
    xn::EnumerationErrors errors;

    rc = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, g_ScriptNode, &errors);
    CHECK_ERRORS(rc, errors, "InitFromXmlFile");
    CHECK_RC(rc, "InitFromXml");

    rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(rc, "Find depth generator");
    rc = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    CHECK_RC(rc, "Find user generator");

    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
            !g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
            qDebug("User generator doesn't support either skeleton or pose detection.\n");
            return XN_STATUS_ERROR;
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    rc = g_Context.StartGeneratingAll();
    CHECK_RC(rc, "StartGenerating");

    XnCallbackHandle hUserCBs, hCalibrationStartCB, hCalibrationCompleteCB, hPoseCBs;
    g_UserGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);
    rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(CalibrationStarted, NULL, hCalibrationStartCB);
    CHECK_RC(rc, "Register to calbiration start");
    rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(CalibrationCompleted, NULL, hCalibrationCompleteCB);
    CHECK_RC(rc, "Register to calibration complete");
    rc = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(PoseDetected, NULL, hPoseCBs);
    CHECK_RC(rc, "Register to pose detected");
    return 0;
}

XnBool NuiManager::AssignPlayer(XnUserID user)
{
    return TRUE;
}

void NuiManager::NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    qDebug("*********NewUser**********\nnew user id : %d \n now users:", user);
    g_UserGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", user);
    XnUserID aUsers[20];
    XnUInt16 nUsers = 20;
    g_UserGenerator.GetUsers(aUsers, nUsers);

    for (int i = 0; i < nUsers; ++i)
    {
        qDebug(" %d ", aUsers[i]);
    }
    qDebug("\n********************\n");
}
void NuiManager::FindPlayer()
{
}
void NuiManager::LostPlayer()
{
}
void NuiManager::LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    qDebug("*********LostPlayer**********\nlost user id : %d \n now users:", user);
    XnUserID aUsers[20];
    XnUInt16 nUsers = 20;
    g_UserGenerator.GetUsers(aUsers, nUsers);

    for (int i = 0; i < nUsers; ++i)
    {
        qDebug(" %d ", aUsers[i]);
    }
    qDebug("\n********************\n");
}
void NuiManager::PoseDetected(xn::PoseDetectionCapability& pose,
    const XnChar* strPose, XnUserID user, void* cxt)
{
    qDebug("*********PoseDetected**********\n");
    qDebug("Found pose \"%s\" for user %d\n", strPose, user);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(user, TRUE);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(user);
    qDebug("*********************\n");
}

void NuiManager::CalibrationStarted(xn::SkeletonCapability& skeleton,
    XnUserID user, void* cxt)
{
    qDebug("*********CalibrationStarted**********\n");
    qDebug("Calibration started for user: %d\n", user);
    qDebug("******************************\n");
}

void NuiManager::CalibrationCompleted(xn::SkeletonCapability& skeleton,
    XnUserID user, XnCalibrationStatus eStatus, void* cxt)
{
    qDebug("*********CalibrationCompleted*********\n");
    qDebug("Calibration done for user [%d] %ssuccessfully\n", user, (eStatus == XN_CALIBRATION_STATUS_OK)?"":"un");
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        g_UserGenerator.GetSkeletonCap().StartTracking(user);
        qDebug("users now: ");
        XnUserID aUsers[20];
        XnUInt16 nUsers = 20;
        g_UserGenerator.GetUsers(aUsers, nUsers);
        for (int i = 0; i < nUsers; ++i)
        {
            qDebug(" %d ", aUsers[i]);
        }
        g_nPlayer = user;


    }else
    {
        qDebug("Request Calibration on user %d again\n", user);
        g_UserGenerator.GetSkeletonCap().RequestCalibration(user, false);
    }
    qDebug("\n*************************\n");
}


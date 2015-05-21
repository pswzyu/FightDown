#include "nuimanager.h"
#include "screenmanager.h"
#include "statemachine.h"

//#define SAMPLE_XML_PATH "./Sample-User.xml"
//#define CHECK_RC(rc, what)											\
//        if (rc != XN_STATUS_OK)											\
//        {																\
//                qDebug("%s failed: %s\n", what, xnGetStatusString(rc));		\
//                return rc;													\
//        }
//#define CHECK_ERRORS(rc, errors, what)		\
//        if (rc == XN_STATUS_NO_NODE_PRESENT)	\
//{										\
//        XnChar strError[1024];				\
//        errors.ToString(strError, 1024);	\
//        qDebug("%s\n", strError);			\
//        return (rc);						\
//}

//xn::Context NuiManager::g_Context;
//xn::ScriptNode NuiManager::g_ScriptNode;
//xn::DepthGenerator NuiManager::g_DepthGenerator;
//xn::UserGenerator NuiManager::g_UserGenerator;
//xn::Recorder* NuiManager::g_pRecorder;

//XnUserID NuiManager::g_nPlayer;
//XnBool NuiManager::g_bCalibrated;
//XnBool NuiManager::g_bPause;


NuiManager::NuiManager(ScreenManager* parent) : sm(parent), m_hNextSkeletonEvent(NULL),
    g_nPlayer(0)
{


    should_run = true;
    is_hand_init = true;

    hand_dart_thres = 900;
    dart_speed_factor = 0.2f;
    player_move_factor = 2;

    init_device();
}
NuiManager::~NuiManager()
{
    if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }
}
void NuiManager::run()
{
    float l_speed_x = 0, l_speed_y = 0;
    float r_speed_x = 0, r_speed_y = 0;

    NUI_SKELETON_DATA* one_skeleton = NULL;
    NUI_COLOR_IMAGE_POINT torso_pos;
    NUI_COLOR_IMAGE_POINT this_left_hand;
    NUI_COLOR_IMAGE_POINT this_right_hand;

    int numof_players = 0;

    while (should_run)
    {
        // Read next available data
        // Wait for 0ms, just quickly test if it is time to process a skeleton
        if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
        {
            NUI_SKELETON_FRAME skeletonFrame = {0};

            // Get the skeleton frame that is ready
            if (FAILED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
            {
                // skip this frame
                continue;
            }

            // smooth out the skeleton data
            m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

            numof_players = 0;

            for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
            {
                NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

                if (NUI_SKELETON_TRACKED == trackingState)
                {
                    // count how many tracked users
                    ++ numof_players;
                    // get the torso pos and the hand pos
                    // torso pos
                    one_skeleton = &(skeletonFrame.SkeletonData[i]);
                    torso_pos = SkeletonPosToScreen(
                                one_skeleton->SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER]);
                    sm->state_machine->player_x = torso_pos.x * player_move_factor +
                            sm->state_machine->stage_width / 2 * (1 - player_move_factor);


                    // hand pos
                    this_left_hand = SkeletonPosToScreen(
                                one_skeleton->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT]);
                    this_right_hand = SkeletonPosToScreen(
                                one_skeleton->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT]);
                    //qDebug("rhx:%ld, rhy%ld",this_right_hand.x, this_right_hand.y);
                    // left hand
                    if (is_hand_init)
                    {
                        last_left_hand = this_left_hand;
                        last_right_hand = this_right_hand;
                        is_hand_init = false;
                    }else
                    {
                        l_speed_x = this_left_hand.x - last_left_hand.x;
                        l_speed_y = this_left_hand.y - last_left_hand.y;
                        r_speed_x = this_right_hand.x - last_right_hand.x;
                        r_speed_y = this_right_hand.y - last_right_hand.y;

                        if ( l_speed_x < 0 && l_speed_x * l_speed_x + l_speed_y * l_speed_y > hand_dart_thres )
                        {
                            sm->state_machine->left_dart_velocity_x = l_speed_x * dart_speed_factor;
                            sm->state_machine->left_dart_velocity_y = l_speed_y * dart_speed_factor;
                        }
                        if ( r_speed_x > 0 && r_speed_x * r_speed_x + r_speed_y * r_speed_y > hand_dart_thres )
                        {
                            sm->state_machine->right_dart_velocity_x = r_speed_x * dart_speed_factor;
                            sm->state_machine->right_dart_velocity_y = r_speed_y * dart_speed_factor;
                        }

                        last_left_hand = this_left_hand;
                        last_right_hand = this_right_hand;
                    }

                    // TODO: only allow one player now
                    break;

                }
                else if (NUI_SKELETON_POSITION_ONLY == trackingState)
                {
                    // ignore this type
                }
            }

            g_nPlayer = numof_players;
            sm->state_machine->any_player = numof_players; // 设定为已有玩家

            // TODO: only support one player now
            if (numof_players == 0)
            {
                is_hand_init = true;
            }
        }


    }
}
void NuiManager::init()
{

}
int NuiManager::init_device()
{
    INuiSensor * pNuiSensor;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        qDebug("FAILED: init_device > NuiGetSensorCount, ; %ld", hr);
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            qDebug("FAILED: init_device > NuiCreateSensorByIndex, %d; %ld", i, hr);
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
        qDebug("No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}


NUI_COLOR_IMAGE_POINT NuiManager::SkeletonPosToScreen(Vector4 skeletonPoint)
{
    long x, y;
    unsigned short depth;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

    float screenPointX = static_cast<float>(x * sm->state_machine->stage_width) / KINECT_WIDTH;
    float screenPointY = static_cast<float>(y * sm->state_machine->stage_height) / KINECT_HEIGHT;

    return NUI_COLOR_IMAGE_POINT{screenPointX, screenPointY};
}

//XnBool NuiManager::AssignPlayer(XnUserID user)
//{
//    return TRUE;
//}

//void NuiManager::NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
//{
//    qDebug("*********NewUser**********\nnew user id : %d \n now users:", user);
//    g_UserGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", user);
//    XnUserID aUsers[20];
//    XnUInt16 nUsers = 20;
//    g_UserGenerator.GetUsers(aUsers, nUsers);

//    for (int i = 0; i < nUsers; ++i)
//    {
//        qDebug(" %d ", aUsers[i]);
//    }
//    qDebug("\n********************\n");
//}
//void NuiManager::FindPlayer()
//{
//}
//void NuiManager::LostPlayer()
//{
//}
//void NuiManager::LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
//{
//    qDebug("*********LostPlayer**********\nlost user id : %d \n now users:", user);
//    XnUserID aUsers[20];
//    XnUInt16 nUsers = 20;
//    g_UserGenerator.GetUsers(aUsers, nUsers);

//    for (int i = 0; i < nUsers; ++i)
//    {
//        qDebug(" %d ", aUsers[i]);
//    }
//    qDebug("\n********************\n");
//}
//void NuiManager::PoseDetected(xn::PoseDetectionCapability& pose,
//    const XnChar* strPose, XnUserID user, void* cxt)
//{
//    qDebug("*********PoseDetected**********\n");
//    qDebug("Found pose \"%s\" for user %d\n", strPose, user);
//    g_UserGenerator.GetSkeletonCap().RequestCalibration(user, TRUE);
//    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(user);
//    qDebug("*********************\n");
//}

//void NuiManager::CalibrationStarted(xn::SkeletonCapability& skeleton,
//    XnUserID user, void* cxt)
//{
//    qDebug("*********CalibrationStarted**********\n");
//    qDebug("Calibration started for user: %d\n", user);
//    qDebug("******************************\n");
//}

//void NuiManager::CalibrationCompleted(xn::SkeletonCapability& skeleton,
//    XnUserID user, XnCalibrationStatus eStatus, void* cxt)
//{
//    qDebug("*********CalibrationCompleted*********\n");
//    qDebug("Calibration done for user [%d] %ssuccessfully\n", user, (eStatus == XN_CALIBRATION_STATUS_OK)?"":"un");
//    if (eStatus == XN_CALIBRATION_STATUS_OK)
//    {
//        g_UserGenerator.GetSkeletonCap().StartTracking(user);
//        qDebug("users now: ");
//        XnUserID aUsers[20];
//        XnUInt16 nUsers = 20;
//        g_UserGenerator.GetUsers(aUsers, nUsers);
//        for (int i = 0; i < nUsers; ++i)
//        {
//            qDebug(" %d ", aUsers[i]);
//        }
//        g_nPlayer = user;


//    }else
//    {
//        qDebug("Request Calibration on user %d again\n", user);
//        g_UserGenerator.GetSkeletonCap().RequestCalibration(user, false);
//    }
//    qDebug("\n*************************\n");
//}


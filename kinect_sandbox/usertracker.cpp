/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Simple Skeleton Sample                               *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include "NiTE.h"

#include "NiteSampleUtilities.h"

#define MAX_USERS 8
#define CHOOSING_BOX_POSITION_X 0
#define CHOOSING_BOX_POSITION_Y 100
#define CHOOSING_BOX_POSITION_RANGE 15


bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

void updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
		USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
		USER_MESSAGE("Out of Scene")
	else if (user.isLost())
		USER_MESSAGE("Lost")

	g_visibleUsers[user.getId()] = user.isVisible();


	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")
			break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
			break;
		case nite::SKELETON_TRACKED:
			USER_MESSAGE("Tracking!")
			break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
			break;
		}
	}
}

int main(int argc, char** argv)
{
	nite::UserTracker userTracker;
	nite::Status niteRc;

	nite::NiTE::initialize();

	niteRc = userTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}
	printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");

	nite::UserTrackerFrameRef userTrackerFrame;
    printf("Box-Range X: %d ... %d\n", CHOOSING_BOX_POSITION_X - CHOOSING_BOX_POSITION_RANGE, CHOOSING_BOX_POSITION_X + CHOOSING_BOX_POSITION_RANGE);
    printf("Box-Range Y: %d ... %d\n", CHOOSING_BOX_POSITION_Y - CHOOSING_BOX_POSITION_RANGE, CHOOSING_BOX_POSITION_Y + CHOOSING_BOX_POSITION_RANGE);
	while (!wasKeyboardHit())
	{
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");

			continue;
		}

		const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
		nite::SkeletonJoint *chosen_hand = nullptr;


		for (int i = 0; i < users.getSize(); ++i)
		{
			const nite::UserData& user = users[i];
			updateUserState(user,userTrackerFrame.getTimestamp());
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}
			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				const nite::SkeletonJoint& left_hand = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
				const nite::SkeletonJoint& right_hand = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);

				if (right_hand.getPositionConfidence() > .5 || left_hand.getPositionConfidence() > .5)
                {
                    int left_x = left_hand.getPosition().x;
                    int left_y = left_hand.getPosition().y;
                    int right_x = right_hand.getPosition().x;
                    int right_y = right_hand.getPosition().y;

	    			printf("Left Hand: %d. (%5.2f, %5.2f)\n", user.getId(), left_hand.getPosition().x, left_hand.getPosition().y);
			    	printf("Right Hand: %d. (%5.2f, %5.2f)\n", user.getId(), right_hand.getPosition().x, right_hand.getPosition().y);

                    if (chosen_hand == nullptr)
                    {
                        if (left_x > (CHOOSING_BOX_POSITION_X - CHOOSING_BOX_POSITION_RANGE) &&
                            left_x < (CHOOSING_BOX_POSITION_X + CHOOSING_BOX_POSITION_RANGE) &&
                            left_y > (CHOOSING_BOX_POSITION_Y - CHOOSING_BOX_POSITION_RANGE) &&
                            left_y < (CHOOSING_BOX_POSITION_Y + CHOOSING_BOX_POSITION_RANGE))
                        {
                            *chosen_hand = left_hand;
                            printf("Left hand chosen");
                        }
                        else if (right_x > (CHOOSING_BOX_POSITION_X - CHOOSING_BOX_POSITION_RANGE) &&
                            right_x < (CHOOSING_BOX_POSITION_X + CHOOSING_BOX_POSITION_RANGE) &&
                            right_y > (CHOOSING_BOX_POSITION_Y - CHOOSING_BOX_POSITION_RANGE) &&
                            right_y < (CHOOSING_BOX_POSITION_Y + CHOOSING_BOX_POSITION_RANGE))
                        {
                            *chosen_hand = right_hand;
                            printf("Right hand chosen");
                        }
                    }
                    else
                    {
                        printf("Choose your playing hand by holding it within the box!");
                    }
                }
			}
		}
	}
	nite::NiTE::shutdown();
}

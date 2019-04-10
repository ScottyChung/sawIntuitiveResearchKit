/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsIntuitiveResearchKitPSM_h
#define _mtsIntuitiveResearchKitPSM_h

#include <cisstParameterTypes/prmActuatorJointCoupling.h>

#include <sawIntuitiveResearchKit/robManipulatorPSMSnake.h>
#include <sawIntuitiveResearchKit/mtsIntuitiveResearchKitArm.h>

// Always include last
#include <sawIntuitiveResearchKit/sawIntuitiveResearchKitExport.h>


class CISST_EXPORT mtsIntuitiveResearchKitPSM: public mtsIntuitiveResearchKitArm
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsIntuitiveResearchKitPSM(const std::string & componentName, const double periodInSeconds);
    mtsIntuitiveResearchKitPSM(const mtsTaskPeriodicConstructorArg & arg);
    inline ~mtsIntuitiveResearchKitPSM() {}

    void SetSimulated(void);
    void Configure(const std::string & filename);

protected:

    /*! Configuration methods */
    inline size_t NumberOfAxes(void) const {
        return 7;
    }

    inline size_t NumberOfJoints(void) const {
        return 7;
    }

    inline size_t NumberOfJointsKinematics(void) const {
        return mSnakeLike ? 8 : 6;
    }

    inline size_t NumberOfBrakes(void) const {
        return 0;
    }

    void UpdateJointsKinematics(void);
    void ToJointsPID(const vctDoubleVec &jointsKinematics, vctDoubleVec &jointsPID);

    robManipulator::Errno InverseKinematics(vctDoubleVec & jointSet,
                                            const vctFrm4x4 & cartesianGoal);

    // see base class
    inline bool IsSafeForCartesianControl(void) const {
        return (JointsKinematics.Position().at(2) > 50.0 * cmn_mm);
    }


    void Init(void);


    // state related methods
    void SetGoalHomingArm(void);
    void EnterArmHomed(void);
    void RunArmHomed(void); // mostly to allow joint control without tool nor adapter
    void LeaveArmHomed(void);
    void TransitionArmHomed(void);

    // engaging adapter
    void EnterChangingCouplingAdapter(void);
    inline void RunChangingCouplingAdapter(void) {
        RunChangingCoupling();
    }
    void EnterEngagingAdapter(void);
    void RunEngagingAdapter(void);
    void TransitionAdapterEngaged(void);
    // engaging tool
    void EnterChangingCouplingTool(void);
    inline void RunChangingCouplingTool(void) {
        RunChangingCoupling();
    }
    void EnterEngagingTool(void);
    void RunEngagingTool(void);
    void TransitionToolEngaged(void);

    void EnterManual(void);

    // shared method for changing coupling
    void RunChangingCoupling(void);

    void EventHandlerAdapter(const prmEventButton & button);

    /*! Set tool present.  This should only be used by the tool event
      handler or for custom tools that can't be detected
      automatically. */
    void SetAdapterPresent(const bool & present);
    void SetToolPresent(const bool & present);

    void EventHandlerTool(const prmEventButton & button);
    void EventHandlerManipClutch(const prmEventButton & button);

    void SetPositionJaw(const prmPositionJointSet & jawPosition);
    void SetPositionGoalJaw(const prmPositionJointSet & jawPosition);
    void SetEffortJaw(const prmForceTorqueJointSet & effort);

    void SetPositionJointLocal(const vctDoubleVec & newPosition);
    void SetEffortJointLocal(const vctDoubleVec & newEffort);

    void EnableJointsEventHandler(const vctBoolVec & enable);
    void CouplingEventHandler(const prmActuatorJointCoupling & coupling);

    /*! Event handlers for tools */
    //@{
    struct {
        mtsFunctionRead GetButton;
        bool IsPresent;
    } Adapter;

    struct {
        mtsFunctionRead GetButton;
        bool IsPresent;
    } Tool;
    //@}

    struct {
        mtsFunctionRead GetButton;
        bool IsPressed;
    } ManipClutch;

    // Functions for events
    struct {
        mtsFunctionWrite ManipClutch;
        std::string ManipClutchPreviousState;
        bool PIDEnabledPreviousState;
    } ClutchEvents;

    /*! 5mm tools with 8 joints */
    bool mSnakeLike = false;

    robManipulatorPSMSnake * ManipulatorPSMSnake = nullptr;
    robManipulator * ToolOffset = nullptr;
    vctFrm4x4 ToolOffsetTransformation;

    prmStateJoint Jaw, JawDesired;
    double JawGoal;
    double EffortJawSet;

    // Home Action
    unsigned int EngagingStage; // 0 requested
    unsigned int LastEngagingStage;

    bool mAdapterNeedEngage = false;
    bool mToolNeedEngage = false;

    struct {
        bool Started;
        std::string NextState;
        bool CouplingForTool;
        bool WaitingForEnabledJoints, ReceivedEnabledJoints;
        vctBoolVec LastEnabledJoints, DesiredEnabledJoints;
        bool WaitingForCoupling, ReceivedCoupling;
        prmActuatorJointCoupling LastCoupling, DesiredCoupling, ToolCoupling;
        vctDoubleVec ToolEngageLowerPosition, ToolEngageUpperPosition;
        vctDoubleVec ToolPositionLowerLimit, ToolPositionUpperLimit;
        vctDoubleVec NoToolPositionLowerLimit, NoToolPositionUpperLimit;
        vctDoubleVec ToolTorqueLowerLimit, ToolTorqueUpperLimit;
    } CouplingChange;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntuitiveResearchKitPSM);

#endif // _mtsIntuitiveResearchKitPSM_h

/*
 * Copyright (c) 2011-2021, The DART development contributors
 * All rights reserved.
 *
 * The list of contributors can be found at:
 *   https://github.com/dartsim/dart/blob/main/LICENSE
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#include <dart/dart.hpp>

const double default_speed_increment = 0.5;

const int default_ik_iterations = 4500;

const double default_force = 50.0; // N
const int default_countdown = 100; // Number of timesteps for applying force

using namespace dart::common;
using namespace dart::dynamics;
using namespace dart::simulation;
using namespace dart::gui;
using namespace dart::gui::osg;
using namespace dart::io;
using namespace dart::math;

class Controller
{
public:
  /// Constructor
  Controller(const SkeletonPtr& biped) : mBiped(biped), mSpeed(0.0)
  {
    int nDofs = mBiped->getNumDofs();

    mForces = Eigen::VectorXd::Zero(nDofs);

    mKp = Eigen::MatrixXd::Identity(nDofs, nDofs);
    mKd = Eigen::MatrixXd::Identity(nDofs, nDofs);

    for (std::size_t i = 0; i < 6; ++i) {
      mKp(i, i) = 0.0;
      mKd(i, i) = 0.0;
    }

    for (std::size_t i = 6; i < mBiped->getNumDofs(); ++i) {
      mKp(i, i) = 1000;
      mKd(i, i) = 50;
    }

    setTargetPositions(mBiped->getPositions());
  }

  /// Reset the desired dof position to the current position
  void setTargetPositions(const Eigen::VectorXd& pose)
  {
    mTargetPositions = pose;
  }

  /// Clear commanding forces
  void clearForces()
  {
    mForces.setZero();
  }

  /// Add commanding forces from PD controllers (Lesson 2 Answer)
  void addPDForces()
  {
    Eigen::VectorXd q = mBiped->getPositions();
    Eigen::VectorXd dq = mBiped->getVelocities();

    Eigen::VectorXd p = -mKp * (q - mTargetPositions);
    Eigen::VectorXd d = -mKd * dq;

    mForces += p + d;
    mBiped->setForces(mForces);
  }

  /// Add commanind forces from Stable-PD controllers (Lesson 3 Answer)
  void addSPDForces()
  {
    Eigen::VectorXd q = mBiped->getPositions();
    Eigen::VectorXd dq = mBiped->getVelocities();

    Eigen::MatrixXd invM
        = (mBiped->getMassMatrix() + mKd * mBiped->getTimeStep()).inverse();
    Eigen::VectorXd p
        = -mKp * (q + dq * mBiped->getTimeStep() - mTargetPositions);
    Eigen::VectorXd d = -mKd * dq;
    Eigen::VectorXd qddot = invM
                            * (-mBiped->getCoriolisAndGravityForces() + p + d
                               + mBiped->getConstraintForces());

    mForces += p + d - mKd * qddot * mBiped->getTimeStep();
    mBiped->setForces(mForces);
  }

  /// add commanding forces from ankle strategy (Lesson 4 Answer)
  void addAnkleStrategyForces()
  {
    Eigen::Vector3d COM = mBiped->getCOM();
    // Approximated center of pressure in sagittal axis
    Eigen::Vector3d offset(0.05, 0, 0);
    Eigen::Vector3d COP
        = mBiped->getBodyNode("h_heel_left")->getTransform() * offset;
    double diff = COM[0] - COP[0];

    Eigen::Vector3d dCOM = mBiped->getCOMLinearVelocity();
    Eigen::Vector3d dCOP
        = mBiped->getBodyNode("h_heel_left")->getLinearVelocity(offset);
    double dDiff = dCOM[0] - dCOP[0];

    int lHeelIndex = mBiped->getDof("j_heel_left_1")->getIndexInSkeleton();
    int rHeelIndex = mBiped->getDof("j_heel_right_1")->getIndexInSkeleton();
    int lToeIndex = mBiped->getDof("j_toe_left")->getIndexInSkeleton();
    int rToeIndex = mBiped->getDof("j_toe_right")->getIndexInSkeleton();
    if (diff < 0.1 && diff >= 0.0) {
      // Feedback rule for recovering forward push
      double k1 = 200.0;
      double k2 = 100.0;
      double kd = 10;
      mForces[lHeelIndex] += -k1 * diff - kd * dDiff;
      mForces[lToeIndex] += -k2 * diff - kd * dDiff;
      mForces[rHeelIndex] += -k1 * diff - kd * dDiff;
      mForces[rToeIndex] += -k2 * diff - kd * dDiff;
    } else if (diff > -0.2 && diff < -0.05) {
      // Feedback rule for recovering backward push
      double k1 = 2000.0;
      double k2 = 100.0;
      double kd = 100;
      mForces[lHeelIndex] += -k1 * diff - kd * dDiff;
      mForces[lToeIndex] += -k2 * diff - kd * dDiff;
      mForces[rHeelIndex] += -k1 * diff - kd * dDiff;
      mForces[rToeIndex] += -k2 * diff - kd * dDiff;
    }
    mBiped->setForces(mForces);
  }

  // Send velocity commands on wheel actuators (Lesson 6 Answer)
  void setWheelCommands()
  {
    int wheelFirstIndex
        = mBiped->getDof("joint_front_left_1")->getIndexInSkeleton();
    for (std::size_t i = wheelFirstIndex; i < mBiped->getNumDofs(); ++i) {
      mKp(i, i) = 0.0;
      mKd(i, i) = 0.0;
    }

    int index1 = mBiped->getDof("joint_front_left_2")->getIndexInSkeleton();
    int index2 = mBiped->getDof("joint_front_right_2")->getIndexInSkeleton();
    int index3 = mBiped->getDof("joint_back_left")->getIndexInSkeleton();
    int index4 = mBiped->getDof("joint_back_right")->getIndexInSkeleton();
    mBiped->setCommand(index1, mSpeed);
    mBiped->setCommand(index2, mSpeed);
    mBiped->setCommand(index3, mSpeed);
    mBiped->setCommand(index4, mSpeed);
  }

  void changeWheelSpeed(double increment)
  {
    mSpeed += increment;
    std::cout << "wheel speed = " << mSpeed << std::endl;
  }

protected:
  /// The biped Skeleton that we will be controlling
  SkeletonPtr mBiped;

  /// Joint forces for the biped (output of the Controller)
  Eigen::VectorXd mForces;

  /// Control gains for the proportional error terms in the PD controller
  Eigen::MatrixXd mKp;

  /// Control gains for the derivative error terms in the PD controller
  Eigen::MatrixXd mKd;

  /// Target positions for the PD controllers
  Eigen::VectorXd mTargetPositions;

  /// For velocity actuator: Current speed of the skateboard
  double mSpeed;
};

class TutorialWorldNode : public dart::gui::osg::RealTimeWorldNode
{
public:
  TutorialWorldNode(WorldPtr world)
    : dart::gui::osg::RealTimeWorldNode(std::move(world)),
      mForceCountDown(0),
      mPositiveSign(true)
  {
    mController = std::make_unique<Controller>(mWorld->getSkeleton("biped"));
  }

  void customPreStep() override
  {
    mController->clearForces();

    // Lesson 3
    mController->addSPDForces();

    // Lesson 4
    mController->addAnkleStrategyForces();

    // Lesson 6
    mController->setWheelCommands();

    // Apply body forces based on user input, and color the body shape red
    if (mForceCountDown > 0) {
      BodyNode* bn = mWorld->getSkeleton("biped")->getBodyNode("h_abdomen");
      auto shapeNodes = bn->getShapeNodesWith<VisualAspect>();
      shapeNodes[0]->getVisualAspect()->setColor(dart::Color::Red());

      if (mPositiveSign)
        bn->addExtForce(
            default_force * Eigen::Vector3d::UnitX(),
            bn->getCOM(),
            false,
            false);
      else
        bn->addExtForce(
            -default_force * Eigen::Vector3d::UnitX(),
            bn->getCOM(),
            false,
            false);

      --mForceCountDown;
    }
  }

  Controller* getController()
  {
    return mController.get();
  }

  void setForceCountDown(int value)
  {
    mForceCountDown = value;
  }

  void setPositiveSign(bool value)
  {
    mPositiveSign = value;
  }

protected:
  /// Controller
  std::unique_ptr<Controller> mController;

  /// Number of iterations before clearing a force entry
  int mForceCountDown;

  /// Whether a force should be applied in the positive or negative direction
  bool mPositiveSign;
};

class InputHandler : public ::osgGA::GUIEventHandler
{
public:
  /// Constructor
  InputHandler(TutorialWorldNode* node) : mNode(node)
  {
    // Do nothing
  }

  /// Handles key events
  bool handle(
      const ::osgGA::GUIEventAdapter& ea, ::osgGA::GUIActionAdapter&) override
  {
    if (::osgGA::GUIEventAdapter::KEYDOWN == ea.getEventType()) {
      switch (ea.getKey()) {
        case ',':
          mNode->setForceCountDown(default_countdown);
          mNode->setPositiveSign(false);
          return true;
        case '.':
          mNode->setForceCountDown(default_countdown);
          mNode->setPositiveSign(true);
          return true;
        case 'a':
          mNode->getController()->changeWheelSpeed(default_speed_increment);
          return true;
        case 's':
          mNode->getController()->changeWheelSpeed(-default_speed_increment);
          return true;
      }
    }

    return false;
  }

protected:
  TutorialWorldNode* mNode;
};

// Load a biped model and enable joint limits and self-collision
// (Lesson 1 Answer)
SkeletonPtr loadBiped()
{
  // Create the world with a skeleton
  WorldPtr world = SkelParser::readWorld("dart://sample/skel/biped.skel");
  assert(world != nullptr);

  SkeletonPtr biped = world->getSkeleton("biped");

  // Set joint limits
  for (std::size_t i = 0; i < biped->getNumJoints(); ++i)
    biped->getJoint(i)->setLimitEnforcement(true);

  // Enable self collision check but ignore adjacent bodies
  biped->enableSelfCollisionCheck();
  biped->disableAdjacentBodyCheck();

  return biped;
}

// Set initial configuration (Lesson 2 Answer)
void setInitialPose(SkeletonPtr biped)
{
  biped->setPosition(
      biped->getDof("j_thigh_left_z")->getIndexInSkeleton(), 0.15);
  biped->setPosition(
      biped->getDof("j_thigh_right_z")->getIndexInSkeleton(), 0.15);
  biped->setPosition(biped->getDof("j_shin_left")->getIndexInSkeleton(), -0.4);
  biped->setPosition(biped->getDof("j_shin_right")->getIndexInSkeleton(), -0.4);
  biped->setPosition(
      biped->getDof("j_heel_left_1")->getIndexInSkeleton(), 0.25);
  biped->setPosition(
      biped->getDof("j_heel_right_1")->getIndexInSkeleton(), 0.25);
}

// Load a skateboard model and connect it to the biped model via an Euler joint
// (Lesson 5 Answer)
void modifyBipedWithSkateboard(SkeletonPtr biped)
{
  // Load the Skeleton from a file
  WorldPtr world = SkelParser::readWorld("dart://sample/skel/skateboard.skel");

  SkeletonPtr skateboard = world->getSkeleton(0);

  EulerJoint::Properties properties = EulerJoint::Properties();
  properties.mT_ChildBodyToJoint.translation() = Eigen::Vector3d(0, 0.1, 0);

  skateboard->getRootBodyNode()->moveTo<EulerJoint>(
      biped->getBodyNode("h_heel_left"), properties);
}

// Set the actuator type for four wheel joints to "VELOCITY" (Lesson 6 Answer)
void setVelocityAccuators(SkeletonPtr biped)
{
  Joint* wheel1 = biped->getJoint("joint_front_left");
  Joint* wheel2 = biped->getJoint("joint_front_right");
  Joint* wheel3 = biped->getJoint("joint_back_left");
  Joint* wheel4 = biped->getJoint("joint_back_right");
  wheel1->setActuatorType(Joint::VELOCITY);
  wheel2->setActuatorType(Joint::VELOCITY);
  wheel3->setActuatorType(Joint::VELOCITY);
  wheel4->setActuatorType(Joint::VELOCITY);
}

// Solve for a balanced pose using IK (Lesson 7 Answer)
Eigen::VectorXd solveIK(SkeletonPtr biped)
{
  // Modify the intial pose to one-foot stance before IK
  biped->setPosition(biped->getDof("j_shin_right")->getIndexInSkeleton(), -1.4);
  biped->setPosition(
      biped->getDof("j_bicep_left_x")->getIndexInSkeleton(), 0.8);
  biped->setPosition(
      biped->getDof("j_bicep_right_x")->getIndexInSkeleton(), -0.8);

  Eigen::VectorXd newPose = biped->getPositions();
  BodyNodePtr leftHeel = biped->getBodyNode("h_heel_left");
  BodyNodePtr leftToe = biped->getBodyNode("h_toe_left");
  double initialHeight = -0.8;

  for (std::size_t i = 0; i < default_ik_iterations; ++i) {
    Eigen::Vector3d deviation = biped->getCOM() - leftHeel->getCOM();
    Eigen::Vector3d localCOM = leftHeel->getCOM(leftHeel);
    LinearJacobian jacobian = biped->getCOMLinearJacobian()
                              - biped->getLinearJacobian(leftHeel, localCOM);

    // Sagittal deviation
    double error = deviation[0];
    Eigen::VectorXd gradient = jacobian.row(0);
    Eigen::VectorXd newDirection = -0.2 * error * gradient;

    // Lateral deviation
    error = deviation[2];
    gradient = jacobian.row(2);
    newDirection += -0.2 * error * gradient;

    // Position constraint on four (approximated) corners of the left foot
    Eigen::Vector3d offset(0.0, -0.04, -0.03);
    error = (leftHeel->getTransform() * offset)[1] - initialHeight;
    gradient = biped->getLinearJacobian(leftHeel, offset).row(1);
    newDirection += -0.2 * error * gradient;

    offset[2] = 0.03;
    error = (leftHeel->getTransform() * offset)[1] - initialHeight;
    gradient = biped->getLinearJacobian(leftHeel, offset).row(1);
    newDirection += -0.2 * error * gradient;

    offset[0] = 0.04;
    error = (leftToe->getTransform() * offset)[1] - initialHeight;
    gradient = biped->getLinearJacobian(leftToe, offset).row(1);
    newDirection += -0.2 * error * gradient;

    offset[2] = -0.03;
    error = (leftToe->getTransform() * offset)[1] - initialHeight;
    gradient = biped->getLinearJacobian(leftToe, offset).row(1);
    newDirection += -0.2 * error * gradient;

    newPose += newDirection;
    biped->setPositions(newPose);
    biped->computeForwardKinematics(true, false, false);
  }
  return newPose;
}

SkeletonPtr createFloor()
{
  SkeletonPtr floor = Skeleton::create("floor");

  // Give the floor a body
  BodyNodePtr body
      = floor->createJointAndBodyNodePair<WeldJoint>(nullptr).second;

  // Give the body a shape
  double floor_width = 10.0;
  double floor_height = 0.01;
  std::shared_ptr<BoxShape> box(
      new BoxShape(Eigen::Vector3d(floor_width, floor_height, floor_width)));
  auto shapeNode = body->createShapeNodeWith<
      VisualAspect,
      CollisionAspect,
      DynamicsAspect>(box);
  shapeNode->getVisualAspect()->setColor(dart::Color::Black());

  // Put the body into position
  Eigen::Isometry3d tf(Eigen::Isometry3d::Identity());
  tf.translation() = Eigen::Vector3d(0.0, -1.0, 0.0);
  body->getParentJoint()->setTransformFromParentBodyNode(tf);

  return floor;
}

int main()
{
  SkeletonPtr floor = createFloor();

  // Lesson 1
  SkeletonPtr biped = loadBiped();

  // Lesson 2
  setInitialPose(biped);

  // Lesson 5
  modifyBipedWithSkateboard(biped);

  // Lesson 6
  setVelocityAccuators(biped);

  // Lesson 7
  Eigen::VectorXd balancedPose = solveIK(biped);
  biped->setPositions(balancedPose);

  WorldPtr world = std::make_shared<World>();
  world->setGravity(Eigen::Vector3d(0.0, -9.81, 0.0));

  if (dart::dynamics::CollisionDetector::getFactory()->canCreate("bullet")) {
    world->getConstraintSolver()->setCollisionDetector(
        dart::dynamics::CollisionDetector::getFactory()->create("bullet"));
  }

  world->addSkeleton(floor);
  world->addSkeleton(biped);

  ::osg::ref_ptr<TutorialWorldNode> node = new TutorialWorldNode(world);

  // Create a viewer for rendering the world and handling user input
  dart::gui::osg::Viewer viewer;
  viewer.setWindowTitle("Biped Tutorial");

  // Prevent this World from simulating
  viewer.addWorldNode(node);

  // Add our custom input handler to the Viewer
  viewer.addEventHandler(new InputHandler(node.get()));

  // Print out instructions for the viewer
  std::cout << viewer.getInstructions() << std::endl;

  // Print instructions
  std::cout << "'.': forward push" << std::endl;
  std::cout << "',': backward push" << std::endl;
  std::cout << "'s': increase skateboard forward speed" << std::endl;
  std::cout << "'a': increase skateboard backward speed" << std::endl;
  std::cout << "space bar: simulation on/off" << std::endl;
  std::cout << "'p': replay simulation" << std::endl;
  std::cout << "'v': Turn contact force visualization on/off" << std::endl;
  std::cout << "'[' and ']': replay one frame backward and forward"
            << std::endl;

  // Set up the window
  viewer.setUpViewInWindow(0, 0, 640, 480);

  // Set up the default viewing position
  viewer.getCameraManipulator()->setHomePosition(
      ::osg::Vec3(5.34f, 3.00f, 2.41f), // eye
      ::osg::Vec3(0.f, 0.f, 0.f),       // center
      ::osg::Vec3(0.f, 1.f, 0.f));      // up

  // Reset the camera manipulator so that it starts in the new viewing position
  viewer.setCameraManipulator(viewer.getCameraManipulator());

  // Run the Viewer
  viewer.run();
}

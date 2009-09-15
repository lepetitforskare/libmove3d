/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Thu Jul 24 10:40:53 2008
 */
#ifndef __CEXTRACT__


extern int gpGet_arm_base_frame ( p3d_rob *robot, p3d_matrix4 frame );

extern int gpGet_platform_frame(p3d_rob *robot, p3d_matrix4 frame);

extern int gpGet_wrist_frame ( p3d_rob *robot, p3d_matrix4 frame );

extern void gpDraw_friction_cone ( p3d_vector3 c, p3d_vector3 normal, double mu, int nb_slices, double length );

extern void gpDraw_friction_cone2(p3d_vector3 c, p3d_vector3 normal, double mu, int nb_slices, double length);

extern configPt gpRandom_robot_base(p3d_rob *robot, double innerRadius, double outerRadius, p3d_vector3 objLoc);

extern int gpGet_SAHfinger_joint_angles(p3d_rob *robot, gpHand_properties &hand_properties, double q[4], int finger_index);

extern int gpSet_SAHfinger_joint_angles(p3d_rob *robot, gpHand_properties &hand_properties, double q[4], int finger_index);

extern int gpSAHfinger_forward_kinematics(p3d_matrix4 Twrist, gpHand_properties &hand, double q[4], p3d_vector3 p, p3d_vector3 fingerpad_normal, int finger_index);

extern int gpSAHfinger_inverse_kinematics(p3d_matrix4 Twrist, gpHand_properties &hand, p3d_vector3 p, double q[4], p3d_vector3 fingerpad_normal, int finger_index);

extern int gpOpen_hand(p3d_rob *robot, gpHand_properties &hand);

extern int gpLock_platform(p3d_rob *robot);

extern int gpUnlock_platform(p3d_rob *robot);

extern int gpLock_arm(p3d_rob *robot, gpArm_type arm_type);

extern int gpUnlock_arm(p3d_rob *robot, gpArm_type arm_type);

extern int gpLock_hand(p3d_rob *robot, gpHand_type hand_type);

extern int gpUnlock_hand(p3d_rob *robot, gpHand_type hand_type);

extern int gpGet_platform_configuration(p3d_rob *robot, double &x, double &y, double &theta);

extern int gpSet_platform_configuration(p3d_rob *robot, double x, double y, double theta);

extern int gpGet_arm_configuration(p3d_rob *robot, gpArm_type arm_type, double &q1, double &q2, double &q3, double &q4, double &q5, double &q6);

extern int gpSet_arm_configuration(p3d_rob *robot, gpArm_type arm_type, double q1, double q2, double q3, double q4, double q5, double q6, bool verbose= false);

extern int gpSet_grasp_configuration(p3d_rob *robot, gpHand_properties &hand, const gpGrasp &grasp);

extern int gpGet_hand_configuration(p3d_rob *robot, gpHand_properties &hand, std::vector<double> q);

extern int gpSet_hand_configuration(p3d_rob *robot, gpHand_properties &hand, std::vector<double> q, bool verbose= false);

extern int gpFold_arm(p3d_rob *robot, gpArm_type arm_type);

extern int gpDeactivate_arm_collisions(p3d_rob *robot);

extern int gpActivate_arm_collisions(p3d_rob *robot);

extern int gpDeactivate_hand_collisions(p3d_rob *robot);

extern int gpActivate_hand_collisions(p3d_rob *robot);

extern int gpDeactivate_object_fingertips_collisions(p3d_rob *robot, p3d_obj *object, gpHand_properties &hand);

extern int gpActivate_object_fingertips_collisions(p3d_rob *robot, p3d_obj *object, gpHand_properties &hand);

extern int gpDeactivate_finger_collisions(p3d_rob *robot, unsigned int finger_index, gpHand_properties &hand);

extern int gpActivate_finger_collisions(p3d_rob *robot, unsigned int finger_index, gpHand_properties &hand);


#endif /* __CEXTRACT__ */

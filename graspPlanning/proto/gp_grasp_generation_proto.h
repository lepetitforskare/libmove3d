
#ifndef __CEXTRACT__

extern p3d_matrix4 *gpSample_grasp_frames(p3d_vector3 cmass, p3d_matrix3 iaxes, double iaabb[6], double translationStep, unsigned int nbDirections, double rotationStep, unsigned int *nbSamples);

extern int gpGrasps_from_grasp_frame(p3d_rob *robot, p3d_obj *object, unsigned int part, p3d_matrix4 gFrame, gpHand_properties &hand, std::list<class gpGrasp> &graspList);

extern int gpGrasps_from_grasp_frame_gripper(p3d_polyhedre *polyhedron, unsigned int part, p3d_matrix4 gFrame, gpHand_properties &hand, std::list<class gpGrasp> &graspList);

extern int gpGrasps_from_grasp_frame_SAHand(p3d_rob *robot, p3d_obj *object, unsigned int part, p3d_matrix4 gFrame, gpHand_properties &hand, std::list<class gpGrasp> &graspList);

extern int gpGrasp_frame_from_inertia_axes ( p3d_matrix3 iaxes, p3d_vector3 cmass, int direction, double displacement, int axis, double angle, p3d_matrix4 gframe );

extern int gpGrasp_generation(p3d_rob *robot, p3d_obj *object, int part, p3d_vector3 cmass, p3d_matrix3 iaxes, double iaabb[6], gpHand_properties &hand, double translationStep, unsigned int nbDirections, double rotationStep, std::list<class gpGrasp> &graspList);


extern int gpInverse_geometric_model_freeflying_hand(p3d_rob *robot, p3d_matrix4 objectFrame, p3d_matrix4 graspFrame, gpHand_properties &hand, configPt q);

extern int gpForward_geometric_model_PA10(p3d_rob *robot, p3d_matrix4 Tend_eff, bool display);

extern int gpInverse_geometric_model_PA10(p3d_rob *robot, p3d_matrix4 Tend_eff, configPt q);

extern int gpGrasp_collision_filter(std::list<gpGrasp> &graspList, p3d_rob *robot, p3d_obj *object, gpHand_properties &hand);

extern int gpGrasp_context_collision_filter(std::list<gpGrasp> &graspList, p3d_rob *robot, p3d_obj *object, gpHand_properties &hand);

extern int gpGrasp_stability_filter(std::list<gpGrasp> &graspList);

extern configPt gpFind_grasp_from_base_configuration(p3d_rob *robot, p3d_obj *object, std::list<gpGrasp> &graspList, gpArm_type arm_type, configPt qbase, gpGrasp &grasp, gpHand_properties &hand);

#endif

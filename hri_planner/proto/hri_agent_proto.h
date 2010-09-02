extern int hri_assign_global_agents(HRI_AGENTS *agents);
extern HRI_AGENT *  hri_assign_source_agent(char *agent_name, HRI_AGENTS *agents);
extern HRI_AGENTS * hri_create_agents(void);
extern int hri_destroy_agents(HRI_AGENTS *agents);
extern HRI_AGENT * hri_create_agent(p3d_rob * robot);
extern int hri_destroy_agent(HRI_AGENT *agent);
extern HRI_MANIP * hri_create_empty_agent_manip();
extern HRI_MANIP * hri_create_agent_manip(HRI_AGENT * agent);
extern int hri_destroy_agent_manip(HRI_MANIP *manip);
extern HRI_NAVIG * hri_create_agent_navig(HRI_AGENT * agent);
extern int hri_destroy_agent_navig(HRI_NAVIG *navig);
extern HRI_PERSP * hri_create_agent_perspective(HRI_AGENT * agent);
extern int hri_destroy_agent_perspective(HRI_PERSP *persp);
extern int hri_fill_all_agents_default_tasks(HRI_AGENTS * agents);
extern int hri_create_fill_agent_default_manip_tasks(GIK_TASK ** tasklist, int * tasklist_no, HRI_AGENT_TYPE type);
extern int hri_create_assign_default_manipulation(HRI_AGENTS * agents);
extern int hri_agent_single_task_manip_move(HRI_AGENT * agent, HRI_GIK_TASK_TYPE type, p3d_vector3 * goalCoord, configPt *q);
extern int g3d_hri_display_shared_zone();
extern int hri_agent_load_default_arm_posture(HRI_AGENT * agent, configPt q);
extern int hri_agent_compute_posture(HRI_AGENT * agent, double neck_height, int state, configPt q);
extern int hri_agent_compute_state_posture(HRI_AGENT * agent, int state, configPt q);
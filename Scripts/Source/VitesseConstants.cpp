#include "VitesseConstants.h"

using namespace AbyssEngine;

const VitesseConstants::Thruster VitesseConstants::Thrusters[static_cast<int>(Thruster::Location::Installed_Units)] =
{
	//ソケット名						//座標				//回転				//スケール
	{"rig_robo_J_backpack_end_L",	{0.0f,0.05f,0.0f},	{0.0f,0.0f,-68.0f},	{0.3f,-0.3f,-0.3f}},//BackPack_R_U,//バックパック右上
	{"rig_robo_J_backpack_end_L",	{0.0f,0.1f,0.1f},	{0.0f,0.0f,-93.0f},	{0.3f,-0.3f,-0.3f}},//BackPack_R_D,//バックパック右下
	{"rig_robo_J_backpack_end_R",	{0.0f,-0.05f,0.0f},	{0.0f,0.0f,68.0f},	{0.3f,0.3f,0.3f}},//BackPack_L_U,//バックパック右上
	{"rig_robo_J_backpack_end_R",	{0.0f,-0.1f,-0.1f},	{0.0f,0.0f,93.0f},	{0.3f,0.3f,0.3f}},//BackPack_L_D,//バックパック右下
	{"rig_J_upleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_U,//右膝裏あたり
	{"rig_J_upleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_U,//左膝裏あたり
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_M,//右ふくらはぎ中間
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_R_D,//右ふくらはぎ下側
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_M,//左ふくらはぎ中間
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//BackLeg_L_D,//左ふくらはぎ下側
	{"rig_J_upleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Waist_R,//腰右側
	{"rig_J_upleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Waist_L,//腰左側
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_R_R,//右膝、右側
	{"rig_J_lowleg_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_R_L,//右膝、左側
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_L_R,//左膝、右側
	{"rig_J_lowleg_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Knee_L_L,//左膝、左側
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_1,//右肩1番一番上のバーニアから時計回りに1,2,3番
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_2,//右肩2番一番上のバーニアから時計回りに1,2,3番
	{"rig_sub02J_shoulder_L",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_R_3,//右肩3番一番上のバーニアから時計回りに1,2,3番
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_1,//左肩1番一番上のバーニアから時計回りに1,2,3番
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_2,//左肩2番一番上のバーニアから時計回りに1,2,3番
	{"rig_sub02J_shoulder_R",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Shoulder_L_3,//左肩3番一番上のバーニアから時計回りに1,2,3番
	{"rig_J_upbody01",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Chest_R,//胸部右側
	{"rig_J_upbody01",	{0.0f,0.0f,0.0f},	{0.0f,0.0f,0.0f},	{0.3f,0.3f,0.3f}},//Chest_L,//胸部左側
};

























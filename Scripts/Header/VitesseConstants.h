#pragma once
#include "MathHelper.h"
#include <string>

class VitesseConstants
{
public:
	struct Thruster
	{
		std::string socketName_;

		//AbyssEngine::Vector3 offsetPos_;
		//AbyssEngine::Vector3 offsetRot_;
		//AbyssEngine::Vector3 offsetScale_;

		//スラスター各部位
		enum class Location
		{
			BackPack_R_U,//バックパック右上
			BackPack_R_D,//バックパック右下
			BackPack_L_U,//バックパック右上
			BackPack_L_D,//バックパック右下

			BackLeg_R_U,//右膝裏あたり
			BackLeg_L_U,//左膝裏あたり

			BackLeg_R_M,//右ふくらはぎ中間
			BackLeg_R_D,//右ふくらはぎ下側
			BackLeg_L_M,//左ふくらはぎ中間
			BackLeg_L_D,//左ふくらはぎ下側

			Waist_R,//腰右側
			Waist_L,//腰左側

			Knee_R_R,//右膝、右側
			Knee_R_L,//右膝、左側
			Knee_L_R,//左膝、右側
			Knee_L_L,//左膝、左側

			Shoulder_R_1,//右肩1番一番上のバーニアから時計回りに1,2,3番
			//Shoulder_R_2,//右肩2番一番上のバーニアから時計回りに1,2,3番
			//Shoulder_R_3,//右肩3番一番上のバーニアから時計回りに1,2,3番
			Shoulder_L_1,//左肩1番一番上のバーニアから時計回りに1,2,3番
			//Shoulder_L_2,//左肩2番一番上のバーニアから時計回りに1,2,3番
			//Shoulder_L_3,//左肩3番一番上のバーニアから時計回りに1,2,3番

			Chest_R,//胸部右側
			Chest_L,//胸部左側

			Installed_Units,//搭載基数
		};
	};
	static const Thruster Thrusters[static_cast<int>(Thruster::Location::Installed_Units)];
};

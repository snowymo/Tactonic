// calculate para * (0,0,0,1)

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
	std::fstream file,outputFile;
	file.open ("LeapCamCalib.txt");
	outputFile.open ("LeapCamOutput.txt");
	std::string str;
	glm::mat4x4 gl_para[10];
	glm::vec4 camPos[10];
	glm::vec4 unit = glm::vec4(0,0,0,1);
	int dataCount = 10;
	double dd;
	
	for(int i = 0; i < dataCount; i++){
		file >> str;	// gl_paraN
		for(int j = 0; j < 16; j++){
			file >> gl_para[i][j/4][j%4];
		}
		// calculate camera position in 3d
		outputFile  << "CamPos:\n";
		camPos[i] = gl_para[i] * unit;
		for(int j = 0; j < 4; j++)
			outputFile << camPos[i][j] << "\t";
		
		outputFile  << "\nLeapMotion:\n";
		file >> str;	// leap motion N
		file >> dd;		// x
		outputFile  << dd << "\t";
		file >> str;	// ,
		file >> dd;		// y
		outputFile  << dd << "\t";
		file >> str;	// ,
		file >> dd;		// z
		outputFile  << dd << "\n";		
	}
	outputFile.close ();
	file.close ();

	// test transformation
	glm::vec4 leapPos = glm::vec4(2.81554	,238.856	,-13.3523,1);
	//glm::vec4 camPos = glm::vec4(4.32622,3.45351,526.571,1);
	glm::mat4x4 transformation = glm::mat4x4(
		-0.314993,  0.0234792,  -0.948803,   -15.5061,
		-0.932507,   0.178421,   0.313998,   -30.4811,
		0.176659,   0.983674, -0.0343071,   -36.4951,
		0,          0,          0,          1
		);
	glm::vec4 calCamPos = leapPos * transformation;
	return 0;
}
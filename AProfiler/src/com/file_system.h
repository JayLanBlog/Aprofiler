#pragma once
#include <io.h>
#include <fstream>

#include <vector>
#include <iostream>
using namespace std;
namespace APROFILER {

	namespace SYS {
	
		void FindAllFiles(string path, vector<string>& files);

		void GetTargetFileFromPath(string path, vector<string>& files, string str, string& targetFile);
	
	}

}
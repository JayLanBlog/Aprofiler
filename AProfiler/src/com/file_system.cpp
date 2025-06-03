#include "file_system.h"

using namespace std;
namespace APROFILER {

	namespace SYS {

		void FindAllFiles(string path, vector<string>& files) {
			//�ļ����
			long long hFile = 0;
			//�ļ���Ϣ
			struct _finddata_t fileinfo;
			string p;
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {  //�Ƚ��ļ������Ƿ����ļ���
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
							files.push_back(p.assign(path).append("\\").append(fileinfo.name));
							//�ݹ�����
							FindAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
						}
					}
					else {
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					}
				} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
				_findclose(hFile);
			}
		}

		void GetTargetFileFromPath(string path, vector<string>& files, string str, string& targetFile) {
			long long hFile = 0;
			//�ļ���Ϣ
			struct _finddata_t fileinfo;
			string p;
			if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
				do {
					if ((fileinfo.attrib & _A_SUBDIR)) {  //�Ƚ��ļ������Ƿ����ļ���
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
							files.push_back(p.assign(path).append("\\").append(fileinfo.name));
							//�ݹ�����
							GetTargetFileFromPath(p.assign(path).append("\\").append(fileinfo.name), files, str, targetFile);
						}
					}
					else {
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
						if (p.find(str) != string::npos)
						{
							targetFile = p;
						}
					}
				} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
				_findclose(hFile);
			}

		}
	}
}
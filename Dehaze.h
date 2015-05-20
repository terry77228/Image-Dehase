#include <string>
#include <vector>

using namespace std;
class Dehazer{

public:
	bool LoadImage(const std::string& _filename);
	bool Dehaze(const int& _patchsize, const double& _t, const double& _w);
	bool WriteImage(const std::string& _filename);

private:
	vector<int> m_Image[3];
};



int dehaze(char * );
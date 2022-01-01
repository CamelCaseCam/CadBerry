#include <string>
#include <iostream>
#include <filesystem>

#include <cpr/cpr.h>

int main(int argc, char** argv)
{
	Sleep(3000);
	std::cout << "Starting CadBerry updater..." << std::endl;
	std::cout << argc << std::endl;
	if (argc < 4)
	{
		std::cout << "You must provide the github repository, path to executable, and number of files as arguments" << std::endl;
		return 0;
	}
	std::string GitHubPath = argv[1];
	std::string BinPath = argv[2];
	std::cout << argv[3] << std::endl;
	int NumFiles = std::stoi(argv[3]);

	for (int i = 0; i < NumFiles; ++i)
	{
		std::string FilePath = argv[4 + i];
		std::ofstream OutputFile(BinPath + FilePath, std::ios::binary);
		cpr::Response r = cpr::Download(OutputFile, cpr::Url(GitHubPath + FilePath));
		std::cout << "Updated 1 file with size " << r.downloaded_bytes / 1000 << "kb from source " << GitHubPath + FilePath << std::endl;
	}
	std::cout << "Done updating " << NumFiles << " files. Feel free to close this window";
	std::string buff;
	std::cin >> buff;

	return 0;
}

//https://github.com/CamelCaseCam/CadBerry/raw/developer-branch/CadBerry/bin/Release-windows-x86_64/Berry/Berry.exe
//https://github.com/CamelCaseCam/CadBerry/raw/developer-branch/CadBerry/bin/Debug-windows-x86_64/Berry/Berry.exe
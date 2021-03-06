// mailing-list-cpp
// https://github.com/aenami/mailing-list-cpp
//

#include "pch.h"
std::string random_string(size_t length)
{
	auto randchar = []() -> char {
		const char charset[] = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	srand(time(NULL));
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

int RandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> randomin(min, max);
	return static_cast<int>(randomin(rd));
}

std::vector<std::string> mail_list;

int main(int argc, char** argv)
{
	std::string folder_path = "temp/";
	namespace fs = std::experimental::filesystem;
	if (!fs::exists(folder_path) || !fs::is_directory(folder_path))
		fs::create_directory(folder_path);
	std::string mail_to_send = "mail.msg", temp_mail;
	if (!fs::exists(mail_to_send) || !fs::exists("input.txt"))
	{
		std::cout << termcolor::red << "ERROR: " << termcolor::white << "Mail file (" << mail_to_send << ") or email address file (input.txt) not found!" << std::endl;
		std::cout << "Please put these files into the same folder as program and rerun!" << std::endl;
		std::getchar();
		return 1;
	}
	std::string script_name = "script.ps1";
	int success = 0, failed = 0;
	std::ifstream in("input.txt");
	if (!in.good())
		return 1;
	std::cout << "\rParsing input file...";
	while (!in.eof())
	{
		std::string cur;
		if (in >> cur)
			if (cur.find('@') != std::string::npos)
			{
				mail_list.push_back(cur);
				std::cout << termcolor::white << "\rParsing input file... (" << termcolor::green << std::to_string(mail_list.size()) << termcolor::white << " address read)";
			}
	}

	std::cout << std::endl << std::flush << std::endl << std::flush;

	for (int i = 0; i < mail_list.size(); i++)
	{
		temp_mail = folder_path + random_string(RandomInt(3, 10)) + ".msg";
		fs::copy(mail_to_send, temp_mail);
		auto adr = mail_list.at(i);
		std::cout << "Sending mail to: " << std::quoted(adr) << " [" << termcolor::cyan << std::to_string(int(i + 1)) << termcolor::white << " of " << termcolor::green << std::to_string(mail_list.size()) << termcolor::white << "] ... ";
		std::ofstream script(std::string(folder_path + script_name)); script 
			   << "$outlook = New-Object -comObject Outlook.Application" << std::endl                          // Creates Outlook interface
			   << "$mail = $outlook.Session.OpenSharedItem(" << std::quoted(temp_mail) << ")" << std::endl     // Opens the specified .msg file
			   << "$mail.Forward()" << std::endl                                                               // Sets the mail state to "forward"
			   << "$mail.Recipients.Add(" << std::quoted(adr) << ")" << std::endl                              // Add the specified address to the mail recipients list
			   << "$mail.Send()" << std::endl                                                                  // Sends the mail
			   << "[System.Runtime.InteropServices.Marshal]::ReleaseComObject($mail) | Out-Null" << std::endl  // Close the mail object (prevents "file is locked" error)
			   << "exit $LASTEXITCODE" << std::endl;                                                           // Returns error code from the script
		script.close();
		std::stringstream f; f << "-ExecutionPolicy Bypass -File " << std::quoted(script_name) << " >$null 2>&1";
		int err = system(std::string("powershell " + f.str()).c_str());
		if (err == 0)
		{
			success++;
			std::cout << termcolor::green << "success!" << termcolor::white << std::endl;
		}
		else
		{
			failed++;
			std::cout << termcolor::red << "failed!" << termcolor::white << std::endl;
		}
		fs::remove(temp_mail);
		fs::remove(std::string(folder_path + script_name));
	}
	std::cout << std::endl;
	std::cout << "Program finished, success: " << termcolor::green << std::to_string(success) << termcolor::white << ", failed: " << termcolor::red << std::to_string(failed) << termcolor::white << ", total: " << termcolor::cyan << std::to_string(mail_list.size()) << termcolor::white << std::endl;
	fs::remove(folder_path);
	std::getchar();
	return 0;
}


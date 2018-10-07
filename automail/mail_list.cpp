// mailing-list-cpp
// https://github.com/minajune/mailing-list-cpp
//

#include "pch.h"

std::vector<std::string> mail_list;

int main(int argc, char** argv)
{
	std::string mail_to_send = "test.msg";
	std::string script_name = "script.ps1";
	std::ifstream in("input.txt");
	if (!in.good())
		return 0;
	std::cout << "\rParsing input file...";
	while (!in.eof())
	{
		std::string cur;
		if (in >> cur)
			if (cur.find('@') != std::string::npos)
			{
				mail_list.push_back(cur);
				std::cout << "\rParsing input file... (" << std::to_string(mail_list.size()) << " address read)";
			}
	}

	std::cout << std::endl << std::flush;

	for (auto adr : mail_list)
	{
		std::cout << "Sending mail to: " << std::quoted(adr) << std::endl;

		std::ofstream script(script_name);
		script << "$outlook = New-Object -comObject Outlook.Application" << std::endl                          // Creates Outlook interface
			   << "$mail = $outlook.Session.OpenSharedItem(" << std::quoted(mail_to_send) << ")" << std::endl  // Opens the specified .msg file
			   << "$mail.Forward()" << std::endl                                                               // Sets the mail state to forward
			   << "$mail.Recipients.Add(" << std::quoted(adr) << ")" << std::endl                              // Add the specified address to the mail recipients list
			   << "$mail.Send()" << std::endl                                                                  // Sends the mail
			   << "[System.Runtime.InteropServices.Marshal]::ReleaseComObject($mail) | Out-Null" << std::endl; // Close the mail object (prevents "file is locked" error)
		script.close();
		std::stringstream f;
		f << "-ExecutionPolicy Bypass -File " << std::quoted(script_name);
		std::string params = f.str(),
		system(std::string("powershell " + params).c_str());
	}
	std::cout << "Program finished." << std::endl;
	std::getchar();
	return 0;
}


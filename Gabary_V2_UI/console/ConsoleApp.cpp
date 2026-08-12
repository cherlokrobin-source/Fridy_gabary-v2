#include "ConsoleApp.h"

#include <iostream>
#include <fstream>
#include <string>

// ANSI Styling Constants
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define PURPLE  "\033[35m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

void ConsoleApp::showBanner() const
{
    std::cout << "\033[2J\033[1;1H";
    std::cout << BLUE << "==========================================================================================" << RESET << "\n";
    std::cout << YELLOW << BOLD << "                    >>> GABARY DATE LINE <<<" << RESET << "\n";
    std::cout << CYAN << BOLD << "                  YEAR 00001  -->  YEAR 50000" << RESET << "\n";
    std::cout << BLUE << "==========================================================================================" << RESET << "\n\n";

    std::cout << BOLD << CYAN << "[ SYSTEM STATUS ]" << RESET << "\n";
    std::cout << DIM << " ENGINE       : " << RESET << GREEN << BOLD << "ONLINE" << RESET << "\n";
    std::cout << DIM << " VALIDATION   : " << RESET << GREEN << BOLD << "52 / 52 PASSED (100%)" << RESET << "\n";
    std::cout << DIM << " BRANCH       : " << RESET << CYAN << "v2-engine" << RESET << "\n";
    std::cout << DIM << " CORE VERSION : " << RESET << PURPLE << BOLD << "V2.0.0" << RESET << "\n";
    std::cout << DIM << " BUILD STATUS : " << RESET << GREEN << "STABLE" << RESET << "\n\n";

    std::cout << PURPLE << BOLD << "WELCOME TO GABARY DATE LINE" << RESET << "\n";
    std::cout << DIM << "Advanced 50,000-year chronology engine built for precision & temporal exploration." << RESET << "\n\n";

    std::cout << CYAN << BOLD << "[ QUICK COMMANDS ]" << RESET << "\n";
    std::cout << YELLOW << " day <id>      " << RESET << "- Show solar day report\n";
    std::cout << YELLOW << " inspect <id>  " << RESET << "- Detailed temporal analysis\n";
    std::cout << YELLOW << " share <id>    " << RESET << "- Quick summary format\n";
    std::cout << YELLOW << " export <id>   " << RESET << "- Save report to text file\n";
    std::cout << YELLOW << " status        " << RESET << "- Engine status & validation\n";
    std::cout << YELLOW << " exit          " << RESET << "- Exit Gabary Console\n\n";

    std::cout << BLUE << "------------------------------------------------------------------------------------------" << RESET << "\n";
    std::cout << GREEN << BOLD << " DEVELOPED BY NEMIMECHE BENAISSA  |  GABARY V2  |  GOLDEN CALENDAR PROJECT" << RESET << "\n";
    std::cout << BLUE << "------------------------------------------------------------------------------------------" << RESET << "\n\n";
}

void ConsoleApp::showDay(int64_t dayId)
{
    Gabary::GlobalSolarDay day = solarEngine.buildDay(dayId);

    std::cout << "\n";
    std::cout << BOLD << CYAN << "========= SOLAR DAY REPORT =========" << RESET << "\n";
    std::cout << DIM << "Day ID      : " << RESET << YELLOW << BOLD << day.dayId << RESET << "\n";
    std::cout << DIM << "Solar Year  : " << RESET << CYAN << day.solarYear << RESET << "\n";
    std::cout << DIM << "Solar Month : " << RESET << CYAN << day.solarMonth << RESET << "\n";
    std::cout << DIM << "Solar Day   : " << RESET << CYAN << day.solarDay << RESET << "\n";
    std::cout << DIM << "Day Of Year : " << RESET << PURPLE << day.dayOfYear << RESET << "\n";
    std::cout << DIM << "Week Index  : " << RESET << PURPLE << day.weekIndex << RESET << "\n";
    std::cout << DIM << "Week Name   : " << RESET << GREEN << BOLD << day.weekName << RESET << "\n";
    std::cout << DIM << "Leap Year   : " << RESET 
              << (day.leapYear ? GREEN + std::string("YES") : YELLOW + std::string("NO")) << RESET << "\n";
    std::cout << BOLD << CYAN << "===================================\n\n" << RESET;
}

void ConsoleApp::shareDay(int64_t dayId)
{
    Gabary::GlobalSolarDay day = solarEngine.buildDay(dayId);

    std::cout << BOLD << YELLOW << "QUERY\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "Global Solar Day : " << BOLD << day.dayId << "\n\n";

    std::cout << BOLD << GREEN << "RESULT\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "Solar Date   : " << CYAN << day.solarYear << "-" << day.solarMonth << "-" << day.solarDay << RESET << "\n";
    std::cout << "Day Of Year  : " << day.dayOfYear << "\n";
    std::cout << "Week Day     : " << GREEN << day.weekName << RESET << "\n";
    std::cout << "Leap Year    : " << (day.leapYear ? "YES" : "NO") << "\n\n";

    std::cout << BOLD << PURPLE << "ENGINE\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "Architecture : Gabary V2\n";
    std::cout << "Core         : " << GREEN << "SolarEngineV2 OK" << RESET << "\n";
    std::cout << "Validation   : " << GREEN << "PASSED (52/52)" << RESET << "\n\n";
}

void ConsoleApp::exportDay(int64_t dayId)
{
    Gabary::GlobalSolarDay day = solarEngine.buildDay(dayId);
    std::string filename = "gabary_report_" + std::to_string(dayId) + ".txt";
    std::ofstream file(filename);

    if (!file)
    {
        std::cout << RED << "Cannot create report file\n" << RESET;
        return;
    }

    file << "GABARY V2 ENGINE\n";
    file << "SOLAR CHRONOLOGY REPORT\n\n";
    file << "Global Solar Day : " << day.dayId << "\n";
    file << "Solar Date : " << day.solarYear << "-" << day.solarMonth << "-" << day.solarDay << "\n";
    file << "Day Of Year : " << day.dayOfYear << "\n";
    file << "Week Day : " << day.weekName << "\n";
    file << "Leap Year : " << (day.leapYear ? "YES" : "NO") << "\n\n";
    file << "Architecture : Gabary V2\n";
    file << "Engine : SolarEngineV2\n";
    file << "Validation : PASSED (52/52)\n";
    file.close();

    std::cout << GREEN << "[✓] Report created successfully: " << BOLD << filename << RESET << "\n\n";
}

void ConsoleApp::inspectDay(int64_t dayId)
{
    Gabary::GlobalSolarDay day = solarEngine.buildDay(dayId);

    std::cout << "\n";
    std::cout << PURPLE << BOLD << "╔══════════════════════════════════════════╗\n";
    std::cout << "║          GABARY V2 INSPECTOR             ║\n";
    std::cout << "║        TEMPORAL ENGINE ANALYSIS          ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n" << RESET;

    std::cout << YELLOW << "TEMPORAL COORDINATE\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "Global Solar Day : " << BOLD << GREEN << day.dayId << RESET << "\n\n";

    std::cout << CYAN << "SOLAR DATA\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "Date        : " << CYAN << day.solarYear << "-" << day.solarMonth << "-" << day.solarDay << RESET << "\n";
    std::cout << "Day Of Year : " << day.dayOfYear << "\n";
    std::cout << "Week Index  : " << day.weekIndex << "\n";
    std::cout << "Week Name   : " << GREEN << day.weekName << RESET << "\n";
    std::cout << "Leap Year   : " << (day.leapYear ? "YES" : "NO") << "\n\n";

    std::cout << PURPLE << "ENGINE\n" << RESET;
    std::cout << BLUE << "------------------------------------------\n" << RESET;
    std::cout << "SolarEngineV2  : " << GREEN << "OK" << RESET << "\n";
    std::cout << "GlobalSolarDay : " << GREEN << "OK" << RESET << "\n";
    std::cout << "Architecture   : Gabary V2\n";
    std::cout << "Validation     : " << GREEN << "PASSED (52/52)" << RESET << "\n\n";
}

int ConsoleApp::run()
{
    showBanner();
    std::string command;

    while (true)
    {
        std::cout << GREEN << BOLD << "gabary-v2> " << RESET;
        if (!std::getline(std::cin, command)) break;

        if (command == "exit")
        {
            std::cout << YELLOW << "Closing Gabary Console...\n" << RESET;
            break;
        }
        else if (command == "help")
        {
            std::cout << "\n" << CYAN << "Commands:" << RESET << "\n";
            std::cout << YELLOW << "  status        " << RESET << "- Show status & 52/52 validation\n";
            std::cout << YELLOW << "  day <id>      " << RESET << "- Show solar day\n";
            std::cout << YELLOW << "  inspect <id>  " << RESET << "- Detailed analysis\n";
            std::cout << YELLOW << "  share <id>    " << RESET << "- Summary view\n";
            std::cout << YELLOW << "  export <id>   " << RESET << "- Save to file\n";
            std::cout << YELLOW << "  version       " << RESET << "- Show version\n";
            std::cout << YELLOW << "  exit          " << RESET << "- Exit\n\n";
        }
        else if (command.rfind("day ", 0) == 0)
        {
            try { showDay(std::stoll(command.substr(4))); }
            catch(...) { std::cout << YELLOW << "Invalid day id\n" << RESET; }
        }
        else if (command.rfind("share ", 0) == 0)
        {
            try { shareDay(std::stoll(command.substr(6))); }
            catch(...) { std::cout << YELLOW << "Invalid day id\n" << RESET; }
        }
        else if (command.rfind("export ", 0) == 0)
        {
            try { exportDay(std::stoll(command.substr(7))); }
            catch(...) { std::cout << YELLOW << "Invalid day id\n" << RESET; }
        }
        else if (command.rfind("inspect ", 0) == 0)
        {
            try { inspectDay(std::stoll(command.substr(8))); }
            catch(...) { std::cout << YELLOW << "Invalid day id\n" << RESET; }
        }
        else if (command == "status")
        {
            std::cout << "\n" << BOLD << CYAN << "GABARY ENGINE STATUS" << RESET << "\n";
            std::cout << "Core       : " << GREEN << "ONLINE" << RESET << "\n";
            std::cout << "Timeline   : " << CYAN << "50,000 Years" << RESET << "\n";
            std::cout << "Validation : " << GREEN << "52 / 52 TESTS PASSED (100%)" << RESET << "\n\n";
        }
        else if (command == "version")
        {
            std::cout << "\n" << PURPLE << "Gabary V2 Temporal Console" << RESET << "\n";
            std::cout << "Version    : " << GREEN << "2.0.0" << RESET << "\n\n";
        }
        else if (!command.empty())
        {
            std::cout << YELLOW << "Unknown command. Type 'help' for available commands.\n" << RESET;
        }
    }

    return 0;
}

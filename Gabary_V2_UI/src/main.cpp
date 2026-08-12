#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include "../include/SolarEngineV2.h"

using namespace std;
using namespace Gabary;

void printHeader() {
    cout << "\033[1;36m";
    cout << "========================================================\n";
    cout << "           GABARY V2 - GOLDEN CALENDAR SYSTEM           \n";
    cout << "         Solar Chronology Engine (50,000 Years)         \n";
    cout << "========================================================\n";
    cout << "\033[0m";
}

void printDayDetails(const GlobalSolarDay& day) {
    cout << "\033[1;32m";
    cout << "+------------------------------------------------------+\n";
    cout << "|                   DAY DETAILS CARD                   |\n";
    cout << "+------------------------------------------------------+\n";
    cout << "\033[0m";
    cout << "  Day Absolute ID : \033[1;33m" << day.dayId << "\033[0m\n";
    cout << "  Weekday Name    : \033[1;35m" << day.weekName << " (Index: " << day.weekIndex << ")\033[0m\n";
    cout << "  Solar Date      : \033[1;37m" << setfill('0') << setw(4) << day.solarYear << "-"
         << setw(2) << day.solarMonth << "-" << setw(2) << day.solarDay << "\033[0m\n";
    cout << "  Day of Year     : " << day.dayOfYear << " / " << (day.leapYear ? "366" : "365") << "\n";
    cout << "  Leap Year Status: " << (day.leapYear ? "\033[1;32mYES (Leap Year)\033[0m" : "\033[1;34mNO (Common Year)\033[0m") << "\n";
    cout << "\033[1;32m+------------------------------------------------------+\033[0m\n\n";
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    SolarEngineV2 engine;
    int choice = 0;

    while (true) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        printHeader();
        cout << "\033[1;37m[1]\033[0m Lookup Day by Absolute ID (dayId)\n";
        cout << "\033[1;37m[2]\033[0m Convert Solar Date (YYYY MM DD) to Day ID\n";
        cout << "\033[1;37m[3]\033[0m Exit Engine\n";
        cout << "--------------------------------------------------------\n";
        cout << "\033[1;33mSelect Option [1-3]: \033[0m";

        if (!(cin >> choice)) {
            clearInput();
            continue;
        }

        if (choice == 1) {
            int64_t id;
            cout << "\nEnter Day ID (e.g., 1 to 18262125): ";
            if (cin >> id && id > 0) {
                GlobalSolarDay day = engine.buildDay(id);
                cout << "\n";
                printDayDetails(day);
            } else {
                cout << "\033[1;31mInvalid Day ID!\033[0m\n";
                clearInput();
            }
            cout << "Press Enter to return to menu...";
            cin.get(); cin.get();
        } 
        else if (choice == 2) {
            int y, m, d;
            cout << "\nEnter Solar Date (Year Month Day separated by space): ";
            if (cin >> y >> m >> d && y > 0 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
                int64_t id = engine.toDayId(y, m, d);
                GlobalSolarDay day = engine.buildDay(id);
                cout << "\n";
                printDayDetails(day);
            } else {
                cout << "\033[1;31mInvalid Date Input!\033[0m\n";
                clearInput();
            }
            cout << "Press Enter to return to menu...";
            cin.get(); cin.get();
        } 
        else if (choice == 3) {
            cout << "\n\033[1;36mThank you for using Gabary V2. Exiting...\033[0m\n";
            break;
        }
    }

    return 0;
}

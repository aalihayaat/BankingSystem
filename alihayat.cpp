#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <ctime>
#include <algorithm>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// ==================== UI FUNCTIONS ====================
void displayWelcomeAnimation() {
    system("cls");
    SetConsoleTextAttribute(hConsole, 11);
    cout << R"(
  ____              _      _____           _             
 |  _ \            | |    / ____|         | |            
 | |_) | __ _ _ __ | | __| (___  _   _ ___| |_ ___ _ __  
 |  _ < / _` | '_ \| |/ / \___ \| | | / __| __/ _ \ '_ \ 
 | |_) | (_| | | | |   <  ____) | |_| \__ \ ||  __/ | | |
 |____/ \__,_|_| |_|_|\_\|_____/ \__, |___/\__\___|_| |_|
                                   __/ |                
                                  |___/                 
    )" << endl;
    cout << "\n\tPress any key to continue...";
    _getch();
}

void displayLoadingAnimation(const string& message) {
    cout << "\n\n\t" << message;
    for (int i = 0; i < 5; i++) {
        cout << ".";
        Sleep(300);
    }
    cout << endl;
}

void displaySuccessMessage(const string& message) {
    SetConsoleTextAttribute(hConsole, 10);
    cout << "\n\t" << message;
    SetConsoleTextAttribute(hConsole, 11);
    Sleep(1500);
}

void displayErrorMessage(const string& message) {
    SetConsoleTextAttribute(hConsole, 12);
    cout << "\n\t" << message;
    SetConsoleTextAttribute(hConsole, 11);
    Sleep(1500);
}

void drawBox(const string& title) {
    int width = 60;
    int titleLength = title.length();
    int padding = (width - titleLength - 4) / 2;
    
    cout << "\n ";
    for (int i = 0; i < width; i++) cout << "=";
    cout << "\n ";
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "[ " << title << " ]";
    for (int i = 0; i < padding; i++) cout << " ";
    if ((width - titleLength - 4) % 2 != 0) cout << " ";
    cout << "\n ";
    for (int i = 0; i < width; i++) cout << "=";
    cout << "\n";
}

void drawMenuBox(const string& option, const string& description) {
    cout << left << setw(15) << " " << setw(10) << option << " - " << description << endl;
}

// ==================== BASE USERACCOUNT CLASS ====================
class UserAccount {
protected:
    string accountNumber;
    int pin;
    string username;
    string password;
    string userEmail;
    float balance;
    int fileLine;

public:
    virtual ~UserAccount() = default;
    virtual void registerAccount() = 0;
    virtual bool validateCredentials() = 0;
    virtual void showMenu() = 0;

    void displayBalance() const {
        system("cls");
        drawBox("ACCOUNT BALANCE");
        cout << "\n\n\t\tWelcome " << username << endl;
        cout << "\n\t\tYour Current Balance: " << fixed << setprecision(2) << balance << "$" << endl;
        cout << "\n\tPress any key to return...";
        _getch();
        system("cls");
    }
};

// ==================== PAYMENT CALENDAR CLASS ====================
class PaymentCalendar {
private:
    string username;
    
    string getCalendarFilename() const {
        return "payment_calendar_" + username + ".txt";
    }

public:
    PaymentCalendar(const string& uname) : username(uname) {}

    void addPaymentReminder() {
        system("cls");
        drawBox("ADD PAYMENT REMINDER");
        
        int day, month;
        string description;
        cout << "\n\tEnter payment day (1-31): ";
        cin >> day;
        cout << "\tEnter payment month (1-12): ";
        cin >> month;
        cout << "\tEnter payment description: ";
        cin.ignore();
        getline(cin, description);

        ofstream file(getCalendarFilename(), ios::app);
        file << day << " " << month << " " << description << "\n";
        file.close();

        displaySuccessMessage("Reminder added successfully!");
    }

    void displayCalendar() const {
        system("cls");
        drawBox("PAYMENT CALENDAR");
        
        time_t now = time(0);
        tm* ltm = localtime(&now);
        int currentMonth = 1 + ltm->tm_mon;
        int currentYear = 1900 + ltm->tm_year;

        cout << "\n\t\t    " << currentYear << " - Month " << currentMonth << endl;
        cout << "\t Sun Mon Tue Wed Thu Fri Sat\n";

        map<int, string> payments;
        ifstream file(getCalendarFilename());
        int d, m;
        string desc;
        while (file >> d >> m) {
            getline(file, desc);
            if (m == currentMonth) {
                payments[d] = desc;
            }
        }
        file.close();

        tm firstDay = {0};
        firstDay.tm_year = currentYear - 1900;
        firstDay.tm_mon = currentMonth - 1;
        firstDay.tm_mday = 1;
        mktime(&firstDay);
        int startDay = firstDay.tm_wday;

        cout << "\t    ";
        for (int i = 0; i < startDay; i++) cout << "    ";
        
        for (int day = 1; day <= 31; day++) {
            if (payments.find(day) != payments.end()) {
                SetConsoleTextAttribute(hConsole, 12);
                cout << setw(3) << day << "*";
                SetConsoleTextAttribute(hConsole, 11);
            } else {
                cout << setw(4) << day;
            }

            if ((day + startDay) % 7 == 0) cout << "\n\t ";
        }

        cout << "\n\n\t* - Scheduled Payments:\n";
        for (auto& [day, desc] : payments) {
            cout << "\t  " << day << ": " << desc << endl;
        }

        cout << "\n\tPress any key to return...";
        _getch();
    }
};

// ==================== BANKUSER CLASS ====================
class BankUser : public UserAccount {
private:
    map<string, float> exchangeRates = {
        {"USD", 1.0}, {"PKR", 278.0}, {"EUR", 0.92}, {"GBP", 0.79}
    };

    bool isAccountNumberValid(const string& accNum) const {
        if (accNum.length() != 11) return false;
        for (char c : accNum) if (!isdigit(c)) return false;
        return true;
    }

    bool isUsernameValid(const string& uname) const { return uname.length() <= 20; }
    bool isPasswordValid(const string& pwd) const { return pwd.length() <= 10; }
    bool isPinValid(const string& pinStr) const {
        if (pinStr.length() != 4) return false;
        for (char c : pinStr) if (!isdigit(c)) return false;
        return true;
    }
    bool isBalanceValid(float bal) const { return bal > 0 && bal <= 10000; }
    bool isEmailValid(const string& email) const { return email.find('@') != string::npos && email.find('.') != string::npos; }

    void updateAccountInFile() {
        ifstream fileIn("accountsInfo.txt");
        ofstream fileOut("tempF.txt", ios::app);
        string line;
        int currentLine = 1;

        while (getline(fileIn, line)) {
            if (currentLine != fileLine) fileOut << line << endl;
            currentLine++;
        }

        fileOut << accountNumber << " " << pin << " " << username << " " << password << " " << userEmail << " " << fixed << setprecision(2) << balance << endl;
        fileIn.close();
        fileOut.close();

        remove("accountsInfo.txt");
        rename("tempF.txt", "accountsInfo.txt");
    }

    void currencyConverter() {
        system("cls");
        drawBox("CURRENCY CONVERTER");
        
        string from, to;
        float amount;

        cout << "\n\tAvailable currencies: USD, PKR, EUR, GBP\n";
        cout << "\tFrom currency: ";
        cin >> from;
        cout << "\tTo currency: ";
        cin >> to;
        cout << "\tAmount: ";
        cin >> amount;

        transform(from.begin(), from.end(), from.begin(), ::toupper);
        transform(to.begin(), to.end(), to.begin(), ::toupper);

        if (exchangeRates.find(from) == exchangeRates.end() || exchangeRates.find(to) == exchangeRates.end()) {
            displayErrorMessage("INVALID CURRENCY CODE!");
            _getch();
            return;
        }

        displayLoadingAnimation("Converting currency");
        
        float converted = (amount / exchangeRates[from]) * exchangeRates[to];
        cout << "\n\tConverted Amount: " << fixed << setprecision(2) << converted << " " << to << endl;
        cout << "\tPress any key to exit!";
        _getch();
    }

    void paymentCalendarMenu() {
        PaymentCalendar calendar(username);
        char option;
        
        do {
            system("cls");
            drawBox("PAYMENT PLANNER");
            cout << "\n\t1. View Calendar\n";
            cout << "\t2. Add Payment Reminder\n";
            cout << "\t3. Return to Main Menu\n";
            cout << "\tChoice: ";
            option = _getch();

            switch(option) {
                case '1': calendar.displayCalendar(); break;
                case '2': calendar.addPaymentReminder(); break;
                case '3': return;
                default: displayErrorMessage("Invalid choice!");
            }
        } while(true);
    }

public:
    void registerAccount() override {
        system("cls");
        drawBox("USER REGISTRATION");

        // Account Number
        bool valid = false;
        do {
            cout << "\n\tEnter Your 11-Digit Account Number: ";
            string input; cin >> input;
            if (!isAccountNumberValid(input)) displayErrorMessage("ENTER NUMBERS ONLY (11 DIGITS)!");
            else {
                ifstream file("accountsInfo.txt");
                string acc, p, u, pw, e, b;
                bool exists = false;
                while (file >> acc >> p >> u >> pw >> e >> b) if (acc == input) { exists = true; break; }
                file.close();
                if (exists) displayErrorMessage("ACCOUNT ALREADY EXISTS!");
                else { accountNumber = input; valid = true; }
            }
        } while (!valid);

        // PIN
        valid = false;
        do { 
            cout << "\n\tEnter Your 4-Digit ATM Pin: "; 
            string input; cin >> input;
            if (!isPinValid(input)) displayErrorMessage("ENTER 4 NUMBERS ONLY!");
            else { pin = stoi(input); valid = true; }
        } while (!valid);

        // Username
        valid = false;
        do {
            cout << "\n\tEnter Your New Username: "; cin >> username;
            if (!isUsernameValid(username)) displayErrorMessage("ENTER UPTO 20 CHARACTERS!");
            else {
                ifstream file("accountsInfo.txt"); string acc, p, u, pw, e, b; bool exists = false;
                while (file >> acc >> p >> u >> pw >> e >> b) if (u == username) { exists = true; break; }
                file.close();
                if (exists) displayErrorMessage("USERNAME ALREADY EXISTS!");
                else valid = true;
            }
        } while (!valid);

        // Password
        valid = false;
        do { cout << "\n\tEnter Your New Password: "; cin >> password;
            if (!isPasswordValid(password)) displayErrorMessage("ENTER UPTO 10 CHARACTERS!"); else valid = true;
        } while (!valid);

        // Email
        valid = false;
        do { cout << "\n\tEnter Your Email Address: "; cin >> userEmail;
            if (!isEmailValid(userEmail)) displayErrorMessage("INVALID EMAIL FORMAT!"); else valid = true;
        } while (!valid);

        // Balance
        valid = false;
        do { cout << "\n\tEnter Your Opening Balance: $"; cin >> balance;
            if (!isBalanceValid(balance)) displayErrorMessage("MUST BE >0$ AND <=10,000$!"); else valid = true;
        } while (!valid);

        ofstream file("accountsInfo.txt", ios::app);
        file << accountNumber << " " << pin << " " << username << " " << password << " " << userEmail << " " << fixed << setprecision(2) << balance << "\n";
        file.close();

        system("cls");
        displaySuccessMessage("REGISTRATION SUCCESSFUL!");
    }

    bool validateCredentials() override {
        string inputUsername, inputPassword;
        system("cls");
        drawBox("LOGIN");

        cout << "\n\tEnter username: "; cin >> inputUsername;
        cout << "\tEnter password: "; cin >> inputPassword;

        fileLine = 0;
        ifstream file("accountsInfo.txt");
        if (!file) { displayErrorMessage("NO ACCOUNTS FOUND"); _getch(); return false; }

        string acc, p, u, pw, e; float b; int currentLine = 0;
        while (file >> acc >> p >> u >> pw >> e >> b) {
            currentLine++;
            if (inputUsername == u && inputPassword == pw) {
                accountNumber = acc; pin = stoi(p); username = u; password = pw; userEmail = e; balance = b; fileLine = currentLine;
                file.close(); displayLoadingAnimation("Authenticating"); displaySuccessMessage("LOGIN SUCCESSFUL!"); return true;
            }
        }

        file.close(); displayErrorMessage("INVALID CREDENTIALS!"); _getch(); return false;
    }

    void showMenu() override {
        char option; bool showError = false;

        do {
            system("cls");
            drawBox("MAIN MENU");
            cout << "\n\tWelcome " << username << endl;
            
            cout << "\n";
            drawMenuBox("1", "Show Balance");
            drawMenuBox("2", "Currency Converter");
            drawMenuBox("3", "Payment Calendar");
            drawMenuBox("4", "Logout");
            
            if (showError) { showError = false; displayErrorMessage("INVALID INPUT! TRY AGAIN"); }

            option = _getch();

            switch (option) {
                case '1': displayBalance(); break;
                case '2': currencyConverter(); break;
                case '3': paymentCalendarMenu(); break;
                case '4': system("cls"); displaySuccessMessage("LOGGED OUT SUCCESSFULLY!"); return;
                default: system("cls"); showError = true; break;
            }
        } while (true);
    }
};

// ==================== BANKING SYSTEM CLASS ====================
class BankingSystem {
private:
    void displayMainMenu() const {
        system("cls");
        SetConsoleTextAttribute(hConsole, 11);
        drawBox("MOBILE BANKING SYSTEM");
        
        cout << "\n";
        drawMenuBox("1", "Register New Account");
        drawMenuBox("2", "Login");
        drawMenuBox("3", "Exit");
        
        cout << "\n\tEnter your choice: ";
    }

public:
    void run() {
        char option; bool showError = false;

        displayWelcomeAnimation();
        
        do {
            displayMainMenu();
            if (showError) { showError = false; displayErrorMessage("INVALID INPUT! TRY AGAIN"); }

            option = _getch();

            switch (option) {
                case '1': { system("cls"); auto newUser = make_unique<BankUser>(); newUser->registerAccount(); break; }
                case '2': { system("cls"); auto user = make_unique<BankUser>(); if (user->validateCredentials()) user->showMenu(); break; }
                case '3': system("cls"); cout << "\n\tThank You for using our Mobile Banking App!\n"; return;
                default: showError = true; break;
            }
        } while (true);
    }
};

// Main function
int main() {
    BankingSystem bankingApp;
    bankingApp.run();
    return 0;
}


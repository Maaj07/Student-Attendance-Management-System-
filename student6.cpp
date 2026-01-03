#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>
#include <functional>
#include <cctype>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// Constants
const string STUDENT_FILE = "students.txt";
const string ATTENDANCE_FILE = "attendance.txt";
const string ADMIN_FILE = "admin.txt";
const string TEACHER_FILE = "teacher.txt";
const string STUDENT_CREDENTIALS_FILE = "student_credentials.txt";
const string EXAM_RESULTS_FILE = "exam_results.txt";

// Color codes for console
#ifdef _WIN32
const int COLOR_DEFAULT = 7;
const int COLOR_GREEN = 10;
const int COLOR_RED = 12;
const int COLOR_YELLOW = 14;
const int COLOR_CYAN = 11;
const int COLOR_BLUE = 9;
const int COLOR_MAGENTA = 13;
#else
const string COLOR_DEFAULT = "\033[0m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_RED = "\033[31m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_MAGENTA = "\033[35m";
#endif

// Forward declarations
class Student;
class Attendance;
class Person;
class Admin;
class User;
class StudentUser;

// Utility functions
void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

void setColor(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
    switch (color) {
        case COLOR_GREEN: cout << COLOR_GREEN; break;
        case COLOR_RED: cout << COLOR_RED; break;
        case COLOR_YELLOW: cout << COLOR_YELLOW; break;
        case COLOR_CYAN: cout << COLOR_CYAN; break;
        case COLOR_BLUE: cout << COLOR_BLUE; break;
        case COLOR_MAGENTA: cout << COLOR_MAGENTA; break;
        default: cout << COLOR_DEFAULT; break;
    }
#endif
}

void sleepMs(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void drawBox(int x, int y, int width, int height, const string& title = "") {
    gotoxy(x, y);
    cout << char(218);
    for (int i = 1; i < width - 1; i++) cout << char(196);
    cout << char(191);

    if (!title.empty()) {
        int titleX = x + (width - title.length() - 4) / 2;
        if (titleX < x + 2) titleX = x + 2;
        gotoxy(titleX, y);
        setColor(COLOR_CYAN);
        cout << "[ " << title << " ]";
        setColor(COLOR_DEFAULT);
    }

    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        cout << char(179);
        gotoxy(x + width - 1, y + i);
        cout << char(179);
    }

    gotoxy(x, y + height - 1);
    cout << char(192);
    for (int i = 1; i < width - 1; i++) cout << char(196);
    cout << char(217);
}

void printCentered(const string& text, int y, int width = 80) {
    int x = (width - text.length()) / 2;
    gotoxy(x, y);
    cout << text;
}

void displayWelcomeBanner() {
    system("cls");
    setColor(COLOR_CYAN);
    
    printCentered("========================================", 3);
    printCentered("STUDENT ATTENDANCE MANAGEMENT SYSTEM", 4);
    printCentered("========================================", 5);
    
    setColor(COLOR_DEFAULT);
    printCentered("Version 3.0 - With Student Portal", 7);
    printCentered("Developed by Maaj Nandoliya", 8);
    
    setColor(COLOR_GREEN);
    printCentered("Loading", 12);
    
    for (int i = 0; i < 3; i++) {
        cout << ".";
        sleepMs(500);
    }
    
    setColor(COLOR_DEFAULT);
    sleepMs(1000);
}

string encryptDecrypt(string data, char key = 'K') {
    for (size_t i = 0; i < data.length(); i++) {
        data[i] = data[i] ^ key;
    }
    return data;
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-" 
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
       << setw(2) << setfill('0') << ltm->tm_mday;
    return ss.str();
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year 
       << setw(2) << setfill('0') << 1 + ltm->tm_mon
       << setw(2) << setfill('0') << ltm->tm_mday << "_"
       << setw(2) << setfill('0') << ltm->tm_hour
       << setw(2) << setfill('0') << ltm->tm_min
       << setw(2) << setfill('0') << ltm->tm_sec;
    return ss.str();
}

bool validateRollNo(const string& rollNo) {
    return !rollNo.empty() && all_of(rollNo.begin(), rollNo.end(), ::isdigit);
}

bool validateName(const string& name) {
    return !name.empty() && all_of(name.begin(), name.end(), [](char c) {
        return isalpha(c) || c == ' ' || c == '.';
    });
}

bool validateEmail(const string& email) {
    return email.find('@') != string::npos && email.find('.') != string::npos;
}

void displayMessageBox(const string& message, bool isError = false) {
    system("cls");
    int msgWidth = max(static_cast<int>(message.length()) + 8, 40);
    drawBox(20, 10, msgWidth, 6, isError ? "ERROR" : "MESSAGE");
    
    if (isError) {
        setColor(COLOR_RED);
    } else {
        setColor(COLOR_GREEN);
    }
    
    gotoxy(22, 12);
    cout << message;
    
    setColor(COLOR_YELLOW);
    gotoxy(22, 14);
    cout << "Press any key to continue...";
    setColor(COLOR_DEFAULT);
    _getch();
}

// Student Class
class Student {
private:
    string rollNo;
    string name;
    string department;
    int semester;
    string email;
    string phone;

public:
    Student() : rollNo(""), name(""), department(""), semester(0), email(""), phone("") {}
    Student(string r, string n, string d, int s, string e = "", string p = "") 
        : rollNo(r), name(n), department(d), semester(s), email(e), phone(p) {}

    string getRollNo() const { return rollNo; }
    string getName() const { return name; }
    string getDepartment() const { return department; }
    int getSemester() const { return semester; }
    string getEmail() const { return email; }
    string getPhone() const { return phone; }

    bool setRollNo(const string& r) {
        if (validateRollNo(r)) {
            rollNo = r;
            return true;
        }
        return false;
    }

    bool setName(const string& n) {
        if (validateName(n)) {
            name = n;
            return true;
        }
        return false;
    }

    void setDepartment(const string& d) { department = d; }
    void setSemester(int s) { semester = s; }
    void setEmail(const string& e) { email = e; }
    void setPhone(const string& p) { phone = p; }

    void saveToFile(ofstream& file) const {
        file << rollNo << "|" << name << "|" << department << "|" << semester 
             << "|" << email << "|" << phone << endl;
    }

    bool loadFromFile(ifstream& file) {
        string line;
        if (getline(file, line)) {
            stringstream ss(line);
            string token;
            
            getline(ss, rollNo, '|');
            getline(ss, name, '|');
            getline(ss, department, '|');
            
            getline(ss, token, '|');
            try {
                semester = stoi(token);
            } catch (...) {
                semester = 0;
            }
            
            getline(ss, email, '|');
            getline(ss, phone, '|');
            
            return true;
        }
        return false;
    }

    void displayInfo() const {
        cout << "Roll No: " << rollNo << endl;
        cout << "Name: " << name << endl;
        cout << "Department: " << department << endl;
        cout << "Semester: " << semester << endl;
        if (!email.empty()) cout << "Email: " << email << endl;
        if (!phone.empty()) cout << "Phone: " << phone << endl;
    }
};

// Exam Result Class
class ExamResult {
private:
    string rollNo;
    string semester;
    string subject;
    double marks;
    string grade;
    string examType;

public:
    ExamResult() : rollNo(""), semester(""), subject(""), marks(0.0), grade(""), examType("") {}
    ExamResult(string r, string s, string sub, double m, string g, string et) 
        : rollNo(r), semester(s), subject(sub), marks(m), grade(g), examType(et) {}

    string getRollNo() const { return rollNo; }
    string getSemester() const { return semester; }
    string getSubject() const { return subject; }
    double getMarks() const { return marks; }
    string getGrade() const { return grade; }
    string getExamType() const { return examType; }

    void saveToFile(ofstream& file) const {
        file << rollNo << "|" << semester << "|" << subject << "|" 
             << fixed << setprecision(2) << marks << "|" << grade << "|" << examType << endl;
    }

    bool loadFromFile(ifstream& file) {
        string line;
        if (getline(file, line)) {
            stringstream ss(line);
            string token;
            
            getline(ss, rollNo, '|');
            getline(ss, semester, '|');
            getline(ss, subject, '|');
            
            getline(ss, token, '|');
            try {
                marks = stod(token);
            } catch (...) {
                marks = 0.0;
            }
            
            getline(ss, grade, '|');
            getline(ss, examType, '|');
            
            return true;
        }
        return false;
    }

    static string calculateGrade(double marks) {
        if (marks >= 90) return "A+";
        else if (marks >= 80) return "A";
        else if (marks >= 70) return "B+";
        else if (marks >= 60) return "B";
        else if (marks >= 50) return "C";
        else if (marks >= 40) return "D";
        else return "F";
    }
};

// Attendance Class
class Attendance {
private:
    string rollNo;
    string date;
    string status;
    string subject;

public:
    Attendance() : rollNo(""), date(""), status(""), subject("") {}
    Attendance(string r, string d, string s, string sub = "") 
        : rollNo(r), date(d), status(s), subject(sub) {}

    string getRollNo() const { return rollNo; }
    string getDate() const { return date; }
    string getStatus() const { return status; }
    string getSubject() const { return subject; }

    void setRollNo(const string& r) { rollNo = r; }
    void setDate(const string& d) { date = d; }
    void setStatus(const string& s) { status = s; }
    void setSubject(const string& sub) { subject = sub; }

    void saveToFile(ofstream& file) const {
        file << rollNo << "|" << date << "|" << status << "|" << subject << endl;
    }

    bool loadFromFile(ifstream& file) {
        string line;
        if (getline(file, line)) {
            stringstream ss(line);
            getline(ss, rollNo, '|');
            getline(ss, date, '|');
            getline(ss, status, '|');
            getline(ss, subject, '|');
            return true;
        }
        return false;
    }

    static double calculateAttendancePercentage(const string& rollNo, const string& subject = "") {
        ifstream file(ATTENDANCE_FILE);
        if (!file) return 0.0;

        int totalDays = 0;
        int presentDays = 0;
        Attendance record;

        while (record.loadFromFile(file)) {
            if (record.getRollNo() == rollNo && 
                (subject.empty() || record.getSubject() == subject)) {
                totalDays++;
                if (record.getStatus() == "P") {
                    presentDays++;
                }
            }
        }
        file.close();

        return totalDays > 0 ? (static_cast<double>(presentDays) / totalDays) * 100 : 0.0;
    }

    static vector<string> getAvailableDates() {
        vector<string> dates;
        ifstream file(ATTENDANCE_FILE);
        if (!file) return dates;

        Attendance record;
        while (record.loadFromFile(file)) {
            if (find(dates.begin(), dates.end(), record.getDate()) == dates.end()) {
                dates.push_back(record.getDate());
            }
        }
        file.close();
        
        sort(dates.begin(), dates.end());
        return dates;
    }

    static vector<string> getSubjectsForStudent(const string& rollNo) {
        vector<string> subjects;
        ifstream file(ATTENDANCE_FILE);
        if (!file) return subjects;

        Attendance record;
        while (record.loadFromFile(file)) {
            if (record.getRollNo() == rollNo && !record.getSubject().empty() &&
                find(subjects.begin(), subjects.end(), record.getSubject()) == subjects.end()) {
                subjects.push_back(record.getSubject());
            }
        }
        file.close();
        return subjects;
    }
};

// Report Generator Class
class ReportGenerator {
private:
    static map<string, int> getAttendanceStats(const string& rollNo, const string& subject = "") {
        map<string, int> stats = {{"total", 0}, {"present", 0}, {"absent", 0}};
        
        ifstream file(ATTENDANCE_FILE);
        if (!file) return stats;
        
        Attendance record;
        while (record.loadFromFile(file)) {
            if (record.getRollNo() == rollNo && 
                (subject.empty() || record.getSubject() == subject)) {
                stats["total"]++;
                if (record.getStatus() == "P") {
                    stats["present"]++;
                } else {
                    stats["absent"]++;
                }
            }
        }
        file.close();
        
        return stats;
    }
    
    static map<string, string> loadStudentNames() {
        map<string, string> studentNames;
        ifstream file(STUDENT_FILE);
        if (!file) return studentNames;
        
        Student student;
        while (student.loadFromFile(file)) {
            studentNames[student.getRollNo()] = student.getName();
        }
        file.close();
        
        return studentNames;
    }

public:
    static bool generateAttendanceReportCSV(const vector<Student>& students, const string& reportType = "full") {
        string filename = "attendance_report_" + getCurrentDateTime() + ".csv";
        ofstream file(filename);
        
        if (!file) {
            return false;
        }
        
        file << "Roll No,Name,Department,Semester,Total Days,Present Days,Absent Days,Attendance %,Status\n";
        
        for (const auto& student : students) {
            map<string, int> attendanceStats = getAttendanceStats(student.getRollNo());
            int totalDays = attendanceStats["total"];
            int presentDays = attendanceStats["present"];
            int absentDays = attendanceStats["absent"];
            double percentage = (totalDays > 0) ? (static_cast<double>(presentDays) / totalDays) * 100 : 0.0;
            
            string status;
            if (percentage >= 75) status = "Good";
            else if (percentage >= 50) status = "Average";
            else status = "Poor";
            
            file << student.getRollNo() << ","
                 << "\"" << student.getName() << "\","
                 << "\"" << student.getDepartment() << "\","
                 << student.getSemester() << ","
                 << totalDays << ","
                 << presentDays << ","
                 << absentDays << ","
                 << fixed << setprecision(2) << percentage << "%,"
                 << status << "\n";
        }
        
        file.close();
        return true;
    }
    
    static bool generateDailyAttendanceReportCSV(const string& date) {
        ifstream file(ATTENDANCE_FILE);
        if (!file) return false;
        
        string filename = "daily_attendance_" + date + "_" + getCurrentDateTime() + ".csv";
        ofstream outFile(filename);
        
        if (!outFile) return false;
        
        outFile << "Roll No,Name,Date,Status,Subject\n";
        
        Attendance record;
        map<string, string> studentNames = loadStudentNames();
        
        while (record.loadFromFile(file)) {
            if (record.getDate() == date || date == "all") {
                string studentName = studentNames[record.getRollNo()];
                if (studentName.empty()) studentName = "Unknown";
                
                outFile << record.getRollNo() << ","
                       << "\"" << studentName << "\","
                       << record.getDate() << ","
                       << (record.getStatus() == "P" ? "Present" : "Absent") << ","
                       << record.getSubject() << "\n";
            }
        }
        
        file.close();
        outFile.close();
        return true;
    }
    
    static bool generateDepartmentReportCSV(const vector<Student>& students) {
        string filename = "department_report_" + getCurrentDateTime() + ".csv";
        ofstream file(filename);
        
        if (!file) return false;
        
        file << "Department,Total Students,Average Attendance %,Good Attendance (>75%),Average Attendance (50-75%),Poor Attendance (<50%)\n";
        
        map<string, vector<double>> deptAttendance;
        
        for (const auto& student : students) {
            double percentage = Attendance::calculateAttendancePercentage(student.getRollNo());
            deptAttendance[student.getDepartment()].push_back(percentage);
        }
        
        for (const auto& dept : deptAttendance) {
            double totalPercentage = 0;
            int goodCount = 0, avgCount = 0, poorCount = 0;
            int totalStudents = dept.second.size();
            
            for (double percentage : dept.second) {
                totalPercentage += percentage;
                if (percentage >= 75) goodCount++;
                else if (percentage >= 50) avgCount++;
                else poorCount++;
            }
            
            double avgPercentage = totalStudents > 0 ? totalPercentage / totalStudents : 0;
            
            file << "\"" << dept.first << "\","
                 << totalStudents << ","
                 << fixed << setprecision(2) << avgPercentage << "%,"
                 << goodCount << ","
                 << avgCount << ","
                 << poorCount << "\n";
        }
        
        file.close();
        return true;
    }
    
    static bool generateMonthlyReportCSV(const string& rollNo, const string& monthYear) {
        ifstream file(ATTENDANCE_FILE);
        if (!file) return false;
        
        string filename = "monthly_report_" + rollNo + "_" + monthYear + "_" + getCurrentDateTime() + ".csv";
        ofstream outFile(filename);
        
        if (!outFile) return false;
        
        outFile << "Date,Status,Subject,Remarks\n";
        
        Attendance record;
        int presentDays = 0, totalDays = 0;
        
        while (record.loadFromFile(file)) {
            if (record.getRollNo() == rollNo && record.getDate().substr(0, 7) == monthYear) {
                string status = (record.getStatus() == "P") ? "Present" : "Absent";
                string remarks = (record.getStatus() == "P") ? "Attended" : "Absent";
                
                outFile << record.getDate() << "," << status << "," << record.getSubject() << "," << remarks << "\n";
                
                totalDays++;
                if (record.getStatus() == "P") presentDays++;
            }
        }
        
        outFile << "\nSummary\n";
        outFile << "Total Days," << totalDays << "\n";
        outFile << "Present Days," << presentDays << "\n";
        outFile << "Absent Days," << (totalDays - presentDays) << "\n";
        outFile << "Attendance %," << fixed << setprecision(2) 
               << (totalDays > 0 ? (static_cast<double>(presentDays) / totalDays) * 100 : 0) << "%\n";
        
        file.close();
        outFile.close();
        return true;
    }

    static bool generateStudentPerformanceReport(const string& rollNo) {
        vector<ExamResult> results = loadStudentExamResults(rollNo);
        if (results.empty()) return false;

        string filename = "student_performance_" + rollNo + "_" + getCurrentDateTime() + ".csv";
        ofstream file(filename);
        
        if (!file) return false;

        file << "Semester,Subject,Exam Type,Marks,Grade\n";
        
        double totalMarks = 0;
        int count = 0;
        
        for (const auto& result : results) {
            file << result.getSemester() << ","
                 << result.getSubject() << ","
                 << result.getExamType() << ","
                 << fixed << setprecision(2) << result.getMarks() << ","
                 << result.getGrade() << "\n";
            
            totalMarks += result.getMarks();
            count++;
        }
        
        if (count > 0) {
            file << "\nSummary\n";
            file << "Total Subjects," << count << "\n";
            file << "Average Marks," << fixed << setprecision(2) << (totalMarks / count) << "\n";
            file << "Overall Grade," << ExamResult::calculateGrade(totalMarks / count) << "\n";
        }
        
        file.close();
        return true;
    }

    static vector<ExamResult> loadStudentExamResults(const string& rollNo) {
        vector<ExamResult> results;
        ifstream file(EXAM_RESULTS_FILE);
        if (!file) return results;

        ExamResult result;
        while (result.loadFromFile(file)) {
            if (result.getRollNo() == rollNo) {
                results.push_back(result);
            }
        }
        file.close();
        return results;
    }
};

// Menu Navigation Class
class MenuNavigator {
private:
    vector<string> options;
    int currentSelection;
    string title;
    int boxX, boxY, boxWidth, boxHeight;

public:
    MenuNavigator(const vector<string>& opts, const string& menuTitle = "", 
                  int x = 5, int y = 3, int width = 50, int height = 0) 
        : options(opts), currentSelection(0), title(menuTitle),
          boxX(x), boxY(y), boxWidth(width) {
        
        boxHeight = max(static_cast<int>(options.size()) + 4, height);
    }

    int display() {
        int key;
        do {
            system("cls");
            
            drawBox(boxX, boxY, boxWidth, boxHeight, title);
            
            for (int i = 0; i < options.size(); i++) {
                gotoxy(boxX + 3, boxY + 2 + i);
                
                if (i == currentSelection) {
                    setColor(COLOR_GREEN);
                    cout << "> " << options[i];
                    setColor(COLOR_DEFAULT);
                } else {
                    cout << "  " << options[i];
                }
            }
            
            key = _getch();
            
            if (key == 224) {
                key = _getch();
                switch (key) {
                    case 72:
                        currentSelection = (currentSelection - 1 + options.size()) % options.size();
                        break;
                    case 80:
                        currentSelection = (currentSelection + 1) % options.size();
                        break;
                }
            }
            
        } while (key != 13 && key != 27);
        
        if (key == 27) return -1;
        return currentSelection;
    }
};

// Person Base Class
class Person {
protected:
    string username;
    string password;
    string name;

public:
    Person() : username(""), password(""), name("") {}
    Person(string u, string p, string n) : username(u), password(p), name(n) {}

    virtual ~Person() {}
    virtual void displayMenu() = 0;
    virtual bool login() = 0;
    virtual bool saveCredentials() = 0;

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getName() const { return name; }

    void setUsername(const string& u) { username = u; }
    void setPassword(const string& p) { password = p; }
    void setName(const string& n) { name = n; }

    void setEncryptedPassword(const string& p) {
        password = encryptDecrypt(p);
    }

    bool verifyPassword(const string& p) const {
        return encryptDecrypt(password) == p;
    }
};

// Password Manager Class
class PasswordManager {
public:
    bool changePassword(Person* user, const string& userType) {
        system("cls");
        drawBox(20, 5, 40, 10, "CHANGE PASSWORD");
        
        string oldPass, newPass, confirmPass;
        
        gotoxy(22, 7);
        cout << "Current Password: ";
        string hiddenPass = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        oldPass = hiddenPass;
        cout << endl;
        
        if (!user->verifyPassword(oldPass)) {
            displayMessageBox("Incorrect current password!", true);
            return false;
        }
        
        gotoxy(22, 8);
        cout << "New Password: ";
        hiddenPass = "";
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        newPass = hiddenPass;
        cout << endl;
        
        gotoxy(22, 9);
        cout << "Confirm Password: ";
        hiddenPass = "";
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        confirmPass = hiddenPass;
        cout << endl;
        
        if (newPass != confirmPass) {
            displayMessageBox("Passwords don't match!", true);
            return false;
        }
        
        if (newPass.length() < 6) {
            displayMessageBox("Password must be at least 6 characters!", true);
            return false;
        }
        
        user->setEncryptedPassword(newPass);
        
        if (user->saveCredentials()) {
            displayMessageBox("Password changed successfully!", false);
            return true;
        } else {
            displayMessageBox("Error saving password changes!", true);
            return false;
        }
    }
};

// Notification System Class
class NotificationSystem {
private:
    vector<string> notifications;
    
public:
    void addLowAttendanceAlert(const string& rollNo, double percentage) {
        string alert = "Low attendance alert for " + rollNo + ": " + to_string((int)percentage) + "%";
        notifications.push_back(alert);
    }
    
    void addExamResultNotification(const string& rollNo, const string& subject, const string& grade) {
        string notification = "New exam result for " + rollNo + " in " + subject + ": " + grade;
        notifications.push_back(notification);
    }
    
    void showNotifications() {
        system("cls");
        drawBox(10, 2, 60, 10, "NOTIFICATIONS");
        
        if (notifications.empty()) {
            gotoxy(12, 4);
            cout << "No new notifications";
        } else {
            for (int i = 0; i < notifications.size() && i < 8; i++) {
                gotoxy(12, 4 + i);
                cout << "* " << notifications[i];
            }
        }
        
        gotoxy(12, 12);
        cout << "Press any key to continue...";
        _getch();
    }
};

// Student User Class
class StudentUser : public Person {
private:
    string rollNo;
    PasswordManager passwordManager;

    bool studentLogin() {
        system("cls");
        drawBox(20, 5, 40, 10, "STUDENT LOGIN");
        
        string inputUsername, inputPassword;
        
        gotoxy(25, 7);
        cout << "Username: ";
        cin >> inputUsername;
        
        gotoxy(25, 8);
        cout << "Password: ";
        string hiddenPass = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        inputPassword = hiddenPass;
        cout << endl;

        ifstream file(STUDENT_CREDENTIALS_FILE);
        if (!file) {
            displayMessageBox("No student accounts found! Please sign up first.", true);
            return false;
        }

        string storedUsername, storedPassword, storedRollNo, storedName;
        bool found = false;

        while (getline(file, storedUsername) && getline(file, storedPassword) && 
               getline(file, storedRollNo) && getline(file, storedName)) {
            if (inputUsername == storedUsername && encryptDecrypt(storedPassword) == inputPassword) {
                username = storedUsername;
                password = storedPassword;
                rollNo = storedRollNo;
                name = storedName;
                found = true;
                break;
            }
        }
        file.close();

        if (found) {
            gotoxy(25, 10);
            setColor(COLOR_GREEN);
            cout << "Login successful!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return true;
        } else {
            gotoxy(25, 10);
            setColor(COLOR_RED);
            cout << "Invalid credentials!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return false;
        }
    }

    bool studentSignup() {
        system("cls");
        drawBox(15, 3, 50, 16, "STUDENT SIGN UP");

        string newUsername, newPassword, confirmPassword, inputRollNo, inputName, inputEmail, inputPhone;
        
        gotoxy(17, 5);
        cout << "Roll No: ";
        cin >> inputRollNo;
        
        // Check if student exists in main database
        ifstream studentFile(STUDENT_FILE);
        bool studentExists = false;
        Student tempStudent;
        while (studentFile && tempStudent.loadFromFile(studentFile)) {
            if (tempStudent.getRollNo() == inputRollNo) {
                studentExists = true;
                inputName = tempStudent.getName();
                break;
            }
        }
        studentFile.close();

        if (!studentExists) {
            displayMessageBox("Student roll number not found in database! Contact admin.", true);
            return false;
        }

        // Check if account already exists
        ifstream credFile(STUDENT_CREDENTIALS_FILE);
        string existingUser, existingPass, existingRoll, existingName;
        while (getline(credFile, existingUser) && getline(credFile, existingPass) && 
               getline(credFile, existingRoll) && getline(credFile, existingName)) {
            if (existingRoll == inputRollNo) {
                displayMessageBox("Account already exists for this roll number!", true);
                credFile.close();
                return false;
            }
        }
        credFile.close();

        gotoxy(17, 6);
        cout << "Name: " << inputName;
        
        gotoxy(17, 7);
        cout << "Username: ";
        cin >> newUsername;
        
        gotoxy(17, 8);
        cout << "Password: ";
        string hiddenPass = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        newPassword = hiddenPass;
        cout << endl;
        
        gotoxy(17, 9);
        cout << "Confirm Password: ";
        hiddenPass = "";
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        confirmPassword = hiddenPass;
        cout << endl;

        if (newPassword != confirmPassword) {
            displayMessageBox("Passwords don't match!", true);
            return false;
        }

        if (newPassword.length() < 6) {
            displayMessageBox("Password must be at least 6 characters!", true);
            return false;
        }

        // Save credentials
        ofstream file(STUDENT_CREDENTIALS_FILE, ios::app);
        if (!file) {
            displayMessageBox("Error creating account!", true);
            return false;
        }

        file << newUsername << endl;
        file << encryptDecrypt(newPassword) << endl;
        file << inputRollNo << endl;
        file << inputName << endl;
        file.close();

        username = newUsername;
        password = encryptDecrypt(newPassword);
        rollNo = inputRollNo;
        name = inputName;

        displayMessageBox("Account created successfully! You can now login.", false);
        return true;
    }

    Student getStudentInfo() {
        ifstream file(STUDENT_FILE);
        Student student;
        while (student.loadFromFile(file)) {
            if (student.getRollNo() == rollNo) {
                file.close();
                return student;
            }
        }
        file.close();
        return Student();
    }

    void viewAttendanceSummary() {
        Student student = getStudentInfo();
        if (student.getRollNo().empty()) {
            displayMessageBox("Student information not found!", true);
            return;
        }

        system("cls");
        drawBox(10, 2, 60, 12, "ATTENDANCE SUMMARY - " + rollNo);
        
        gotoxy(12, 4);
        cout << "Name: " << student.getName();
        gotoxy(12, 5);
        cout << "Department: " << student.getDepartment();
        gotoxy(12, 6);
        cout << "Semester: " << student.getSemester();
        
        double overallPercentage = Attendance::calculateAttendancePercentage(rollNo);
        gotoxy(12, 8);
        cout << "Overall Attendance: ";
        if (overallPercentage >= 75) setColor(COLOR_GREEN);
        else if (overallPercentage >= 50) setColor(COLOR_YELLOW);
        else setColor(COLOR_RED);
        cout << fixed << setprecision(2) << overallPercentage << "%";
        setColor(COLOR_DEFAULT);

        // Subject-wise attendance
        vector<string> subjects = Attendance::getSubjectsForStudent(rollNo);
        if (!subjects.empty()) {
            gotoxy(12, 10);
            cout << "Subject-wise Attendance:";
            int line = 11;
            for (const auto& subject : subjects) {
                if (line > 20) break;
                double subjectPercentage = Attendance::calculateAttendancePercentage(rollNo, subject);
                gotoxy(14, line++);
                cout << subject << ": " << fixed << setprecision(2) << subjectPercentage << "%";
            }
        }

        gotoxy(12, 22);
        cout << "Press any key to continue...";
        _getch();
    }

    void viewDetailedAttendance() {
        system("cls");
        drawBox(10, 2, 65, 20, "DETAILED ATTENDANCE RECORD - " + rollNo);
        
        gotoxy(12, 4);
        setColor(COLOR_YELLOW);
        cout << "Date          | Subject        | Status  ";
        gotoxy(12, 5);
        cout << "--------------|----------------|---------";
        setColor(COLOR_DEFAULT);

        ifstream file(ATTENDANCE_FILE);
        if (!file) {
            setColor(COLOR_RED);
            gotoxy(12, 4);
            cout << "No attendance records found!";
            setColor(COLOR_DEFAULT);
            gotoxy(12, 6);
            cout << "Press any key to continue...";
            _getch();
            return;
        }

        Attendance record;
        bool found = false;
        int line = 6;

        while (record.loadFromFile(file) && line < 18) {
            if (record.getRollNo() == rollNo) {
                gotoxy(12, line++);
                cout << setw(12) << left << record.getDate() 
                     << " | " << setw(14) << left << (record.getSubject().empty() ? "General" : record.getSubject())
                     << " | " << setw(7) << left << (record.getStatus() == "P" ? "Present" : "Absent");
                found = true;
            }
        }
        file.close();

        if (!found) {
            setColor(COLOR_RED);
            gotoxy(12, 4);
            cout << "No attendance records found!";
            setColor(COLOR_DEFAULT);
        }

        gotoxy(12, 19);
        cout << "Press any key to continue...";
        _getch();
    }

    void viewExamResults() {
        vector<ExamResult> results = ReportGenerator::loadStudentExamResults(rollNo);
        
        system("cls");
        drawBox(10, 2, 65, 15, "EXAM RESULTS - " + rollNo);
        
        if (results.empty()) {
            gotoxy(12, 4);
            setColor(COLOR_YELLOW);
            cout << "No exam results available yet.";
            setColor(COLOR_DEFAULT);
        } else {
            gotoxy(12, 4);
            setColor(COLOR_CYAN);
            cout << "Semester  Subject          Exam Type    Marks  Grade";
            gotoxy(12, 5);
            cout << "--------  --------------  -----------  -----  -----";
            setColor(COLOR_DEFAULT);

            int line = 6;
            double totalMarks = 0;
            int count = 0;

            for (const auto& result : results) {
                if (line > 13) break;
                gotoxy(12, line++);
                cout << setw(8) << left << result.getSemester()
                     << "  " << setw(14) << left << result.getSubject()
                     << "  " << setw(11) << left << result.getExamType()
                     << "  " << setw(5) << right << fixed << setprecision(1) << result.getMarks()
                     << "  " << setw(5) << left << result.getGrade();
                
                totalMarks += result.getMarks();
                count++;
            }

            if (count > 0) {
                gotoxy(12, line + 1);
                cout << "Overall Average: " << fixed << setprecision(2) << (totalMarks / count) 
                     << " (" << ExamResult::calculateGrade(totalMarks / count) << ")";
            }
        }

        gotoxy(12, 16);
        cout << "Press any key to continue...";
        _getch();
    }

    void generatePersonalReport() {
        if (ReportGenerator::generateStudentPerformanceReport(rollNo)) {
            displayMessageBox("Personal performance report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("No data available to generate report!", true);
        }
    }

    void viewProfile() {
        Student student = getStudentInfo();
        if (student.getRollNo().empty()) {
            displayMessageBox("Student information not found!", true);
            return;
        }

        system("cls");
        drawBox(20, 5, 40, 12, "STUDENT PROFILE");
        
        gotoxy(22, 7);
        cout << "Roll No: " << student.getRollNo();
        gotoxy(22, 8);
        cout << "Name: " << student.getName();
        gotoxy(22, 9);
        cout << "Department: " << student.getDepartment();
        gotoxy(22, 10);
        cout << "Semester: " << student.getSemester();
        if (!student.getEmail().empty()) {
            gotoxy(22, 11);
            cout << "Email: " << student.getEmail();
        }
        if (!student.getPhone().empty()) {
            gotoxy(22, 12);
            cout << "Phone: " << student.getPhone();
        }

        gotoxy(22, 14);
        cout << "Press any key to continue...";
        _getch();
    }

public:
    StudentUser() : Person(), rollNo("") {}

    bool login() override {
        vector<string> loginOptions = {"Login", "Sign Up", "Back"};
        MenuNavigator loginMenu(loginOptions, "STUDENT PORTAL", 20, 8, 40);
        
        int choice = loginMenu.display();
        
        switch (choice) {
            case 0: return studentLogin();
            case 1: return studentSignup();
            case 2: 
            case -1: return false;
            default: return false;
        }
    }

    bool saveCredentials() override {
        // Credentials are saved during signup, no need to implement here
        return true;
    }

    void displayMenu() override {
        vector<string> studentOptions = {
            "View Attendance Summary",
            "View Detailed Attendance", 
            "View Exam Results",
            "Generate Personal Report",
            "View Profile",
            "Change Password",
            "Logout"
        };

        MenuNavigator studentMenu(studentOptions, "STUDENT DASHBOARD - " + name, 15, 3, 50);
        
        int choice;
        do {
            choice = studentMenu.display();
            
            if (choice == -1) break;

            switch (choice) {
                case 0: viewAttendanceSummary(); break;
                case 1: viewDetailedAttendance(); break;
                case 2: viewExamResults(); break;
                case 3: generatePersonalReport(); break;
                case 4: viewProfile(); break;
                case 5: passwordManager.changePassword(this, "student"); break;
                case 6: return;
            }
        } while (true);
    }

    string getRollNo() const { return rollNo; }
};

// Admin Class (Updated with exam management)
class Admin : public Person {
private:
    vector<Student> students;
    PasswordManager passwordManager;
    NotificationSystem notificationSystem;

    void loadStudents() {
        students.clear();
        ifstream file(STUDENT_FILE);
        if (!file) return;

        Student student;
        while (student.loadFromFile(file)) {
            students.push_back(student);
        }
        file.close();
    }

    void saveStudents() {
        ofstream file(STUDENT_FILE);
        if (!file) return;

        for (const auto& student : students) {
            student.saveToFile(file);
        }
        file.close();
    }

    bool adminLogin() {
        ifstream file(ADMIN_FILE);
        if (!file) {
            createDefaultAdmin();
            file.open(ADMIN_FILE);
        }

        string storedUsername, storedPassword;
        if (!getline(file, storedUsername) || !getline(file, storedPassword)) {
            file.close();
            createDefaultAdmin();
            file.open(ADMIN_FILE);
            getline(file, storedUsername);
            getline(file, storedPassword);
        }
        file.close();

        username = storedUsername;
        password = storedPassword;

        system("cls");
        drawBox(20, 5, 40, 10, "ADMIN LOGIN");
        
        string inputUsername, inputPassword;
        
        gotoxy(25, 7);
        cout << "Username: ";
        cin >> inputUsername;
        
        gotoxy(25, 8);
        cout << "Password: ";
        string hiddenPass = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        inputPassword = hiddenPass;
        cout << endl;

        if (inputUsername == storedUsername && encryptDecrypt(storedPassword) == inputPassword) {
            gotoxy(25, 10);
            setColor(COLOR_GREEN);
            cout << "Login successful!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return true;
        } else {
            gotoxy(25, 10);
            setColor(COLOR_RED);
            cout << "Invalid credentials!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return false;
        }
    }

    void createDefaultAdmin() {
        ofstream file(ADMIN_FILE);
        if (file) {
            file << "admin" << endl;
            file << encryptDecrypt("admin123") << endl;
            file.close();
        }
    }

    void generateComprehensiveReport() {
        loadStudents();
        if (students.empty()) {
            displayMessageBox("No students found to generate report!", true);
            return;
        }

        if (ReportGenerator::generateAttendanceReportCSV(students, "comprehensive")) {
            displayMessageBox("Comprehensive report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating comprehensive report!", true);
        }
    }

    void generateDepartmentWiseReport() {
        loadStudents();
        if (students.empty()) {
            displayMessageBox("No students found to generate report!", true);
            return;
        }

        if (ReportGenerator::generateDepartmentReportCSV(students)) {
            displayMessageBox("Department-wise report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating department report!", true);
        }
    }

    void generateDailyReport() {
        vector<string> dates = Attendance::getAvailableDates();
        if (dates.empty()) {
            displayMessageBox("No attendance records found!", true);
            return;
        }

        system("cls");
        drawBox(15, 3, 50, 8, "GENERATE DAILY REPORT");
        
        gotoxy(17, 5);
        cout << "Available Dates:";
        for (int i = 0; i < dates.size() && i < 3; i++) {
            gotoxy(17, 6 + i);
            cout << (i + 1) << ". " << dates[i];
        }
        
        gotoxy(17, 9);
        cout << "Enter date (YYYY-MM-DD) or 'all': ";
        string date;
        cin >> date;

        if (ReportGenerator::generateDailyAttendanceReportCSV(date)) {
            displayMessageBox("Daily report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating daily report!", true);
        }
    }

    void addExamResult() {
        system("cls");
        drawBox(15, 3, 50, 12, "ADD EXAM RESULT");
        
        string rollNo, semester, subject, examType;
        double marks;
        
        gotoxy(17, 5);
        cout << "Student Roll No: ";
        cin >> rollNo;
        
        // Verify student exists
        bool studentExists = false;
        for (const auto& student : students) {
            if (student.getRollNo() == rollNo) {
                studentExists = true;
                break;
            }
        }
        
        if (!studentExists) {
            displayMessageBox("Student not found!", true);
            return;
        }
        
        gotoxy(17, 6);
        cout << "Semester: ";
        cin >> semester;
        
        gotoxy(17, 7);
        cout << "Subject: ";
        cin.ignore();
        getline(cin, subject);
        
        gotoxy(17, 8);
        cout << "Exam Type (Midterm/Final/Quiz): ";
        getline(cin, examType);
        
        gotoxy(17, 9);
        cout << "Marks: ";
        cin >> marks;
        
        string grade = ExamResult::calculateGrade(marks);
        
        ExamResult result(rollNo, semester, subject, marks, grade, examType);
        
        ofstream file(EXAM_RESULTS_FILE, ios::app);
        if (!file) {
            displayMessageBox("Error saving exam result!", true);
            return;
        }
        
        result.saveToFile(file);
        file.close();
        
        notificationSystem.addExamResultNotification(rollNo, subject, grade);
        displayMessageBox("Exam result added successfully!", false);
    }

    void viewExamResults() {
        system("cls");
        drawBox(10, 2, 70, 20, "ALL EXAM RESULTS");
        
        ifstream file(EXAM_RESULTS_FILE);
        if (!file) {
            gotoxy(12, 4);
            cout << "No exam results found!";
            gotoxy(12, 6);
            cout << "Press any key to continue...";
            _getch();
            return;
        }
        
        gotoxy(12, 4);
        setColor(COLOR_CYAN);
        cout << "Roll No  Semester  Subject          Exam Type    Marks  Grade";
        gotoxy(12, 5);
        cout << "-------  --------  --------------  -----------  -----  -----";
        setColor(COLOR_DEFAULT);
        
        ExamResult result;
        int line = 6;
        while (result.loadFromFile(file) && line < 18) {
            gotoxy(12, line++);
            cout << setw(7) << left << result.getRollNo()
                 << "  " << setw(8) << left << result.getSemester()
                 << "  " << setw(14) << left << result.getSubject()
                 << "  " << setw(11) << left << result.getExamType()
                 << "  " << setw(5) << right << fixed << setprecision(1) << result.getMarks()
                 << "  " << setw(5) << left << result.getGrade();
        }
        file.close();
        
        gotoxy(12, 19);
        cout << "Press any key to continue...";
        _getch();
    }

public:
    Admin() : Person() {
        loadStudents();
    }

    bool login() override {
        return adminLogin();
    }

    bool saveCredentials() override {
        ofstream file(ADMIN_FILE);
        if (!file) return false;
        
        file << username << endl;
        file << password << endl;
        file.close();
        return true;
    }

    void displayMenu() override {
        vector<string> adminOptions = {
            "Add New Student",
            "View All Students", 
            "Update Student",
            "Delete Student",
            "Search Student",
            "View Attendance Report",
            "Add Exam Result",
            "View Exam Results",
            "Generate Comprehensive Report",
            "Generate Department Report",
            "Generate Daily Report",
            "System Statistics",
            "Change Password",
            "View Notifications",
            "Logout"
        };

        MenuNavigator adminMenu(adminOptions, "ADMIN PANEL", 15, 3, 50);
        
        int choice;
        do {
            choice = adminMenu.display();
            
            if (choice == -1) break;

            switch (choice) {
                case 0: addStudent(); break;
                case 1: viewAllStudents(); break;
                case 2: updateStudent(); break;
                case 3: deleteStudent(); break;
                case 4: searchStudent(); break;
                case 5: viewAttendanceReport(); break;
                case 6: addExamResult(); break;
                case 7: viewExamResults(); break;
                case 8: generateComprehensiveReport(); break;
                case 9: generateDepartmentWiseReport(); break;
                case 10: generateDailyReport(); break;
                case 11: showStatistics(); break;
                case 12: passwordManager.changePassword(this, "admin"); break;
                case 13: notificationSystem.showNotifications(); break;
                case 14: return;
            }
        } while (true);
    }

    void addStudent() {
        system("cls");
        drawBox(15, 3, 50, 14, "ADD NEW STUDENT");
        
        string rollNo, name, department, email, phone;
        int semester;

        gotoxy(17, 5);
        cout << "Roll No: ";
        cin >> rollNo;

        for (const auto& student : students) {
            if (student.getRollNo() == rollNo) {
                displayMessageBox("Error: Roll number already exists!", true);
                return;
            }
        }

        cin.ignore();
        gotoxy(17, 6);
        cout << "Name: ";
        getline(cin, name);
        
        gotoxy(17, 7);
        cout << "Department: ";
        getline(cin, department);
        
        gotoxy(17, 8);
        cout << "Semester: ";
        cin >> semester;
        
        cin.ignore();
        gotoxy(17, 9);
        cout << "Email (optional): ";
        getline(cin, email);
        
        gotoxy(17, 10);
        cout << "Phone (optional): ";
        getline(cin, phone);

        if (validateRollNo(rollNo) && validateName(name)) {
            Student newStudent(rollNo, name, department, semester, email, phone);
            students.push_back(newStudent);
            saveStudents();
            displayMessageBox("Student added successfully!", false);
        } else {
            displayMessageBox("Invalid roll number or name!", true);
        }
    }

    void viewAllStudents() {
        loadStudents();
        system("cls");
        int boxHeight = max(static_cast<int>(students.size()) + 9, 12);
        drawBox(3, 2, 74, boxHeight, "ALL STUDENTS");
        
        if (students.empty()) {
            setColor(COLOR_RED);
            gotoxy(5, 4);
            cout << "No students found!";
            setColor(COLOR_DEFAULT);
        } else {
            setColor(COLOR_CYAN);
            gotoxy(5, 4);
            cout << "Total Students: " << students.size();
            setColor(COLOR_DEFAULT);
            
            gotoxy(5, 6);
            setColor(COLOR_YELLOW);
            cout << "Roll No  Name                Department    Semester  Email";
            gotoxy(5, 7);
            cout << "-------  ------------------  ------------  --------  -----";
            setColor(COLOR_DEFAULT);
            
            for (int i = 0; i < students.size(); i++) {
                gotoxy(5, 8 + i);
                cout << setw(7) << left << students[i].getRollNo() 
                     << "  " << setw(18) << left << students[i].getName()
                     << "  " << setw(12) << left << students[i].getDepartment()
                     << "  " << setw(8) << left << students[i].getSemester()
                     << "  " << setw(15) << left << (students[i].getEmail().empty() ? "N/A" : students[i].getEmail());
            }
        }
        
        setColor(COLOR_YELLOW);
        gotoxy(5, boxHeight - 2);
        cout << "\n Press any key to continue...";
        setColor(COLOR_DEFAULT);
        _getch();
    }

    void updateStudent() {
        system("cls");
        drawBox(15, 3, 50, 12, "UPDATE STUDENT");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter roll number to update: ";
        cin >> rollNo;

        for (auto& student : students) {
            if (student.getRollNo() == rollNo) {
                string name, department, email, phone;
                int semester;

                cin.ignore();
                gotoxy(17, 7);
                cout << "New Name: ";
                getline(cin, name);
                
                gotoxy(17, 8);
                cout << "New Department: ";
                getline(cin, department);
                
                gotoxy(17, 9);
                cout << "New Semester: ";
                cin >> semester;
                
                cin.ignore();
                gotoxy(17, 10);
                cout << "New Email: ";
                getline(cin, email);
                
                gotoxy(17, 11);
                cout << "New Phone: ";
                getline(cin, phone);

                if (validateName(name)) {
                    student.setName(name);
                    student.setDepartment(department);
                    student.setSemester(semester);
                    student.setEmail(email);
                    student.setPhone(phone);
                    saveStudents();
                    displayMessageBox("Student updated successfully!", false);
                    return;
                } else {
                    displayMessageBox("Invalid name!", true);
                    return;
                }
            }
        }
        displayMessageBox("Student not found!", true);
    }

    void deleteStudent() {
        system("cls");
        drawBox(15, 3, 50, 6, "DELETE STUDENT");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter roll number to delete: ";
        cin >> rollNo;

        auto it = remove_if(students.begin(), students.end(),
            [&rollNo](const Student& s) { return s.getRollNo() == rollNo; });

        if (it != students.end()) {
            students.erase(it, students.end());
            saveStudents();
            displayMessageBox("Student deleted successfully!", false);
        } else {
            displayMessageBox("Student not found!", true);
        }
    }

    void searchStudent() {
        system("cls");
        drawBox(15, 3, 50, 8, "SEARCH STUDENT");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter roll number: ";
        cin >> rollNo;

        bool found = false;
        for (const auto& student : students) {
            if (student.getRollNo() == rollNo) {
                found = true;
                system("cls");
                drawBox(15, 3, 50, 12, "STUDENT FOUND");
                gotoxy(17, 5);
                cout << "Roll No: " << student.getRollNo();
                gotoxy(17, 6);
                cout << "Name: " << student.getName();
                gotoxy(17, 7);
                cout << "Department: " << student.getDepartment();
                gotoxy(17, 8);
                cout << "Semester: " << student.getSemester();
                if (!student.getEmail().empty()) {
                    gotoxy(17, 9);
                    cout << "Email: " << student.getEmail();
                }
                if (!student.getPhone().empty()) {
                    gotoxy(17, 10);
                    cout << "Phone: " << student.getPhone();
                }
                
                double percentage = Attendance::calculateAttendancePercentage(rollNo);
                gotoxy(17, 11);
                cout << "Attendance: " << fixed << setprecision(2) << percentage << "%";
                
                gotoxy(17, 13);
                cout << "Press any key to continue...";
                _getch();
                return;
            }
        }
        
        if (!found) {
            displayMessageBox("Student not found!", true);
        }
    }

    void viewAttendanceReport() {
        loadStudents();
        system("cls");
        int boxHeight = max(static_cast<int>(students.size()) + 9, 12);
        drawBox(3, 2, 74, boxHeight, "ATTENDANCE REPORT");
        
        setColor(COLOR_CYAN);
        gotoxy(5, 4);
        cout << "Roll No  Name                Percentage";
        gotoxy(5, 5);
        cout << "-------  ------------------  ----------";
        setColor(COLOR_DEFAULT);

        for (int i = 0; i < students.size(); i++) {
            double percentage = Attendance::calculateAttendancePercentage(students[i].getRollNo());
            gotoxy(5, 6 + i);
            cout << setw(7) << left << students[i].getRollNo() 
                 << "  " << setw(18) << left << students[i].getName() << "  ";
            
            if (percentage >= 75) {
                setColor(COLOR_GREEN);
            } else if (percentage >= 50) {
                setColor(COLOR_YELLOW);
            } else {
                setColor(COLOR_RED);
            }
            
            cout << fixed << setprecision(2) << percentage << "%";
            setColor(COLOR_DEFAULT);
            
            if (percentage < 75) {
                notificationSystem.addLowAttendanceAlert(students[i].getRollNo(), percentage);
            }
        }
        
        setColor(COLOR_YELLOW);
        gotoxy(5, boxHeight - 2);
        cout << "Press any key to continue...";
        setColor(COLOR_DEFAULT);
        _getch();
    }

    void showStatistics() {
        loadStudents();
        
        int totalStudents = students.size();
        map<string, int> deptCount;
        map<int, int> semCount;
        
        for (const auto& student : students) {
            deptCount[student.getDepartment()]++;
            semCount[student.getSemester()]++;
        }
        
        // Count student accounts
        int studentAccounts = 0;
        ifstream credFile(STUDENT_CREDENTIALS_FILE);
        if (credFile) {
            string line;
            while (getline(credFile, line)) {
                if (line.find('|') == string::npos) { // Simple line count for credentials
                    studentAccounts++;
                }
            }
            credFile.close();
        }
        studentAccounts /= 4; // Each account has 4 lines
        
        system("cls");
        drawBox(10, 2, 60, 18, "SYSTEM STATISTICS");
        
        gotoxy(12, 4);
        cout << "Total Students in Database: " << totalStudents;
        gotoxy(12, 5);
        cout << "Student Accounts Created: " << studentAccounts;
        
        gotoxy(12, 7);
        cout << "Department-wise Distribution:";
        int line = 8;
        for (const auto& dept : deptCount) {
            gotoxy(15, line++);
            cout << dept.first << ": " << dept.second << " students";
        }
        
        gotoxy(12, line + 1);
        cout << "Semester-wise Distribution:";
        line += 2;
        for (const auto& sem : semCount) {
            gotoxy(15, line++);
            cout << "Semester " << sem.first << ": " << sem.second << " students";
        }
        
        gotoxy(12, line + 1);
        cout << "Press any key to continue...";
        _getch();
    }
};

// User Class (Teacher) - Updated with subject-wise attendance
class User : public Person {
private:
    PasswordManager passwordManager;

    bool teacherLogin() {
        ifstream file(TEACHER_FILE);
        if (!file) {
            createDefaultTeacher();
            file.open(TEACHER_FILE);
        }

        string storedUsername, storedPassword;
        if (!getline(file, storedUsername) || !getline(file, storedPassword)) {
            file.close();
            createDefaultTeacher();
            file.open(TEACHER_FILE);
            getline(file, storedUsername);
            getline(file, storedPassword);
        }
        file.close();

        username = storedUsername;
        password = storedPassword;

        system("cls");
        drawBox(20, 5, 40, 10, "TEACHER LOGIN");
        
        string inputUsername, inputPassword;
        
        gotoxy(25, 7);
        cout << "Username: ";
        cin >> inputUsername;
        
        gotoxy(25, 8);
        cout << "Password: ";
        string hiddenPass = "";
        char ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!hiddenPass.empty()) {
                    hiddenPass.pop_back();
                    cout << "\b \b";
                }
            } else {
                hiddenPass += ch;
                cout << '*';
            }
        }
        inputPassword = hiddenPass;
        cout << endl;

        if (inputUsername == storedUsername && encryptDecrypt(storedPassword) == inputPassword) {
            gotoxy(25, 10);
            setColor(COLOR_GREEN);
            cout << "Login successful!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return true;
        } else {
            gotoxy(25, 10);
            setColor(COLOR_RED);
            cout << "Invalid credentials!";
            setColor(COLOR_DEFAULT);
            sleepMs(1000);
            return false;
        }
    }

    void createDefaultTeacher() {
        ofstream file(TEACHER_FILE);
        if (file) {
            file << "teacher" << endl;
            file << encryptDecrypt("teacher123") << endl;
            file.close();
        }
    }

    vector<Student> loadAllStudents() {
        vector<Student> allStudents;
        ifstream file(STUDENT_FILE);
        if (!file) return allStudents;

        Student student;
        while (student.loadFromFile(file)) {
            allStudents.push_back(student);
        }
        file.close();
        return allStudents;
    }

    void generateClassReport() {
        vector<Student> students = loadAllStudents();
        if (students.empty()) {
            displayMessageBox("No students found to generate report!", true);
            return;
        }

        if (ReportGenerator::generateAttendanceReportCSV(students, "class")) {
            displayMessageBox("Class report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating class report!", true);
        }
    }

    void generateStudentMonthlyReport() {
        vector<Student> students = loadAllStudents();
        if (students.empty()) {
            displayMessageBox("No students found!", true);
            return;
        }

        system("cls");
        drawBox(15, 3, 50, 8, "STUDENT MONTHLY REPORT");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter student roll number: ";
        cin >> rollNo;

        bool studentExists = false;
        for (const auto& student : students) {
            if (student.getRollNo() == rollNo) {
                studentExists = true;
                break;
            }
        }

        if (!studentExists) {
            displayMessageBox("Student not found!", true);
            return;
        }

        gotoxy(17, 7);
        cout << "Enter month and year (YYYY-MM): ";
        string monthYear;
        cin >> monthYear;

        if (ReportGenerator::generateMonthlyReportCSV(rollNo, monthYear)) {
            displayMessageBox("Monthly report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating monthly report!", true);
        }
    }

    void generateDailyAttendanceReport() {
        vector<string> dates = Attendance::getAvailableDates();
        if (dates.empty()) {
            displayMessageBox("No attendance records found!", true);
            return;
        }

        system("cls");
        drawBox(15, 3, 50, 8, "GENERATE DAILY REPORT");
        
        gotoxy(17, 5);
        cout << "Available Dates:";
        for (int i = 0; i < dates.size() && i < 3; i++) {
            gotoxy(17, 6 + i);
            cout << (i + 1) << ". " << dates[i];
        }
        
        gotoxy(17, 9);
        cout << "Enter date (YYYY-MM-DD) or 'all': ";
        string date;
        cin >> date;

        if (ReportGenerator::generateDailyAttendanceReportCSV(date)) {
            displayMessageBox("Daily report generated successfully as CSV file!", false);
        } else {
            displayMessageBox("Error generating daily report!", true);
        }
    }

public:
    User() : Person() {}

    bool login() override {
        return teacherLogin();
    }

    bool saveCredentials() override {
        ofstream file(TEACHER_FILE);
        if (!file) return false;
        
        file << username << endl;
        file << password << endl;
        file.close();
        return true;
    }

    void displayMenu() override {
        vector<string> teacherOptions = {
            "Mark Daily Attendance",
            "Mark Subject-wise Attendance",
            "Quick Mark Attendance",
            "View Individual Attendance", 
            "Check Attendance Percentage",
            "Generate Class Report",
            "Generate Student Monthly Report",
            "Generate Daily Report",
            "Change Password",
            "Logout"
        };

        MenuNavigator teacherMenu(teacherOptions, "TEACHER PANEL", 15, 3, 50);
        
        int choice;
        do {
            choice = teacherMenu.display();
            
            if (choice == -1) break;

            switch (choice) {
                case 0: markAttendance(); break;
                case 1: markSubjectAttendance(); break;
                case 2: quickMarkAttendance(); break;
                case 3: viewIndividualAttendance(); break;
                case 4: checkAttendancePercentage(); break;
                case 5: generateClassReport(); break;
                case 6: generateStudentMonthlyReport(); break;
                case 7: generateDailyAttendanceReport(); break;
                case 8: passwordManager.changePassword(this, "teacher"); break;
                case 9: return;
            }
        } while (true);
    }

    void markAttendance() {
        vector<Student> students = loadAllStudents();
        if (students.empty()) {
            displayMessageBox("No students found!", true);
            return;
        }

        string date = getCurrentDate();
        system("cls");
        int boxHeight = students.size() + 7;
        if (boxHeight > 25) boxHeight = 25;
        
        drawBox(10, 2, 65, boxHeight, "MARK ATTENDANCE - " + date);

        ofstream file(ATTENDANCE_FILE, ios::app);
        if (!file) {
            displayMessageBox("Error opening attendance file!", true);
            return;
        }

        for (int i = 0; i < students.size() && i < boxHeight - 7; i++) {
            char status;
            gotoxy(12, 4 + i);
            setColor(COLOR_YELLOW);
            cout << students[i].getRollNo();
            setColor(COLOR_DEFAULT);
            cout << " - " << students[i].getName();
            gotoxy(52, 4 + i);
            cout << "(P/A): ";
            cin >> status;

            string attendanceStatus = (toupper(status) == 'P') ? "P" : "A";
            Attendance record(students[i].getRollNo(), date, attendanceStatus);
            record.saveToFile(file);
        }
        file.close();
        
        displayMessageBox("Attendance marked successfully!", false);
    }

    void markSubjectAttendance() {
        vector<Student> students = loadAllStudents();
        if (students.empty()) {
            displayMessageBox("No students found!", true);
            return;
        }

        string date = getCurrentDate();
        string subject;
        
        system("cls");
        drawBox(15, 3, 50, 8, "SUBJECT-WISE ATTENDANCE");
        
        gotoxy(17, 5);
        cout << "Enter subject name: ";
        cin.ignore();
        getline(cin, subject);
        
        system("cls");
        int boxHeight = students.size() + 7;
        if (boxHeight > 25) boxHeight = 25;
        
        drawBox(10, 2, 65, boxHeight, "MARK ATTENDANCE - " + subject + " - " + date);

        ofstream file(ATTENDANCE_FILE, ios::app);
        if (!file) {
            displayMessageBox("Error opening attendance file!", true);
            return;
        }

        for (int i = 0; i < students.size() && i < boxHeight - 7; i++) {
            char status;
            gotoxy(12, 4 + i);
            setColor(COLOR_YELLOW);
            cout << students[i].getRollNo();
            setColor(COLOR_DEFAULT);
            cout << " - " << students[i].getName();
            gotoxy(52, 4 + i);
            cout << "(P/A): ";
            cin >> status;

            string attendanceStatus = (toupper(status) == 'P') ? "P" : "A";
            Attendance record(students[i].getRollNo(), date, attendanceStatus, subject);
            record.saveToFile(file);
        }
        file.close();
        
        displayMessageBox("Subject attendance marked successfully!", false);
    }

    void quickMarkAttendance() {
        vector<Student> students = loadAllStudents();
        if (students.empty()) {
            displayMessageBox("No students found!", true);
            return;
        }

        string date = getCurrentDate();
        
        system("cls");
        drawBox(10, 2, 50, 8, "QUICK ATTENDANCE - " + date);
        
        gotoxy(12, 4);
        cout << "Mark all as (P)resent or (A)bsent: ";
        char choice;
        cin >> choice;
        
        string status;
        if (toupper(choice) == 'P') {
            status = "P";
        } else if (toupper(choice) == 'A') {
            status = "A";
        } else {
            displayMessageBox("Invalid choice! Please enter P or A.", true);
            return;
        }
        
        ofstream file(ATTENDANCE_FILE, ios::app);
        if (!file) {
            displayMessageBox("Error opening attendance file!", true);
            return;
        }
        
        for (const auto& student : students) {
            Attendance record(student.getRollNo(), date, status);
            record.saveToFile(file);
        }
        file.close();
        
        string message = "Attendance marked for " + to_string(students.size()) + 
                        " students as " + (status == "P" ? "Present" : "Absent");
        displayMessageBox(message, false);
    }

    void viewIndividualAttendance() {
        system("cls");
        drawBox(15, 3, 50, 8, "VIEW INDIVIDUAL ATTENDANCE");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter student roll number: ";
        cin >> rollNo;

        ifstream file(ATTENDANCE_FILE);
        if (!file) {
            displayMessageBox("No attendance records found!", true);
            return;
        }

        system("cls");
        drawBox(10, 2, 65, 20, "ATTENDANCE RECORD - " + rollNo);
        
        gotoxy(12, 4);
        setColor(COLOR_YELLOW);
        cout << "Date          | Subject        | Status  ";
        gotoxy(12, 5);
        cout << "--------------|----------------|---------";
        setColor(COLOR_DEFAULT);

        Attendance record;
        bool found = false;
        int line = 6;

        while (record.loadFromFile(file) && line < 18) {
            if (record.getRollNo() == rollNo) {
                gotoxy(12, line++);
                cout << setw(12) << left << record.getDate() 
                     << " | " << setw(14) << left << (record.getSubject().empty() ? "General" : record.getSubject())
                     << " | " << setw(7) << left << (record.getStatus() == "P" ? "Present" : "Absent");
                found = true;
            }
        }
        file.close();

        if (!found) {
            setColor(COLOR_RED);
            gotoxy(12, 4);
            cout << "No attendance records found!";
            setColor(COLOR_DEFAULT);
        } else {
            double percentage = Attendance::calculateAttendancePercentage(rollNo);
            
            gotoxy(12, line + 1);
            cout << "Overall Attendance: ";
            if (percentage >= 75) {
                setColor(COLOR_GREEN);
            } else if (percentage >= 50) {
                setColor(COLOR_YELLOW);
            } else {
                setColor(COLOR_RED);
            }
            cout << fixed << setprecision(2) << percentage << "%";
            setColor(COLOR_DEFAULT);
        }
        
        gotoxy(12, 19);
        cout << "Press any key to continue...";
        _getch();
    }

    void checkAttendancePercentage() {
        system("cls");
        drawBox(15, 3, 50, 8, "CHECK ATTENDANCE PERCENTAGE");
        
        string rollNo;
        gotoxy(17, 5);
        cout << "Enter student roll number: ";
        cin >> rollNo;

        double percentage = Attendance::calculateAttendancePercentage(rollNo);
        
        system("cls");
        drawBox(20, 8, 40, 7, "ATTENDANCE RESULT");
        gotoxy(22, 10);
        cout << "Roll Number: " << rollNo;
        gotoxy(22, 12);
        
        if (percentage >= 75) {
            setColor(COLOR_GREEN);
        } else if (percentage >= 50) {
            setColor(COLOR_YELLOW);
        } else {
            setColor(COLOR_RED);
        }
        cout << "Percentage: " << fixed << setprecision(2) << percentage << "%";
        setColor(COLOR_DEFAULT);
        
        gotoxy(22, 14);
        cout << "Press any key to continue...";
        _getch();
    }
};

// Main Application Class
class AttendanceSystem {
private:
    void showMainMenu() {
        vector<string> mainOptions = {
            "Admin Panel",
            "Teacher Panel", 
            "Student Portal",
            "Exit System"
        };

        MenuNavigator mainMenu(mainOptions, "STUDENT ATTENDANCE MANAGEMENT SYSTEM", 15, 5, 50);
        
        int choice = mainMenu.display();
        
        switch (choice) {
            case 0: {
                Admin admin;
                if (admin.login()) {
                    admin.displayMenu();
                }
                break;
            }
            case 1: {
                User user;
                if (user.login()) {
                    user.displayMenu();
                }
                break;
            }
            case 2: {
                StudentUser student;
                if (student.login()) {
                    student.displayMenu();
                }
                break;
            }
            case 3: 
            case -1:
                system("cls");
                drawBox(25, 10, 30, 5, "GOODBYE");
                gotoxy(27, 12);
                cout << "Thank you for using";
                gotoxy(30, 13);
                cout << "the system!";
                sleepMs(2000);
                exit(0);
        }
    }

public:
    void run() {
        displayWelcomeBanner();
        
        while (true) {
            showMainMenu();
        }
    }
};

int main() {
#ifdef _WIN32
    system("title Student Attendance Management System");
    system("mode con: lines=30 cols=80");
#endif
    
    AttendanceSystem system;
    system.run();
    return 0;
}
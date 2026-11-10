
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class Status { PASS, FAIL };

struct Student {
    int id;
    std::string name;
    double marks[3];
    double average;
    Status status;
};

const double PASS_MARK = 50.0;
const double EPS = 1e-9;

// ---------- input helpers (strict: whole line must be a valid number) ----------
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) { std::cout << "\nInput closed. Goodbye.\n"; std::exit(0); }
    return line;
}
bool parseInt(const std::string& s, int& out) {
    std::istringstream iss(s);
    char extra;
    return (iss >> out) && !(iss >> extra);
}
bool parseDouble(const std::string& s, double& out) {
    std::istringstream iss(s);
    char extra;
    return (iss >> out) && !(iss >> extra) && std::isfinite(out);
}
int askInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        int v;
        if (parseInt(readLine(prompt), v) && v >= lo && v <= hi) return v;
        std::cout << "  Invalid: enter a whole number from " << lo << " to " << hi << ".\n";
    }
}
double askMark(const std::string& prompt) {
    while (true) {
        double v;
        if (parseDouble(readLine(prompt), v) && v >= 0 && v <= 100) return v;
        std::cout << "  Invalid: mark must be a number from 0 to 100.\n";
    }
}
std::string askName() {
    while (true) {
        std::string n = readLine("Name: ");
        std::size_t b = n.find_first_not_of(" \t");
        if (b != std::string::npos) return n.substr(b, n.find_last_not_of(" \t") - b + 1);
        std::cout << "  Name cannot be empty.\n";
    }
}

// ---------- core logic ----------
void updateResult(Student& s) {          // reference parameter: modifies the caller's record
    s.average = (s.marks[0] + s.marks[1] + s.marks[2]) / 3.0;
    s.status = (s.average >= PASS_MARK) ? Status::PASS : Status::FAIL;
}
int findIndex(const std::vector<Student>& v, int id) {
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i].id == id) return static_cast<int>(i);
    return -1;
}
void printHeader() {
    std::cout << std::left << std::setw(8) << "ID" << std::setw(14) << "Name" << std::setw(8) << "M1"
              << std::setw(8) << "M2" << std::setw(8) << "M3" << std::setw(10) << "Average" << "Status\n"
              << std::string(64, '-') << "\n";
}
void printRow(const Student& s) {
    std::cout << std::left << std::setw(8) << s.id << std::setw(14) << s.name << std::fixed << std::setprecision(2)
              << std::setw(8) << s.marks[0] << std::setw(8) << s.marks[1] << std::setw(8) << s.marks[2]
              << std::setw(10) << s.average << (s.status == Status::PASS ? "PASS" : "FAIL") << "\n";
}

// ---------- menu actions ----------
void registerStudent(std::vector<Student>& v) {
    int id = askInt("Student ID (1-999999): ", 1, 999999);
    if (findIndex(v, id) != -1) { std::cout << "Duplicate ID: a student with ID " << id << " already exists.\n"; return; }
    Student s{};
    s.id = id;
    s.name = askName();
    for (int i = 0; i < 3; ++i) s.marks[i] = askMark("Mark " + std::to_string(i + 1) + " (0-100): ");
    updateResult(s);
    v.push_back(s);
    std::cout << "Registered " << s.name << ".\n";
}
void listStudents(const std::vector<Student>& v) {
    if (v.empty()) { std::cout << "No students registered.\n"; return; }
    printHeader();
    for (const Student& s : v) printRow(s);
}
void searchStudent(const std::vector<Student>& v) {
    if (v.empty()) { std::cout << "No students registered.\n"; return; }
    int id = askInt("ID to search: ", 1, 999999);
    int i = findIndex(v, id);
    if (i < 0) { std::cout << "Student with ID " << id << " not found.\n"; return; }
    printHeader();
    printRow(v[static_cast<std::size_t>(i)]);
}
void updateMarks(std::vector<Student>& v) {
    if (v.empty()) { std::cout << "No students registered.\n"; return; }
    int id = askInt("ID to update: ", 1, 999999);
    int i = findIndex(v, id);
    if (i < 0) { std::cout << "Student with ID " << id << " not found.\n"; return; }
    Student& s = v[static_cast<std::size_t>(i)];
    for (int k = 0; k < 3; ++k) s.marks[k] = askMark("New mark " + std::to_string(k + 1) + " (0-100): ");
    updateResult(s);
    std::cout << "Updated. New average " << std::fixed << std::setprecision(2) << s.average << ", "
              << (s.status == Status::PASS ? "PASS" : "FAIL") << ".\n";
}
void classStatistics(const std::vector<Student>& v) {
    if (v.empty()) { std::cout << "No students registered.\n"; return; }
    double total = 0;
    for (const Student& s : v) total += s.average;
    auto mm = std::minmax_element(v.begin(), v.end(),
                                  [](const Student& a, const Student& b) { return a.average < b.average; });
    auto passes = std::count_if(v.begin(), v.end(), [](const Student& s) { return s.status == Status::PASS; });
    std::cout << std::fixed << std::setprecision(2)
              << "Students     : " << v.size() << "\n"
              << "Class average: " << total / static_cast<double>(v.size()) << "\n"
              << "Highest      : " << mm.second->average << " (" << mm.second->name << ")\n"
              << "Lowest       : " << mm.first->average << " (" << mm.first->name << ")\n"
              << "Passed       : " << passes << "\nFailed       : " << static_cast<long>(v.size()) - passes << "\n";
}
void topStudent(const std::vector<Student>& v) {
    if (v.empty()) { std::cout << "No students registered.\n"; return; }
    double best = std::max_element(v.begin(), v.end(), [](const Student& a, const Student& b) {
                      return a.average < b.average; })->average;
    std::cout << "Top average: " << std::fixed << std::setprecision(2) << best << "\n";
    printHeader();
    for (const Student& s : v)
        if (std::fabs(s.average - best) < EPS) printRow(s);      // prints every tied student
}
void passList(const std::vector<Student>& v) {
    std::vector<Student> passed;
    std::copy_if(v.begin(), v.end(), std::back_inserter(passed),
                 [](const Student& s) { return s.status == Status::PASS; });   // lambda filter
    if (passed.empty()) { std::cout << "No passing students.\n"; return; }
    std::sort(passed.begin(), passed.end(), [](const Student& a, const Student& b) {
        if (std::fabs(a.average - b.average) > EPS) return a.average > b.average;
        return a.id < b.id;
    });
    printHeader();
    for (const Student& s : passed) printRow(s);
}
void loadDemo(std::vector<Student>& v) {
    const struct { int id; const char* name; double m[3]; } d[10] = {
        {101, "Amina", {90, 85, 95}}, {102, "Brice", {45, 50, 40}}, {103, "Chantal", {60, 65, 70}},
        {104, "Daniel", {49.99, 49.99, 49.99}}, {105, "Estelle", {50, 50, 50}}, {106, "Frank", {30, 35, 25}},
        {107, "Grace", {90, 85, 95}}, {108, "Hugo", {75, 80, 70}}, {109, "Irene", {55, 45, 60}},
        {110, "Jean", {100, 0, 50}}};
    for (const auto& r : d) {
        Student s{};
        s.id = r.id; s.name = r.name;
        for (int i = 0; i < 3; ++i) s.marks[i] = r.m[i];
        updateResult(s);
        v.push_back(s);
    }
}

int main(int argc, char* argv[]) {
    std::vector<Student> students;
    students.reserve(10);
    if (argc > 1 && std::string(argv[1]) == "--demo") { loadDemo(students); std::cout << "(demo data loaded)\n"; }

    int choice = 0;
    do {
        std::cout << "\n=== Student Academic Management System ===\n"
                     "1 Register   2 List   3 Search by ID   4 Update marks\n"
                     "5 Class statistics   6 Top student   7 Pass list   8 Exit\n";
        std::string line = readLine("Choice: ");
        if (!parseInt(line, choice)) choice = 0;
        switch (choice) {
            case 1: registerStudent(students); break;
            case 2: listStudents(students); break;
            case 3: searchStudent(students); break;
            case 4: updateMarks(students); break;
            case 5: classStatistics(students); break;
            case 6: topStudent(students); break;
            case 7: passList(students); break;
            case 8: std::cout << "Goodbye.\n"; break;
            default: std::cout << "Invalid menu choice. Enter 1-8.\n";
        }
    } while (choice != 8);
    return 0;
}

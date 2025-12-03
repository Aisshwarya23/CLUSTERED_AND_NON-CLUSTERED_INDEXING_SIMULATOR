#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <sys/stat.h>
#include <direct.h>
#include <unordered_set>
#include <map>

using namespace std;

void createDirectory(const string& path) {
    #ifdef _WIN32
        _mkdir(path.c_str());
    #else
        mkdir(path.c_str(), 0777);
    #endif
}

struct Record {
    int id;
    string name;
    int salary;

    string toString() const {
        return to_string(id) + "," + name + "," + to_string(salary);
    }
};

vector<Record> readData(const string& filename) {
    vector<Record> records;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Record r;
        getline(ss, token, ','); r.id = stoi(token);
        getline(ss, r.name, ',');
        getline(ss, token, ','); r.salary = stoi(token);
        records.push_back(r);
    }
    return records;
}

void writeData(const string& filename, const vector<Record>& records) {
    ofstream file(filename);
    unordered_set<string> seen;
    for (const auto& r : records) {
        if (seen.insert(r.toString()).second) {
            file << r.toString() << "\n";
        }
    }
}

void logAccess(const string& filename) {
    ofstream log("output/access_log.txt", ios::app);
    log << "Accessed: " << filename << "\n";
}

void writeFinalSorted(const vector<Record>& sorted, const string& filename) {
    writeData(filename, sorted);
    logAccess(filename);
}

void clusteredIndexByName(const vector<Record>& records) {
    vector<Record> sorted = records;
    sort(sorted.begin(), sorted.end(), [](Record a, Record b) { return a.name < b.name; });

    createDirectory("output/clustered_name_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : sorted) {
        string s1 = r.name.substr(0, 1);
        string s2 = r.name.substr(0, 2);
        string s3 = r.name.substr(0, 3);

        vector<string> paths = {
            "output/clustered_name_sort/name_" + s1 + ".txt",
            "output/clustered_name_sort/name_" + s2 + ".txt",
            "output/clustered_name_sort/name_" + s3 + ".txt"
        };

        for (const auto& path : paths) {
            if (written[path].insert(r.toString()).second) {
                ofstream out(path, ios::app);
                out << r.toString() << "\n";
                out.close();
                logAccess(path);
            }
        }
    }

    writeFinalSorted(sorted, "output/final_sorted_result_name.txt");
}

void clusteredIndexById(const vector<Record>& records) {
    vector<Record> sorted = records;
    sort(sorted.begin(), sorted.end(), [](Record a, Record b) { return a.id < b.id; });

    createDirectory("output/clustered_id_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : sorted) {
        string path = "output/clustered_id_sort/id_" + to_string(r.id).substr(0, 1) + ".txt";
        if (written[path].insert(r.toString()).second) {
            ofstream out(path, ios::app);
            out << r.toString() << "\n";
            out.close();
            logAccess(path);
        }
    }

    writeFinalSorted(sorted, "output/final_sorted_result_id.txt");
}

void clusteredIndexBySalary(const vector<Record>& records) {
    vector<Record> sorted = records;
    sort(sorted.begin(), sorted.end(), [](Record a, Record b) { return a.salary < b.salary; });

    createDirectory("output/clustered_salary_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : sorted) {
        int rangeStart = (r.salary / 1000) * 1000;
        int rangeEnd = rangeStart + 999;
        stringstream ss;
        ss << "output/clustered_salary_sort/salary_" << rangeStart << "_" << rangeEnd << ".txt";
        string path = ss.str();

        if (written[path].insert(r.toString()).second) {
            ofstream out(path, ios::app);
            out << r.toString() << "\n";
            out.close();
            logAccess(path);
        }
    }

    writeFinalSorted(sorted, "output/final_sorted_result_salary.txt");
}

void nonClusteredIndexByName(const vector<Record>& records) {
    createDirectory("output/non_clustered_name_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : records) {
        string path = "output/non_clustered_name_sort/name_" + r.name + ".txt";
        if (written[path].insert(r.toString()).second) {
            ofstream out(path, ios::app);
            out << r.toString() << "\n";
            out.close();
            logAccess(path);
        }
    }

    writeFinalSorted(records, "output/final_sorted_result_name.txt");
}

void nonClusteredIndexById(const vector<Record>& records) {
    createDirectory("output/non_clustered_id_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : records) {
        string path = "output/non_clustered_id_sort/id_" + to_string(r.id) + ".txt";
        if (written[path].insert(r.toString()).second) {
            ofstream out(path, ios::app);
            out << r.toString() << "\n";
            out.close();
            logAccess(path);
        }
    }

    writeFinalSorted(records, "output/final_sorted_result_id.txt");
}

void nonClusteredIndexBySalary(const vector<Record>& records) {
    createDirectory("output/non_clustered_salary_sort");
    map<string, unordered_set<string>> written;

    for (const auto& r : records) {
        string path = "output/non_clustered_salary_sort/salary_" + to_string(r.salary) + ".txt";
        if (written[path].insert(r.toString()).second) {
            ofstream out(path, ios::app);
            out << r.toString() << "\n";
            out.close();
            logAccess(path);
        }
    }

    writeFinalSorted(records, "output/final_sorted_result_salary.txt");
}

int main() {
    string inputFile = "data.txt";
    vector<Record> records = readData(inputFile);

    createDirectory("output");
    ofstream("output/access_log.txt") << "";

    int choice = 0;
    while (true) {
        cout << "Choose clustering or non-clustering constraint:\n";
        cout << "1. Clustered Index by Name\n2. Clustered Index by ID\n3. Clustered Index by Salary\n";
        cout << "4. Non-clustered Index by Name\n5. Non-clustered Index by ID\n6. Non-clustered Index by Salary\n";
        cout << "7. EXIT\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: clusteredIndexByName(records); break;
            case 2: clusteredIndexById(records); break;
            case 3: clusteredIndexBySalary(records); break;
            case 4: nonClusteredIndexByName(records); break;
            case 5: nonClusteredIndexById(records); break;
            case 6: nonClusteredIndexBySalary(records); break;
            case 7: cout << "Exiting program.\n"; return 0;
            default: cout << "Invalid choice! Please select a valid option.\n";
        }
    }

    cout << "Indexing simulation complete! Check the 'output' folder.\n";
    return 0;
}



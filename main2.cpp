#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


using namespace std;

const string FILENAME = "tasks.txt";
bool Work = true;

struct Task {
    string text;
    bool done = false;
};

// Проверка: есть ли хотя бы одна непустая строка в файле
bool fileHasTasks(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (!line.empty()) return true;
    }
    return false;
}

// Добавить задачу в файл (добавляем в конец)
void addTaskToFile(const string& message) {
    ofstream file(FILENAME, ios::app); // открываем файл в режиме ДОПИСИ
    file << 0 << ";" << message << "\n"; // 0 — задача не выполнена
    file.close();
}

// Показать все задачи
void showTasksFromFile() {
    ifstream file(FILENAME);
    if (!file.is_open() || file.peek() == ifstream::traits_type::eof()) {
        cout << "Список задач пуст.\n";
        return;
    }

    cout << "\nСписок задач:\n";
    string line;
    int index = 1;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string done_str, text;
        if (getline(ss, done_str, ';') && getline(ss, text)) {
            bool done = (done_str == "1");
            cout << index++ << ". [" << (done ? 'x' : ' ') << "] " << text << endl;
        }
    }

    file.close();
}

bool markTaskDone(const string& filename, int lineNumber) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Не удалось открыть файл для чтения.\n";
        return false;
    }

    vector<string> lines;
    string line;

    // Считываем все строки
    while (getline(infile, line)) {
        lines.push_back(line);
    }

    infile.close();

    // Проверяем номер строки
    if (lineNumber < 1 || lineNumber > lines.size()) {
        cerr << "Неверный номер строки.\n";
        return false;
    }

    // Получаем строку и меняем только 0 на 1 в начале
    string& targetLine = lines[lineNumber - 1];
    if (targetLine.rfind("0;", 0) == 0) { // если строка начинается с "0;"
        targetLine[0] = '1';
    } else {
        cout << "Задача уже выполнена или формат неверный.\n";
        return false;
    }

    // Перезаписываем файл
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Не удалось открыть файл для записи.\n";
        return false;
    }

    for (const string& l : lines) {
        outfile << l << '\n';
    }

    outfile.close();
    return true;
}


int main() {
    while (Work) {
        cout << "\nВыберите тип действия:\n";
        cout << "0 — выход\n";
        cout << "1 — добавить задачу\n";
        cout << "2 — показать список задач\n";
        cout << "3 — отметить задачу выполненной\n";

        int x;
        cin >> x;

        switch (x) {
            case 0:
                Work = false;
                break;
            case 1: {
                cout << "Введите текст задачи: ";
                string name_task;
                cin.ignore(); // очистка буфера перед getline
                getline(cin, name_task);
                addTaskToFile(name_task);
                break;
            }
            case 2:
                showTasksFromFile();
                break;
            case 3:{
                cout << "Введите номер задачи: ";
                string number_task;
                cin >> number_task;
                int num_task;
                stringstream ss(number_task);
                ss >> num_task;
                markTaskDone(FILENAME, num_task);
            }
                
                break;
            default:
                cout << "Ошибка ввода!" << endl;
                break;
        }
    }
    return 0;
}

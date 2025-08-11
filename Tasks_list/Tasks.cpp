#include <iostream>
#include <vector>
#include <string>
#include <fstream>
// #include <nlohmann/json.hpp>
#include <algorithm>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

const string FILENAME = "tasks.json";

class Task {
private:
    int id;
    string name;
    bool status;

public:
    Task(int id, string name, bool status) 
        : id(id), name(name), status(status) {}

    int getId() const { return id; }
    string getName() const { return name; }
    bool getStatus() const { return status; }
    void setStatus(bool newStatus) { status = newStatus; }

    void displayTask() const {
        cout << "[" << id << "] " << name 
             << " - " << (status ? "Выполнено" : "Не выполнено") << endl;
    }
};

class TaskManager {
private:
    vector<Task> tasks;
    int nextId = 1;

    void autoSave() {
        saveToJson();
    }

public:
    void addTask(const Task& task) {
        tasks.push_back(task);
        autoSave();  // Автосохранение после добавления
    }

    bool removeTask(int id) {
        auto it = find_if(tasks.begin(), tasks.end(), 
            [id](const Task& t) { return t.getId() == id; });
        
        if (it != tasks.end()) {
            tasks.erase(it);
            autoSave();  // Автосохранение после удаления
            return true;
        }
        return false;
    }

    bool markTaskCompleted(int id) {
        auto it = find_if(tasks.begin(), tasks.end(), 
            [id](const Task& t) { return t.getId() == id; });
        
        if (it != tasks.end()) {
            it->setStatus(true);
            autoSave();  // Автосохранение после изменения
            return true;
        }
        return false;
    }

    void displayAllTask() const {
        if (tasks.empty()) {
            cout << "Список задач пуст." << endl;
            return;
        }
        for (const auto& task : tasks) {
            task.displayTask();
        }
    }

    void saveToJson() {
        json j;
        j["tasks"] = json::array();
        j["nextId"] = nextId;
        
        for (const auto& task : tasks) {
            json taskJson;
            taskJson["id"] = task.getId();
            taskJson["name"] = task.getName();
            taskJson["status"] = task.getStatus();
            j["tasks"].push_back(taskJson);
        }

        ofstream file(FILENAME);
        if (file.is_open()) {
            file << j.dump(4);
        } else {
            cerr << "Ошибка сохранения файла!" << endl;
        }
    }

    void loadFromJson() {
        ifstream file(FILENAME);
        if (!file.is_open()) {
            return;
        }

        try {
            json j = json::parse(file);
            nextId = j.value("nextId", 1);
            
            if (j.contains("tasks") && j["tasks"].is_array()) {
                for (const auto& taskJson : j["tasks"]) {
                    tasks.push_back(Task(
                        taskJson["id"],
                        taskJson["name"],
                        taskJson["status"]
                    ));
                }
            }
        } catch (const exception& e) {
            cerr << "Ошибка загрузки: " << e.what() << endl;
        }
    }

    int generateId() {
        return nextId++;
    }
};

void showMenu() {
    cout << "\nВыберите действие\n";
    cout << "1 - Добавить задачу\n";
    cout << "2 - Просмотреть список задач\n";
    cout << "3 - Отметить задачу выполненной\n";
    cout << "4 - Удалить задачу\n";
    cout << "5 - Выход\n";
}

int main() {
    TaskManager Manager;
    Manager.loadFromJson();

    int choice;
    do {
        showMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                string name;
                cout << "Введите название задачи: ";
                getline(cin, name);
                int newId = Manager.generateId();
                Manager.addTask(Task(newId, name, false));
                cout << "Задача добавлена и сохранена." << endl;
                break;
            }  
            case 2:
                Manager.displayAllTask();
                break;
            case 3: {
                int taskId;
                cout << "Введите ID задачи для отметки: ";
                cin >> taskId;
                if (Manager.markTaskCompleted(taskId)) {
                    cout << "Задача отмечена как выполненная." << endl;
                } else {
                    cout << "Задача с таким ID не найдена." << endl;
                }
                break;
            }
            case 4: {
                int taskId;
                cout << "Введите ID задачи для удаления: ";
                cin >> taskId;
                if (Manager.removeTask(taskId)) {
                    cout << "Задача удалена." << endl;
                } else {
                    cout << "Задача с таким ID не найдена." << endl;
                }
                break;
            }
            case 5:
                return 0;
            default:
                cout << "Неверный выбор!" << endl;
        }
    } while (true);

    return 0;
}
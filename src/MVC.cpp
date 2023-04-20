// MVC (Model-View-Controller) - это архитектурный паттерн программирования, который разделяет приложение на три основных компонента (Модель, Представление и Контроллер). 

// Пример паттерна MVC для C++ с использованием только стандартной библиотеки:

// Модель (Model):
// ```
#include <string>
#include <vector>

class Student {
public:
    Student(const std::string& name, const std::string& lastName, int age) :
        mName(name), mLastName(lastName), mAge(age) {}

    std::string getName() const {
        return mName;
    }

    std::string getLastName() const {
        return mLastName;
    }

    int getAge() const {
        return mAge;
    }

private:
    std::string mName;
    std::string mLastName;
    int mAge;
};

class StudentsModel {
public:
    void addStudent(const Student& student) {
        mStudents.push_back(student);
    }

    std::vector<Student> getStudents() const {
        return mStudents;
    }

private:
    std::vector<Student> mStudents;
};
// ```
// Представление (View):
// ```
#include <iostream>
#include <vector>

class StudentsView {
public:
    void showStudents(const std::vector<Student>& students) const {
        for (const auto& student : students) {
            std::cout << "Name: " << student.getName()
                      << ", Last Name: " << student.getLastName()
                      << ", Age: " << student.getAge() << std::endl;
        }
    }
};
// ```
// Контроллер (Controller):
// ```
class StudentsController {
public:
    StudentsController(StudentsModel& model, StudentsView& view) :
        mModel(model), mView(view) {}

    void addStudent(const Student& student) {
        mModel.addStudent(student);
    }

    void showStudents() {
        mView.showStudents(mModel.getStudents());
    }

private:
    StudentsModel& mModel;
    StudentsView& mView;
};
// ```
// Пример использования паттерна:
// ```
int maintoo() {
    StudentsModel model;
    StudentsView view;
    StudentsController controller(model, view);


    controller.addStudent(Student("Ivan", "Ivanov", 20));
    controller.addStudent(Student("Petr", "Petrov", 25));
    controller.showStudents();

    return 0;
}
// ```
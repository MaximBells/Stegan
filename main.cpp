#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Person{
public:
    Person(const string &name, const string &address);

    void setAddress(const string &address);
    virtual void print() const;
    string getAddress();
    string getName();

protected:
    string _name;
    string _address;
};

class Student: public Person{
public:
    Student(const string &name, const string &address, int course, double rating);

    void setRating(double newRating);
    void print() const;

protected:
    int _course;
    double _rating;
};
string Person::getAddress(){
    return _address;
}
string Person::getName() {
    return _name;
}

Person::Person(const string &name, const string &address):
        _name(name), _address(address){
}

void Person::setAddress(const string &address){
    _address = address;
}


void Person::print() const{
    cout << _name << " adress: " << _address;
}

Student::Student(const string &name, const string &address, int course, double rating):
        Person(name, address), _course(course), _rating(rating){
}

void Student::setRating(double newRating){
    _rating = newRating;
}

void Student::print() const{
    Person::print();

    cout << " course: " << _course << " rating: " << _rating;
}

void process(const vector<reference_wrapper<Person>> persons){
    cout << "Inside process:" << endl;

    for(auto p: persons){
        p.get().print();
        cout << endl;
    }
}

struct Node{
    shared_ptr<Person> data;
    shared_ptr<Node> next;
};

void printList(shared_ptr<Node> head){
    for(shared_ptr<Node> current = head->next; current != nullptr; current = current->next){
        current->data->print();
    }
}

string findAddress(shared_ptr<Node>head, string name){
    int n = 0;
    string empty = "";
    for(shared_ptr<Node> current = head->next; current != nullptr; current = current->next){
      if(name==current->data->getName()){
          return current->data->getAddress();
          n++;
      }
    }
    if (n==0)return empty;
}

shared_ptr<Node> createList(){
    return make_shared<Node>();
}

void addElement(shared_ptr<Node> head, shared_ptr<Person> data){
    shared_ptr<Node> newElem = make_shared<Node>();
    newElem->next = head->next;
    newElem->data = data;
    head->next = newElem;
}

int main(){
    shared_ptr<Node> list = createList();

    addElement(list, make_shared<Person>("Name_1", "Address_1"));
    addElement(list, make_shared<Person>("Name_2", "Address_2"));
    cout << endl;
    cout << findAddress(list,"Name_2") << endl;
cout << endl;
    printList(list);

    return 0;
}

/*
Задание.
Реализуйте функцию
std::stringfindAddress(shared_ptr<Node>head, std::stringname),
которая по имени человека возвращает его адрес.
На вход функции подается указатель head на список структур Node
и имя искомого человека. Функция возвращает адрес человека или
пустую строку, если такого человека в списке нет.
*/
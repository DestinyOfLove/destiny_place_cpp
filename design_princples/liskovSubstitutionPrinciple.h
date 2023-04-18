//
// Created by L S on 2023/4/19.
//

#ifndef DESTINY_PLACE_CPP_LISKOVSUBSTITUTIONPRINCIPLE_H
#define DESTINY_PLACE_CPP_LISKOVSUBSTITUTIONPRINCIPLE_H

#include <iostream>

/*
 * 里氏替换原则的样例。写一个客户端界面的样例，客户端界面有一个显示方法，可以显示一个矩形或者圆形
 */

class Shape {
public:
    virtual void draw() = 0;
};

class Rectangle : public Shape {
public:
    void draw() override {
        std::cout << "draw a rectangle" << std::endl;
    }
};

class Circle : public Shape {
public:
    void draw() override {
        std::cout << "draw a circle" << std::endl;
    }
};

class Client {
public:
    static void display(Shape *shape) {
        shape->draw();
    }
};

class LSPTest {
public:
    static void run();
};


#endif //DESTINY_PLACE_CPP_LISKOVSUBSTITUTIONPRINCIPLE_H

//
// Created by L S on 2023/4/22.
//

#ifndef DESTINY_PLACE_CPP_BAD_H
#define DESTINY_PLACE_CPP_BAD_H

#include <iostream>

// 演员动作类，定义了演员的动作
class ActorAction {
public:
    virtual ~ActorAction() {}

    virtual void perform() = 0;
};

// 跳舞动作类，继承自演员动作类
class DanceAction : public ActorAction {
public:
    virtual void perform() override {
        std::cout << "Dancing...\n";
    }
};

// 唱歌动作类，继承自演员动作类
class SingAction : public ActorAction {
public:
    virtual void perform() override {
        std::cout << "Singing...\n";
    }
};

// 演员类，包含了演员的一些基本信息和动作
class Actor {
public:
    Actor(const std::string &name, ActorAction *action) : name_(name), action_(action) {}

    void setAction(ActorAction *action) {
        action_ = action;
    }

    void performAction() {
        std::cout << name_ << " performs the following action:\n";
        action_->perform();
    }

private:
    std::string name_;
    ActorAction *action_;
};


#endif //DESTINY_PLACE_CPP_BAD_H

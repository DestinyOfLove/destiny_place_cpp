//
// Created by L S on 2023/4/22.
//

#ifndef DESTINY_PLACE_CPP_GOOD_H
#define DESTINY_PLACE_CPP_GOOD_H

#include <iostream>

// 演员动作类，定义了一个接口，用于描述演员的动作
class IActorAction {
public:
    virtual ~IActorAction() {}

    virtual void Play() = 0;
};

// 演员类，包含了演员的一些基本信息和动作
class Actor {
public:
    Actor(const std::string &name, std::shared_ptr<IActorAction> action)
            : name_(name), action_(action) {}

    void SetAction(std::shared_ptr<IActorAction> action) {
        action_ = action;
    }

    void PerformAction() {
        std::cout << name_ << " performs the following action:\n";
        action_->Play();
    }

private:
    std::string name_;
    std::shared_ptr<IActorAction> action_;
};

// 演员的跳舞动作类，实现了演员动作接口
class DanceAction : public IActorAction {
public:
    virtual void Play() override {
        std::cout << "Dancing...\n";
    }
};

// 演员的唱歌动作类，实现了演员动作接口
class SingAction : public IActorAction {
public:
    virtual void Play() override {
        std::cout << "Singing...\n";
    }
};


#endif //DESTINY_PLACE_CPP_GOOD_H

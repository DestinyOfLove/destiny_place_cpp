//
// Created by L S on 2023/4/19.
//

#include "liskovSubstitutionPrinciple.h"

void LSPTest::run() {
    Shape *shape1 = new Rectangle();
    Shape *shape2 = new Circle();
    Client::display(shape1);
    Client::display(shape2);
}
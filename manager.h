#ifndef MANAGER_H
#define MANAGER_H

class Manager
{
public:
    Manager();
    virtual ~Manager();
    virtual void updateData();
    virtual void init();
};

#endif // MANAGER_H

#ifndef DRIVINGSIMULATOR_GAMESTATE_H
#define DRIVINGSIMULATOR_GAMESTATE_H

class Application;

class GameState {
public:
    explicit GameState(Application &application)
            : application(application) {}

    virtual void imgui() = 0;

    virtual void draw() = 0;

    virtual void update(double dt) = 0;

protected:
    Application& application;
};


#endif //DRIVINGSIMULATOR_GAMESTATE_H

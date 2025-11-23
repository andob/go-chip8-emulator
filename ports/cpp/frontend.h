#ifndef FRONTEND_H
#define FRONTEND_H

#include <memory>
#include <SDL_render.h>
#include "system.h"

using namespace std;

class Frontend
{
    unique_ptr<System>& system;

    void render_frame(SDL_Renderer* renderer) const;

    struct EventHandlerResult { bool should_exit_frontend; };
    EventHandlerResult handle_events() const;

public:
    explicit Frontend(unique_ptr<System>& system) : system(system) {}

    void run() const;
};

#endif

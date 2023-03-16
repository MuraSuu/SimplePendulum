#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_matrix.h>
#include "SDL_Manager.hh"

void CapFramerate(long& then, float& remainder)
{
    long wait, frameTime;
    wait = 16 + remainder;
    remainder -= (int)remainder;
    frameTime = SDL_GetTicks() - then;
    wait -= frameTime;

    if (wait < 1)
    {
        wait = 1;
    }

    SDL_Delay(wait);
    remainder += 0.667f;
    then = SDL_GetTicks();
}

//Parameters: g,l,m
//for the purposes of this program, the length l must
//stay somewhere in 0 < l < 10.
int Func(double t, const double y[], double dydt[], void* param)
{
    double* temp = static_cast<double*>(param);
    double gravity = temp[0], length = temp[1], mass = temp[2];
    
    dydt[0] = y[1];
    dydt[1] = -(gravity/length)*gsl_sf_sin(y[0]);
    
    return GSL_SUCCESS;
}

int Jac(double t, const double y[], double* dfdy, double dfdt[], void* param)
{
    double* temp = static_cast<double*>(param);
    double gravity = temp[0], length = temp[1], mass = temp[2];
    
    gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2,2);
    gsl_matrix* m = &dfdy_mat.matrix;
    
    gsl_matrix_set(m, 0, 0, 0.0);
    gsl_matrix_set(m, 0, 1, 1.0);
    gsl_matrix_set(m, 1, 0, -(gravity/length)*gsl_sf_cos(y[0]));
    gsl_matrix_set(m, 1, 1, 0.0);
    
    return GSL_SUCCESS;
}

int main()
{
    //GSL initialization.
    double param[] = {2, 1.5, 0.05}; //Parameters: g,l,m.
    gsl_odeiv2_system sys = {Func, Jac, 2, &param};
    gsl_odeiv2_driver* driver = gsl_odeiv2_driver_alloc_y_new(&sys, gsl_odeiv2_step_rkf45, 1e-6, 1e-6, 0.0);
    
    //SDL initialization.
    SDL_Manager manager("Simple pendulum", 800, 600);
    bool quit = false;
    long then = SDL_GetTicks();
    float remainder = 0.0f;
    
    double t = 0.0, ti = 0.0;
    double y[2] = {3.14159/6, 0.0}; //y[0] theta, y[1] theta dot. Almost 45 degrees.
    
    int x0 = 400, y0 = 200;
    //This 60 here is a normalization constant for 0 < l <= 10.
    //if l = 10 then the rod will touch the bottom of the screen.
    int L = 60 * param[1];
    SDL_Rect rect {0, 0, 25, 25};
    
    while(quit == false)
    {
        //Perhaps this is harmful for the program. 
        for(SDL_Event event; SDL_PollEvent(&event); )
        {
            if(event.type == SDL_QUIT)
            {
                quit = true;
            }
            else{
                break;
            }
        }
        
        if(quit == true) break;
        ti += 0.05; 
        
        //Do the computations and check for possible errors.
        int status = gsl_odeiv2_driver_apply(driver, &t, ti, y);
        if(status != GSL_SUCCESS)
        {
            std::fprintf(stderr, "Error with return: %d\n", status);
            quit = true;
            break;
        }
        SDL_SetRenderDrawColor(manager.render, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(manager.render);
        /*
            The pendulum will be a line hanging from a fixed point to another
            point that will move using the theta(radians) that was computed using the odeiv2.
            Instead of a ball I will use a rectangle, because there is already too many
            computations happening here.
        */
        
        int dx = static_cast<int>(x0+L*gsl_sf_sin(y[0]));
        int dy = static_cast<int>(y0+L*gsl_sf_cos(y[0]));
        rect.x = dx - rect.w/2;
        rect.y = dy - rect.h/2;
        
        SDL_SetRenderDrawColor(manager.render, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawLine(manager.render, x0, y0, dx, dy);
        SDL_SetRenderDrawColor(manager.render, 0xFF,0x0,0x0,0xFF);
        SDL_RenderDrawRect(manager.render, &rect);
        SDL_RenderPresent(manager.render);
        CapFramerate(then, remainder);
    }
    gsl_odeiv2_driver_free(driver);//Release memory.
}
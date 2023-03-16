#include <cstdio>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <cmath>

//Parameters: g,l,m
//TODO: Scale this program so it can iterate through n different initial
//      conditions.
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
    
    gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
    gsl_matrix* m = &dfdy_mat.matrix;
    
    gsl_matrix_set(m, 0, 0, 0.0);
    gsl_matrix_set(m, 0, 1, 1.0);
    gsl_matrix_set(m, 1, 0, -(gravity/length)*gsl_sf_cos(y[0]));
    gsl_matrix_set(m, 1, 1, 0.0);
    
    //dfdt[0] = 0.0;
    //dfdt[1] = 0.0;
    
    return GSL_SUCCESS;
}

int main()
{
    double temp[3] = {2, 1.5, 0.05}; //Parameters: g,l,m
    gsl_odeiv2_system system{Func, Jac, 2, temp};
    gsl_odeiv2_driver* driver = gsl_odeiv2_driver_alloc_y_new(&system, gsl_odeiv2_step_rk8pd, 1e-6, 1e-6, 0.0);
    
    double t = 0.0, t1 = 100.0;
    double theta_dot = 2*std::sqrt(temp[0]/temp[1]);
    double y[2] = {0.0, theta_dot/5.0};
    
    for(int k = 0; k < 10; ++k)
    {
        for(int i = 1; i <= 100; ++i) //Run 100 interations.
        {
            double ti = i*t1/100.0;
            int status = gsl_odeiv2_driver_apply(driver, &t, ti, y);
            if(status != GSL_SUCCESS)
            {
                std::fprintf(stderr, "Error with return: %d\n", status);
                break;
            }
            /**For later: Figure out a better way to pipe this data to python, so
               it can draw a graph.**/
            std::printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
        }
        std::printf("End\n");
        y[1] += theta_dot/5.0; //Get the next angular_velocity in controlled steps.
        t = 0;
    }
    gsl_odeiv2_driver_free(driver); //Release memory.   
}
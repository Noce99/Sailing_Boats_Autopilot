#include <iostream>
#include <chrono>
#include <thread> //std::this_thread::sleep_for(std::chrono::milliseconds(50));
#include <string.h>
#include <vector>
#include <cmath>
#include <matplot/matplot.h>
#include <random>

using namespace matplot;

#define zone_of_interest 2

#define islands_x_y_size_mean 10
#define islands_x_y_size_variance 5
#define islands_x_y_height_mean 5
#define islands_x_y_height_variance 2

#define wind_x_y_size_mean 150
#define wind_x_y_size_variance 35
#define wind_x_y_height_mean 80
#define wind_x_y_height_variance 10

class paraboloid{
    /*
    Full equation:
    x^2(a*cos(theta)^2+b*sin(theta)^2)+y^2(a*sin(theta)^2+b*cos(theta)^2)+2xy*(b-a)*cos(theta)*sin(theta)+c=z
    Short one:
    x^2*A+y^2*B+xy*C+D=z
    */
    double A, B, C, D, pos_x, pos_y;
    double min_x, max_x, min_y, max_y;
    public:
    paraboloid(double x, double y, double x_size, double y_size, double theta, double height){
        double a = -height/(4*x_size*x_size);
        double b = -height/(4*y_size*y_size);
        A = (a*cos(theta)*cos(theta)+b*sin(theta)*sin(theta));
        B = (a*sin(theta)*sin(theta)+b*cos(theta)*cos(theta));
        C = 2*(b-a)*cos(theta)*sin(theta);
        D = height;
        pos_x = x;
        pos_y = y;
        min_x = zone_of_interest*(x-x_size);
        max_x = zone_of_interest*(x+x_size);
        min_y = zone_of_interest*(y-y_size);
        max_y = zone_of_interest*(y+y_size);
    }

    double at(double x, double y){
        return (x-pos_x)*(x-pos_x)*A+(y-pos_y)*(y-pos_y)*B+(x-pos_x)*(y-pos_y)*C+D;
    }

    std::vector<double> gradient_at(double x, double y){
        return std::vector<double>{2*A*(x-pos_x)+C*(y-pos_y), 2*B*(y-pos_y)+C*(x-pos_x)};
    }

    std::vector<double> perpendicular_gradient_at(double x, double y){
        return std::vector<double>{-2*B*(y-pos_y)+C*(x-pos_x), 2*A*(x-pos_x)+C*(y-pos_y)};
    }

    void plot(int point_along_x){
        double resolution = (max_x-min_x)/(1.0*point_along_x);
        double min, max;
        if (min_x<=min_y){
            min = min_x;
        }else{
            min = min_y;
        }
        if (max_x>=max_y){
            max = max_x;
        }else{
            max = max_y;
        }
        auto [X, Y] = meshgrid(iota(min, resolution, max));
        auto Z = transform(X, Y, [this](double x, double y){return at(x, y);});
        mesh(X, Y, Z);
        auto Z_plane = transform(X, Y, [](double x, double y){return 0;});
        hold(on);
        mesh(X, Y, Z_plane)->edge_color({0, 0, 0, 1});
        title("Prima Isoletta!");
        show();
    }
};

class  paraboloid_accumulator{
    double x_map_size, y_map_size;
    std::vector<paraboloid> all_paraboloid;
    int paraboloid_num;

    public:
    paraboloid_accumulator(double my_x_map_size, double my_y_map_size, int my_paraboloid_num, double x_y_size_mean, double x_y_size_variance, double x_y_height_mean, double x_y_height_variance){
        x_map_size = my_x_map_size;
        y_map_size = my_y_map_size;
        paraboloid_num = my_paraboloid_num;
        double x, y, x_size, y_size, theta, height;
        std::default_random_engine generator;
        std::normal_distribution<double> xy_size_gauss(x_y_size_mean, x_y_size_variance);
        std::normal_distribution<double> xy_height_gauss(x_y_height_mean, x_y_height_variance);
        for (int i=0; i<paraboloid_num; i++){
            x = (rand()*1.0/RAND_MAX)*x_map_size - x_map_size/2;
            y = (rand()*1.0/RAND_MAX)*y_map_size - y_map_size/2;
            x_size = xy_size_gauss(generator);
            if (x_size<1){
                x_size = 1;
            }
            y_size = xy_size_gauss(generator);
            if (y_size<1){
                y_size = 1;
            }
            theta = (rand()*1.0/RAND_MAX)*2*M_PI;
            height = xy_height_gauss(generator);
            all_paraboloid.push_back(paraboloid(x, y, x_size, y_size, theta, height));
            // std::cout << "Created a paroboloid in [" << x << "; " << y << "] of size [" << x_size << "; " << y_size << "] of theta " << theta << " rad and height " << height << std::endl;
        }
    }

    double at(double x, double y){
        double result = 0;
        double tmp;
        for (int i=0; i<paraboloid_num; i++){
            tmp = all_paraboloid[i].at(x, y);
            if (tmp>0){
                result += tmp;
            }
        }
        return result;
    }
    std::vector<double> normalized_gradient_at(double x, double y){
        std::vector<double> result = {0, 0};
        std::vector<double> tmp;
        double norm;
        for (int i=0; i<paraboloid_num; i++){
            if (all_paraboloid[i].at(x, y) > 0){
                tmp = all_paraboloid[i].gradient_at(x, y);
                norm = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]);
                if (norm>0){
                    result[0] += tmp[0]/norm;
                    result[1] += tmp[1]/norm;
                }
            }
        }
        return result;
    }

    std::vector<double> normalized_perpendicular_gradient_at(double x, double y){
        std::vector<double> result = {0, 0};
        std::vector<double> tmp;
        double norm;
        for (int i=0; i<paraboloid_num; i++){
            if (all_paraboloid[i].at(x, y) > 0){
                tmp = all_paraboloid[i].perpendicular_gradient_at(x, y);
                norm = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]);
                if (norm>0){
                    result[0] += tmp[0]/norm;
                    result[1] += tmp[1]/norm;
                }
            }
        }
        return result;
    }

    void plot(int point_along_x){
        double resolution = x_map_size/(1.0*point_along_x);
        double min, max;
        if (x_map_size>=y_map_size){
            min = -x_map_size/2;
            max = x_map_size/2;
        }else{
            min = -y_map_size/2;
            max = y_map_size/2;
        }
        auto [X, Y] = meshgrid(iota(min, resolution, max));
        auto Z = transform(X, Y, [this](double x, double y){return at(x, y);});
        auto my_meshc = meshc(X, Y, Z);
        my_meshc->contour_levels(40);
        title("Some Paraboloids");
        zlim({xlim()[0], xlim()[1]});

        show();
    }

    void plot_gradient(int point_along_x){
        double resolution = x_map_size/(1.0*point_along_x);
        double min, max;
        if (x_map_size>=y_map_size){
            min = -x_map_size/2;
            max = x_map_size/2;
        }else{
            min = -y_map_size/2;
            max = y_map_size/2;
        }
        auto [X_c, Y_c] = meshgrid(iota(min, resolution/10, max));
        auto Z = transform(X_c, Y_c, [this](double x, double y){return at(x, y)-250;});
        auto cont = contour(X_c, Y_c, Z);
        cont->n_levels(60);

        hold(on);

        auto [X, Y] = meshgrid(iota(min, resolution, max));
        vector_2d u = transform(X, Y, [this](double x, double y) { return normalized_gradient_at(x, y)[0]; });
        vector_2d v = transform(X, Y, [this](double x, double y) { return normalized_gradient_at(x, y)[1]; });

        title("Some Paraboloids Gradients");
        quiver(X, Y, u, v, 0.5);

        hold(on);

        u = transform(X, Y, [this](double x, double y) { return normalized_perpendicular_gradient_at(x, y)[0]; });
        v = transform(X, Y, [this](double x, double y) { return normalized_perpendicular_gradient_at(x, y)[1]; });

        title("Some Paraboloids Gradients");
        quiver(X, Y, u, v, 0.5);
        show();
    }

    void plot_both(int point_along_x){
        double resolution = x_map_size/(1.0*point_along_x);
        double min, max;
        if (x_map_size>=y_map_size){
            min = -x_map_size/2;
            max = x_map_size/2;
        }else{
            min = -y_map_size/2;
            max = y_map_size/2;
        }
        auto [X, Y] = meshgrid(iota(min, resolution, max));
        auto Z = transform(X, Y, [this](double x, double y){return at(x, y)-250;});
        auto my_meshc = meshc(X, Y, Z);
        my_meshc->contour_levels(40);
        title("Some Paraboloids");
        zlim({xlim()[0], xlim()[1]});

        hold(on);

        auto [X_2, Y_2] = meshgrid(iota(min, resolution*5, max));
        vector_2d u = transform(X_2, Y_2, [this](double x, double y) { return normalized_gradient_at(x, y)[0]; });
        vector_2d v = transform(X_2, Y_2, [this](double x, double y) { return normalized_gradient_at(x, y)[1]; });

        title("Some Paraboloids Gradients");
        quiver(X_2, Y_2, u, v, 0.5);

        show();
    }
};

int main(){
    srand (time(NULL));
    paraboloid_accumulator arcipelago = paraboloid_accumulator(500, 500, 30, islands_x_y_size_mean, islands_x_y_size_variance, islands_x_y_height_mean, islands_x_y_height_variance);
    paraboloid_accumulator wind = paraboloid_accumulator(500, 500, 4, wind_x_y_size_mean, wind_x_y_size_variance, wind_x_y_height_mean, wind_x_y_height_variance);
    wind.plot_gradient(30);
}


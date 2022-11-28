#include <iostream>
#include <math.h>
#include <Eigen/Dense> // https://eigen.tuxfamily.org/dox/group__QuickRefPage.html
#include "matplotlibcpp.h"
#include "SBA_socket.cpp"

namespace plt = matplotlibcpp;
using Eigen::Matrix2d;
using Eigen::Matrix4d;
using Eigen::Vector2d;
using Eigen::Vector4d;

class Current_Model {
	private:
		Vector2d v_c;
	public:
		Current_Model(Vector2d initial_v_c = Vector2d(0, 0)){
			v_c = initial_v_c;
		}
		Vector2d get_v_c(){
			return v_c;
		}
};

class Wind_Model {
	private:
		Vector2d v_w;
	public:
		Wind_Model(Vector2d initial_v_w = Vector2d(0, 0)){
			v_w = initial_v_w;
		}
		Vector2d get_v_w(){
			return v_w;
		}
};

class Sailing_Boat_Model {
	private:
		Vector4d eta; // 		x, 		y, 		phi, 	xi
		Vector4d nu; // 		u, 		v, 		p, 		r
		Vector4d nu_r(){ // 	u_r, 	v_r, 	p_r, 	r_r
			Vector2d nu_r_small = Vector2d(nu(0), nu(1)) - J_2D()*CM.get_v_c();
			return  Vector4d(nu_r_small(0), nu_r_small(1), nu(2), nu(3));
		}
		double rho_water = 1025;
		double rho_air = 1.23;
		double l_1 = -1.82;
		double l_2 = 1.35;
		double l_3 = -0.66;
		double l_4 = 3.7;
		double h_1 = 5.2;
		double h_2 = 0.95;
		double h_3 = 0.7;
		double A_s = 22.8;
		double A_k = 0.93;
		double A_r = 0.3;
		double t_max = 0.2;
		double m = 1600;
	  	double I_xx = 6800;
	  	double I_zz = 8500;
	  	double x_u_dot = -160;
	  	double y_v_dot = -1200;
	  	double k_p_dot = -1000;
	  	double n_r_dot = -2400;
	  	Matrix4d J(){
	  		return Matrix4d {	{cos(eta(3)),	-sin(eta(3))*cos(eta(2)), 	0, 	0},
								{sin(eta(3)),	cos(eta(3))*cos(eta(2)), 	0, 	0},
								{0,		0, 							1, 	0},
								{0,		0, 							0, 	cos(eta(2))}};
	  	}
	  	Matrix2d J_2D(){
	  		return Matrix2d {	{cos(eta(3)),			sin(eta(3))},
								{-sin(eta(3))*cos(eta(2)),	cos(eta(3))*cos(eta(2))}};
	  	}
		Matrix4d M_RB{	{m, 			0, 			0, 		0},
						{0, 			m, 	0, 		0},
						{0, 			0, 	I_xx, 	0},
						{0, 			0, 	0, 		I_zz}};
		Matrix4d M_A{	{x_u_dot, 		0, 			0, 				0},
						{0, 			y_v_dot, 	0, 				0},
						{0, 			0, 	k_p_dot, 		0},
						{0, 			0, 	0, 				n_r_dot}};
		Matrix4d C_RB(){
			return Matrix4d {	{0, 		-m*nu(3), 	0,	0},
								{m*nu(3), 	0, 			0,	0},
								{0, 		0, 			0,	0},
								{0, 		0, 			0,	0}};
		}
		Matrix4d C_A(){
			Vector4d my_nu_r = nu_r();
			return Matrix4d {	{0, 				0, 					0,		y_v_dot*my_nu_r(1)},
								{0, 		0, 					0,		-x_u_dot*my_nu_r(0)},
								{0, 		0, 					0,		0},
								{-y_v_dot*my_nu_r(1), 	x_u_dot*my_nu_r(0), 0,		0}};
		}
		Matrix4d D_l {	{10,	0,		0,	0},
						{0,		16,		0,	0},
						{0,		0,		0,	0},
						{0,		0,		0,	40}};
		Vector4d D(){
			Vector4d my_nu_r = nu_r();
			return D_l * my_nu_r;
		}
		double C_Ls(double alpha){
		    double sign = alpha/abs(alpha);
			alpha = abs(alpha*180/M_PI);
			if (alpha>=0 && alpha<=100){
				return sign*(-1.877662 + 0.265759*alpha - 0.006852092*alpha*alpha + 0.00006764791*alpha*alpha*alpha - 2.366522e-7*alpha*alpha*alpha*alpha);
			}else{
				std::cout << "Request of calculation of C_Ls for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		double C_Fs(double R_n){
			return 0.075/((log10(R_n)-2)*(log10(R_n)-2));
		}
		double C_Ds(double alpha, double R_n){
			alpha = abs(alpha*180/M_PI);
			if (alpha>=0 && alpha<=100){
				double C_Ds0 = 0.1638985 + 0.00133082*alpha + 0.00001135329*alpha*alpha + 0.000001185599*alpha*alpha*alpha;
				return C_Ds0 + 2*(1+2*(t_max/sqrt(A_s)))*C_Fs(R_n);
			}else{
				std::cout << "Request of calculation of C_Ds for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		Vector4d S(double lambda){
		    std::cout << "Xi: " << eta(3) << std::endl;
		    std::cout << "----------------" << std::endl;
		    std::cout << J_2D() << std::endl;
		    std::cout << "----------------" << std::endl;
		    std::cout << "WM.get_v_w(): " << WM.get_v_w() << std::endl;
		    std::cout << "J_2D()*WM.get_v_w(): " << J_2D()*WM.get_v_w() << std::endl;
		    Vector2d V_ws = J_2D()*WM.get_v_w() - Vector2d(nu(0), nu(1)) - Vector2d(nu(3)*l_2*sin(lambda), -nu(3)*(l_1 + l_2*cos(lambda)+nu(2)*h_1));
		    double beta_ws = atan2(V_ws(1), V_ws(0));
		    std::cout << "Beta_ws: " << beta_ws << std::endl;
		    double alpha_s = beta_ws - lambda + M_PI;
		    double R_n_air = sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_s)/1.5111e-5;
            //double S_L = 0.5*rho_air*A_s*C_Ls(alpha_s)*V_ws.norm();
            //double S_D = 0.5*rho_air*A_s*C_Ds(alpha_s, R_n_air)*V_ws.norm();
            double S_L = 0.5*rho_air*A_s*general_C_L(alpha_s)*V_ws.norm();
            double S_D = 0.5*rho_air*A_s*general_C_D(alpha_s)*V_ws.norm();
            double S_x = -S_L*sin(beta_ws)+S_D*cos(beta_ws);
			double S_y = S_L*cos(beta_ws)+S_D*sin(beta_ws);
			double S_x_p = S_x*cos(eta(3))+S_y*sin(eta(3));
			double S_y_p = -S_x*sin(eta(3))+S_y*cos(eta(3));
			return Vector4d(S_x, S_y, h_1*S_y, S_x_p*l_2*sin(lambda)-S_y_p*(l_1+l_2*cos(lambda)));
            //return Vector4d(S_L, S_D, h_1*S_y, S_x_p*l_2*sin(lambda)-S_y_p*(l_1+l_2*cos(lambda)));
		}
		double C_Lk(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=-10 && alpha<=10){
				return 0.1400141*alpha - 0.000677465*alpha*alpha*alpha;
			}else{
				std::cout << "Request of calculation of C_Lk for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		double C_Dk(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=-10 && alpha<=10){
				return 0.01 - 0.00003333333*alpha*alpha + 0.000009333333*alpha*alpha*alpha*alpha;
			}else{
				std::cout << "Request of calculation of C_Dk for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		Vector4d K(){
		    Vector4d my_nu_r = nu_r();
            Vector2d V_ck = - Vector2d(my_nu_r(0), my_nu_r(1)) - Vector2d(0, -nu(3)*l_3-nu(2)*h_2);
            double beta_ck = atan2(V_ck(1), V_ck(0));
            double alpha_k = -beta_ck+M_PI;
            double R_n_k = sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_k)/9.7937e-7;
            //double K_L = 0.5*rho_water*A_k*C_Lk(alpha_k)*V_ck.norm();
            //double K_D = 0.5*rho_water*A_k*C_Dk(alpha_k)*V_ck.norm();
            double K_L = 0.5*rho_water*A_k*general_C_L(alpha_k)*V_ck.norm();
            double K_D = 0.5*rho_water*A_k*general_C_D(alpha_k)*V_ck.norm();
            double K_x = K_L*sin(beta_ck)+K_D*cos(beta_ck);
			double K_y = -K_L*cos(beta_ck)+K_D*sin(beta_ck);
			return Vector4d(K_x, K_y, -h_2*K_y, -l_3*K_y);
		}
		double C_Lr(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=-15 && alpha<=15){
				return 0.1419403*alpha - 0.0003181921*alpha*alpha*alpha;
			}else{
				std::cout << "Request of calculation of C_Lr for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		double C_Dr(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=-15 && alpha<=15){
				return 0.017 - 0.00006111111*alpha*alpha + 0.000001911111*alpha*alpha*alpha*alpha;
			}else{
				std::cout << "Request of calculation of C_Dr for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		Vector4d R(double sigma){
		    Vector4d my_nu_r = nu_r();
		    Vector2d V_cr = - Vector2d(my_nu_r(0), my_nu_r(1)) - Vector2d(0, -nu(3)*l_4-nu(2)*h_3);
		    double beta_cr = atan2(V_cr(1), V_cr(0));
		    double alpha_r = -beta_cr+sigma+M_PI;
		    double R_n_r = sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_r)/9.7937e-7;
		    //double R_L = 0.5*rho_water*A_r*C_Lr(alpha_r)*V_cr.norm();
		    //double R_D = 0.5*rho_water*A_r*C_Dr(alpha_r)*V_cr.norm();
		    double R_L = 0.5*rho_water*A_r*general_C_L(alpha_r)*V_cr.norm();
		    double R_D = 0.5*rho_water*A_r*general_C_D(alpha_r)*V_cr.norm();
		    double R_x = R_L*sin(beta_cr)+R_D*cos(beta_cr);
			double R_y = -R_L*cos(beta_cr)+R_D*sin(beta_cr);
			return Vector4d(R_x, R_y, -h_3*R_y, -l_4*R_y);
			//return Vector4d(R_L, R_D, R_D, R_D);
		}
		double general_C_L(double alpha){
            // y = -0.01702952 + 0.07040326*x - 0.001261044*x^2 + 0.000007347746*x^3 - 2.40808e-8*x^4 + 5.627903e-11*x^5
            alpha = alpha*180/M_PI;
            while (alpha > 180){
                alpha = alpha - 360;
            }
            while (alpha < -180){
                alpha = alpha + 360;
            }
            int sign = int(alpha/abs(alpha));
            alpha = abs(alpha);
            double cl = -0.01702952 + 0.07040326*alpha - 0.001261044*alpha*alpha + 0.000007347746*alpha*alpha*alpha - 2.40808e-8*alpha*alpha*alpha*alpha + 5.627903e-11*alpha*alpha*alpha*alpha*alpha;
            if (sign < 0){
                return -cl;
            }else{
                return cl;
            }
		}
		double general_C_D(double alpha){
            alpha = alpha*180/M_PI;
            while (alpha > 180){
                alpha = alpha - 360;
            }
            while (alpha < -180){
                alpha = alpha + 360;
            }
            alpha = abs(alpha);
            return -0.004239731*alpha + 0.0009123078*alpha*alpha - 0.00001340114*alpha*alpha*alpha + 7.230022e-8*alpha*alpha*alpha*alpha - 1.377991e-10*alpha*alpha*alpha*alpha*alpha;
		}
		Current_Model CM;
		Wind_Model WM;
  	public:
	  	Sailing_Boat_Model(	Vector4d initial_eta = Vector4d {0, 0, 0, M_PI/4},
	  						Vector4d initial_nu = Vector4d {0, 0, 0, 0},
	  						Current_Model given_CM = Current_Model(Vector2d(-1, 0)),
	  						Wind_Model given_WM = Wind_Model(Vector2d(-1, 0))){
	  		eta = initial_eta; // A deep copy is done: I have checked
	  		nu = initial_nu; // A deep copy is done: I have checked
	  		CM = given_CM;
	  		WM = given_WM;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << S(0) << std::endl;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << K() << std::endl;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << R(0) << std::endl;
	  		std::cout << "--------------------" << std::endl;
            int n = 360;
            std::vector<double> x(n), ls(n), ds(n), lk(n), dk(n), lr(n), dr(n), standard_lc(n), standard_dc(n), Sx(n), Sy(n), Rx(n), Ry(n);
            double R_n_air = sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_s)/1.5111e-5;
            Vector4d s, r;
            for(int i=-180; i<180; ++i) {
                x.at(i+180) = i;
                /*ls.at(i+180) = C_Ls(i*M_PI/180);
                ds.at(i+180) = C_Ds(i*M_PI/180, R_n_air);
                lk.at(i+180) = C_Lk(i*M_PI/180);
                dk.at(i+180) = C_Dk(i*M_PI/180);
                lr.at(i+180) = C_Lr(i*M_PI/180);
                dr.at(i+180) = C_Dr(i*M_PI/180);*/
                standard_lc.at(i+180) = general_C_L(i*M_PI/180);
                standard_dc.at(i+180) = general_C_D(i*M_PI/180);
                s = S(i*M_PI/180);
                r = R(i*M_PI/180);
                Sx.at(i+180) = s(0);
                Sy.at(i+180) = s(1);
                Rx.at(i+180) = r(0);
                Ry.at(i+180) = r(1);
            }

            // Set the size of output image to 1200x780 pixels
            // plt::figure_size(1200, 780);
            // Plot line from given x and y data. Color is selected automatically.
            /*plt::plot(x, ls);
            plt::plot(x, ds);
            plt::save("./LD_s.png");
            plt::figure();
            plt::plot(x, lk);
            plt::plot(x, dk);
            plt::save("./LD_k.png");
            plt::figure();
            plt::plot(x, lr);
            plt::plot(x, dr);
            plt::save("./LD_r.png");
            plt::figure();*/
            plt::plot(x, standard_lc);
            plt::plot(x, standard_dc);
            plt::save("./LC_LD_standard.png");
            plt::figure();
            plt::plot(x, Sx, {{"label", "S_x"}});
            plt::plot(x, Sy, {{"label", "S_y"}});
            plt::grid(true);
            plt::legend();
            plt::save("./S_vs_Lambda.png");
            plt::figure();
            plt::plot(x, Rx, {{"label", "R_x"}});
            plt::plot(x, Ry, {{"label", "R_y"}});
            plt::grid(true);
            plt::legend();
            plt::save("./R_vs_Sigma.png");

            plt::show();
	  	}

		Vector4d get_eta(){
			return eta;
		}

		Vector4d get_nu(){
			return nu;
		}
};

int main(){
    MySocketServer MSS = MySocketServer();
    int c = 0;
    while(true){
        MSS.send_data(Vector4d(c, c, c, M_PI/4));
        c++;
    }
  	Sailing_Boat_Model SBM = Sailing_Boat_Model();
}


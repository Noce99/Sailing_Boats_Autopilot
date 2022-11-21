#include <iostream>
#include <math.h>
#include <Eigen/Dense> // https://eigen.tuxfamily.org/dox/group__QuickRefPage.html
#include "matplotlibcpp.h"

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
		double lambda = 0;
		double sigma = 0;
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
								{0,				0, 							1, 	0},
								{0,				0, 							0, 	cos(eta(2))}};
	  	}
	  	Matrix2d J_2D(){
	  		return Matrix2d {	{cos(eta(3)),				sin(eta(3))},
								{-sin(eta(3))*cos(eta(2)),	cos(eta(3))*cos(eta(2))}};
	  	}
		Matrix4d M_RB{	{m, 			0, 			0, 		0},
						{0, 			m, 			0, 		0},
						{0, 			0, 			I_xx, 	0},
						{0, 			0, 			0, 		I_zz}};
		Matrix4d M_A{	{x_u_dot, 		0, 			0, 				0},
						{0, 			y_v_dot, 	0, 				0},
						{0, 			0, 			k_p_dot, 		0},
						{0, 			0, 			0, 				n_r_dot}};
		Matrix4d C_RB(){
			return Matrix4d {	{0, 		-m*nu(3), 	0,	0},
								{m*nu(3), 	0, 			0,	0},
								{0, 		0, 			0,	0},
								{0, 		0, 			0,	0}};
		}
		Matrix4d C_A(){
			Vector4d my_nu_r = nu_r();
			return Matrix4d {	{0, 					0, 					0,		y_v_dot*my_nu_r(1)},
								{0, 					0, 					0,		-x_u_dot*my_nu_r(0)},
								{0, 					0, 					0,		0},
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
		// All I need for calculating S
		Vector2d V_ws(){
			return J_2D()*WM.get_v_w() - Vector2d(nu(0), nu(1)) - Vector2d(nu(3)*l_2*sin(lambda), -nu(3)*(l_1 + l_2*cos(lambda)+nu(2)*h_1)); // miss a thermi in lambda dot
		}
		double beta_ws(){
			Vector2d my_V_ws = V_ws();
			return atan2(my_V_ws(1), my_V_ws(0));
		}
		double alpha_s(){
			return beta_ws() - lambda;
			//return beta_ws() - lambda + M_PI;
		}
		double R_n_air(){
			return sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_s)/1.5111e-5;
		}
		double C_Ls(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=0 && alpha<=100){
				return -1.877662 + 0.265759*alpha - 0.006852092*alpha*alpha + 0.00006764791*alpha*alpha*alpha - 2.366522e-7*alpha*alpha*alpha*alpha;
			}else{
				std::cout << "Request of calculation of C_Ls for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		double C_Fs(){
			double denominator = log10(R_n_air())-2;
			return 0.075/(denominator*denominator);
		}
		double C_Ds(double alpha){
			alpha = alpha*180/M_PI;
			if (alpha>=0 && alpha<=100){
				double C_Ds0 = 0.1638985 + 0.00133082*alpha + 0.00001135329*alpha*alpha + 0.000001185599*alpha*alpha*alpha;
				return C_Ds0 + 2*(1+2*(t_max/sqrt(A_s)))*C_Fs();
			}else{
				std::cout << "Request of calculation of C_Ds for an impossible value: " << alpha << std::endl;
				return 0;
			}
		}
		double S_L(){
			return 0.5*rho_air*A_s*C_Ls(alpha_s())*V_ws().norm();
		}
		double S_D(){
			return 0.5*rho_air*A_s*C_Ds(alpha_s())*V_ws().norm();
		}
		Vector4d S(){
			double my_beta_ws = beta_ws();
			double my_S_L = S_L();
			double my_S_D = S_D();
			double S_x = -my_S_L*sin(my_beta_ws)+my_S_D*cos(my_beta_ws);
			double S_y = my_S_L*cos(my_beta_ws)+my_S_D*sin(my_beta_ws);
			double S_x_p = S_x*cos(eta(3))+S_y*sin(eta(3));
			double S_y_p = -S_x*sin(eta(3))+S_y*cos(eta(3));
			return Vector4d(S_x, S_y, h_1*S_y, S_x_p*l_2*sin(lambda)-S_y_p*(l_1+l_2*cos(lambda)));
		}
		// All I need for calculating K
		Vector2d V_ck(){
			Vector4d my_nu_r = nu_r();
			return - Vector2d(my_nu_r(0), my_nu_r(1)) - Vector2d(0, -nu(3)*l_3-nu(2)*h_2);
		}
		double beta_ck(){
			Vector2d my_V_ck = V_ck();
			return atan2(my_V_ck(1), my_V_ck(0));
		}
		double alpha_k(){
			return -beta_ck()-M_PI;
		}
		double R_n_k(){
			return sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_k)/9.7937e-7;
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
		double K_L(){
			return 0.5*rho_water*A_k*C_Lk(alpha_k())*V_ck().norm();
		}
		double K_D(){
			return 0.5*rho_water*A_k*C_Dk(alpha_k())*V_ck().norm();
		}
		Vector4d K(){
			double my_beta_ck = beta_ck();
			double my_K_L = K_L();
			double my_K_D = K_D();
			double K_x = my_K_L*sin(my_beta_ck)+my_K_D*cos(my_beta_ck);
			double K_y = -my_K_L*cos(my_beta_ck)+my_K_D*sin(my_beta_ck);
			return Vector4d(K_x, K_y, -h_2*K_y, -l_3*K_y);
		}
		// All I need for calculating R
		Vector2d V_cr(){
			Vector4d my_nu_r = nu_r();
			return - Vector2d(my_nu_r(0), my_nu_r(1)) - Vector2d(0, -nu(3)*l_4-nu(2)*h_3);
		}
		double beta_cr(){
			Vector2d my_V_cr = V_cr();
			return atan2(my_V_cr(1), my_V_cr(0));
		}
		double alpha_r(){
			return -beta_cr()+sigma+M_PI;
		}
		double R_n_r(){
			return sqrt(nu(0)*nu(0) + nu(1)*nu(1))*sqrt(A_r)/9.7937e-7;
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
		double R_L(){
			return 0.5*rho_water*A_r*C_Lr(alpha_r())*V_cr().norm();
		}
		double R_D(){
			return 0.5*rho_water*A_r*C_Dr(alpha_r())*V_cr().norm();
		}
		Vector4d R(){
			double my_beta_cr = beta_cr();
			double my_R_L = R_L();
			double my_R_D = R_D();
			double R_x = my_R_L*sin(my_beta_cr)+my_R_D*cos(my_beta_cr);
			double R_y = -my_R_L*cos(my_beta_cr)+my_R_D*sin(my_beta_cr);
			return Vector4d(R_x, R_y, -h_3*R_y, -l_4*R_y);
		}
		Current_Model CM;
		Wind_Model WM;
  	public:
	  	Sailing_Boat_Model(	Vector4d initial_eta = Vector4d {0, 0, 0, 0},
	  						Vector4d initial_nu = Vector4d {0, 0, 0, 0},
	  						Current_Model given_CM = Current_Model(Vector2d(0, 0)),
	  						Wind_Model given_WM = Wind_Model(Vector2d(0.1, 0.1))){
	  		eta = initial_eta; // A deep copy is done: I have checked
	  		nu = initial_nu; // A deep copy is done: I have checked
	  		CM = given_CM;
	  		WM = given_WM;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << S() << std::endl;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << K() << std::endl;
	  		std::cout << "--------------------" << std::endl;
	  		std::cout << R() << std::endl;
	  		std::cout << "--------------------" << std::endl;
	  	}

		Vector4d get_eta(){
			return eta;
		}

		Vector4d get_nu(){
			return nu;
		}
};

int main(){
  	Sailing_Boat_Model SBM = Sailing_Boat_Model(Vector4d(0, 0, 0, 0), Vector4d(0, 0, 0, 0));
    plt::plot({1,3,2,4});
    plt::show();
}

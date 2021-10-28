#include <tuple>

#ifdef _HAS_CXX20
#include <numbers>
#else
#define CC_NUMBERS
#endif

#ifdef BOOST_UBLAS
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#define  _init_ <<=
#endif

#ifdef EIGEN
#include <Eigen/core>
#define _init_ <<
#endif

namespace coordinate_conversion_test { 
	namespace numbers {
#ifdef CC_NUMBERS
		constexpr double pi = 3.141592653589793;
#else
		using std::numbers::pi;
#endif
		constexpr double earth_rotate_rate = 0.0007292115;
		constexpr double earth_radius = 6378137.0;
		constexpr double earth_polar_radius = 6356752.3142;
		constexpr double eccentricity = 0.081819190842622;
		constexpr double eccent_sqrd = 0.0066943800042608354;
		constexpr double e_sqrd_prime = 0.0067394967565869027;
		constexpr double flattening = 1 - eccent_sqrd;
		constexpr double radians_to_degrees = 180 / pi;
		constexpr double degrees_to_radians = pi / 180;
	}

#ifdef BOOST_UBLAS
	namespace ublas = boost::numeric::ublas;
	template<int rows, int cols>
	using matrix = ublas::bounded_matrix<double, rows, cols, boost::numeric::ublas::column_major>;
	template<int rows>
	using vector = ublas::bounded_vector<double, rows>;
	using vector3d = vector<3>;
	using matrix3d = matrix<3, 3>;
	inline auto make_vector(double const& p1, double const& p2, double const& p3)
	{
		vector3d temp; temp[0] = p1; temp[1] = p2, temp[2] = p3; return temp;
	}

#define matrixbase(type) type 
#endif
	

#ifdef EIGEN
#define matrixbase(type) Eigen::MatrixBase<type> 
	using vector3d = Eigen::Vector3d;
	using matrix3d = Eigen::Matrix3d;
	using make_vector = vector3d;
	template<int rows, int cols>
	using matrix = Eigen::Matrix<double, rows, cols>;
#endif 

	inline auto cosd(double const& degrees) { return std::cos(degrees * numbers::degrees_to_radians); }
	inline auto sind(double const& degrees) { return std::sin(degrees * numbers::degrees_to_radians); }
	inline auto sindcosd(double const& degrees) { return std::tuple{ sind(degrees), cosd(degrees) };  }
	inline auto atan2d(double const& x, double const &y) { return std::atan2(x,y) * numbers::radians_to_degrees; }
	inline auto atand(double const& x) { return std::atan(x) * numbers::radians_to_degrees; }
	inline auto asind(double const& x) { return std::asin(x) * numbers::radians_to_degrees; }
	inline auto mod360(double const & degree) {
		double degrees = std::fmod(degree, 360);
		if (degrees < 0) degrees += 360;
		return degrees;
	}

	inline const auto prime_vertical_radius (double const & sin_lat)  {
		return (numbers::earth_radius / std::sqrt(1 - numbers::eccent_sqrd * std::pow(sin_lat, 2.)));
	}
	
	// should be spherical to ecef then spherical to enu. so you can do ned or whatever. Generalize more.
	template<class lla_t>
	inline auto enu2ecef_rotation(matrixbase(lla_t) const& lla) {
		const auto [sin_lat, cos_lat] { sindcosd(lla[0]) };
		const auto [sin_lon, cos_lon] { sindcosd(lla[1]) };
		matrix3d enu2ecef_rot;
		enu2ecef_rot _init_ -sin_lon, -sin_lat * cos_lon, cos_lat * cos_lon,
					         cos_lon, -sin_lat * sin_lon, cos_lat * sin_lon,
			                       0,            cos_lat,           sin_lat;
		return enu2ecef_rot;
	}

	template<class lla_t>
	inline auto ecef2enu_rotation(matrixbase(lla_t) const& lla) {
		const auto [sin_lat, cos_lat] { sindcosd(lla[0]) };
		const auto [sin_lon, cos_lon] { sindcosd(lla[1]) };
		matrix3d ecef2enu_rot;
		ecef2enu_rot _init_      -sin_lon,            cos_lon,       0,
					   -sin_lat * cos_lon, -sin_lat * sin_lon, cos_lat,
					    cos_lat * cos_lon,  cos_lat * sin_lon, sin_lat;
		return ecef2enu_rot;
	}

	template<class lla_t>
	inline auto lla2ecef(matrixbase(lla_t) const& lla) {
		double const& alt{ lla[2] };
		const auto [sin_lat, cos_lat]{ sindcosd(lla[0]) };
		const auto [sin_lon, cos_lon]{ sindcosd(lla[1]) };
		double const N{ prime_vertical_radius(sin_lat) };
		return make_vector(((N+alt)*cos_lat*cos_lon),
			               ((N+alt)*cos_lat*sin_lon),
			               ((numbers::flattening*N)+alt)*sin_lat);
	}

	template<class ecef_t>
	inline auto ecef2lla(matrixbase(ecef_t) const& ecef) {
		double const& x{ ecef[0] }; double const& y{ ecef[1] }; double const& z{ ecef[2] };
		const double p{ std::sqrt(std::pow(x,2) + std::pow(y,2)) };
		const double theta{ (std::atan((z * numbers::earth_radius) / (p * numbers::earth_polar_radius))) };
		const double sin_theta_cubed{ std::pow(std::sin(theta),3) };
		const double cos_theta_cubed{ std::pow(std::cos(theta),3) };
		const double lat_rad{ std::atan((z + numbers::e_sqrd_prime * numbers::earth_polar_radius * sin_theta_cubed)
			/ (p - numbers::eccent_sqrd * numbers::earth_radius * cos_theta_cubed)) };
		const double lon{ atan2d(y, x) };
		const double alt{ (p / std::cos(lat_rad)) - prime_vertical_radius(std::sin(lat_rad)) };
		return make_vector((lat_rad * numbers::radians_to_degrees), lon, alt);
	}

	// should be more general cartesian2polar
	template<class enu_t>
	inline auto enu2rae(matrixbase(enu_t) const& enu) {
		double const& east{ enu[0] }; double const& north{ enu[1] }; double const& up{ enu[2] };
		const double range    { std::sqrt(east*east + north*north + up*up) };  
		const double azimuth  { mod360(atan2d(east, north)) };
		const double elevation{ asind(up / range) };
		return make_vector(range, azimuth, elevation);
	}

	template<typename rae_t>
	inline auto rae2enu(matrixbase(rae_t)const& rae) {
		const double & rng{ rae[0] };
		const auto [sin_az, cos_az] { sindcosd(rae[1]) };
		const auto [sin_el, cos_el] { sindcosd(rae[2]) };
		return make_vector( (rng * cos_el * sin_az),
							(rng * cos_el * cos_az),
							(rng * sin_el) );
	}

	template<class ecef_t, class lla_t>
	inline auto ecef2enu(matrixbase(ecef_t)const& ecef, matrixbase(lla_t) const& lla) {
		return vector3d{ ecef_enu_rotation(lla) * (ecef - lla2ecef(lla)) };
	}
	template<class enu_t, class lla_t>
	inline auto enu2ecef(matrixbase(enu_t)const& enu, matrixbase(lla_t) const& lla) {
		return vector3d{(enu2ecef_rotation(lla) * enu) + lla2ecef(lla)};
	}
	template<class enu_t, class lla_t>
	inline auto enu2lla(matrixbase(enu_t)const& enu, matrixbase(lla_t) const& lla) {
		return vector3d{ ecef2lla(enu2ecef(enu, lla)) };
	}
	template<class rae_t, class lla_t>
	inline auto rae2lla(matrixbase(rae_t)const& rae, matrixbase(lla_t) const& lla) {
		return vector3d{ enu2lla(rae2enu(rae), lla) };
	}
	template<class rae_t, class lla_t>
	inline auto rae2ecef(matrixbase(rae_t)const& rae, matrixbase(lla_t) const& lla) {
		return vector3d{ lla2ecef(rae2lla(rae, lla)) };
	}
	template<class lla_t, class ref_lla_t>
	inline auto lla2enu(matrixbase(lla_t)const& lla, matrixbase(ref_lla_t) const& ref_lla) {
		return vector3d{ ecef2enu(lla2ecef(lla), ref_lla) };
	}
	template<class lla_t, class ref_lla_t>
	inline auto lla2rae(matrixbase(lla_t)const& lla, matrixbase(lla_t) const& ref_lla) {
		return vector3d{ enu2rae(lla2enu(lla, ref_lla)) };
	}
	template<class ecef_t, class lla_t>
	inline auto ecef2rae(matrixbase(ecef_t)const& ecef, matrixbase(lla_t) const& lla) {
		return vector3d{ enu2rae(ecef2enu(ecef, lla)) };
	}
}

namespace cct = coordinate_conversion_test;

#include <numbers>
#include <Eigen/Dense>


namespace coordinate_conversions {

	constexpr double radians_to_degrees = 180 / std::numbers::pi;
	constexpr double degrees_to_radians = std::numbers::pi / 180;

	namespace numbers {
		constexpr double earth_rotate_rate = 0.0007292115;
		constexpr double earth_radius = 6378137.0;
		constexpr double earth_polar_radius = 6356752.3142;
		constexpr double eccentricity = 0.081819190842622;
		constexpr double eccent_sqrd = 0.0066943800042608354;
		constexpr double e_sqrd_prime = 0.0067394967565869027;
		constexpr double flattening = 1 - eccent_sqrd;
	}



	//using namespace Eigen;
	template<typename derived>
	using MatrixBase = typename Eigen::MatrixBase<derived>;

	template<typename scalar, int rows, int cols>
	using Matrix = typename Eigen::Matrix<scalar, rows, cols>;



	// should be spherical to ecef then spherical to enu. so you can do ned or whatever. Generalize more.
	template<typename derived>
	inline auto enu2ecef_rotation(MatrixBase<derived> const& reference_lla) {
		using scalar_type = typename derived::Scalar;
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::RowsAtCompileTime >;

		scalar_type sin_latitude{ (scalar_type)(std::sin(reference_lla[0] * degrees_to_radians)) };
		scalar_type cos_latitude{ (scalar_type)(std::cos(reference_lla[0] * degrees_to_radians)) };
		scalar_type sin_longitude{ (scalar_type)(std::sin(reference_lla[1] * degrees_to_radians)) };
		scalar_type cos_longitude{ (scalar_type)(std::cos(reference_lla[1] * degrees_to_radians)) };

		matrix_type enu2ecef_rot;
		enu2ecef_rot << -sin_longitude, -sin_latitude * cos_longitude, cos_latitude* cos_longitude,
			             cos_longitude, -sin_latitude * sin_longitude, cos_latitude* sin_longitude,
			                         0,                  cos_latitude,                sin_latitude;

		return enu2ecef_rot;
	}

	template<typename scalar>
	inline auto prime_vertical_radius(scalar const& sin_lat) {
		return (numbers::earth_radius / std::sqrt(1 - numbers::eccent_sqrd * std::pow(sin_lat, 2.)));
	}

	// MathWorks Function.
	template <typename derived>
	inline auto lla2ecef(MatrixBase<derived> const& lla) {
		using scalar_type = typename derived::Scalar;
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;

		scalar_type sin_latitude{ (scalar_type)(std::sin(lla[0] * degrees_to_radians)) };
		scalar_type cos_latitude{ (scalar_type)(std::cos(lla[0] * degrees_to_radians)) };
		scalar_type sin_longitude{ (scalar_type)(std::sin(lla[1] * degrees_to_radians)) };
		scalar_type cos_longitude{ (scalar_type)(std::cos(lla[1] * degrees_to_radians)) };

		scalar_type const& altitude = lla[2];

		scalar_type N{ (scalar_type)prime_vertical_radius(sin_latitude) };


		matrix_type ecef;
		ecef << ((N + altitude) * cos_latitude * cos_longitude),
				((N + altitude) * cos_latitude * sin_longitude),
				(((numbers::flattening)*N + altitude) * sin_latitude);

		return ecef;
	}

	template <typename derived>
	inline auto ecef2lla(MatrixBase<derived> const& ecef) {
		using scalar_type = typename derived::Scalar;
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;

		scalar_type const& x{ ecef[0] }; scalar_type const& y{ ecef[1] }; scalar_type const& z{ ecef[2] };
		scalar_type p{ (scalar_type)std::sqrt(std::pow(x,2) + std::pow(y,2)) };
		scalar_type theta{ (scalar_type)(std::atan((z * numbers::earth_radius) / (p * numbers::earth_polar_radius))) };
		scalar_type sin_theta_cubed{ (scalar_type)(std::pow(std::sin(theta),3)) };
		scalar_type cos_theta_cubed{ (scalar_type)(std::pow(std::cos(theta),3)) };

		matrix_type lla;
		scalar_type lat_rad{ ((scalar_type)(std::atan((z + numbers::e_sqrd_prime * numbers::earth_polar_radius * sin_theta_cubed)
			/ (p - numbers::eccent_sqrd * numbers::earth_radius * cos_theta_cubed)))) };
		scalar_type lon{ ((scalar_type)(radians_to_degrees * std::atan2(y, x))) };
		scalar_type alt{ ((scalar_type)((p / std::cos(lat_rad)) - prime_vertical_radius(std::sin(lat_rad)))) };

		lla << (scalar_type)(lat_rad * radians_to_degrees), lon, alt;

		return lla;

	}

	template<typename derived>
	inline auto ecef2enu(MatrixBase<derived> const& ecef, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ enu2ecef_rotation(reference_lla).transpose() * (ecef - lla2ecef(reference_lla)) };
	}

	template<typename derived>
	inline auto enu2ecef(MatrixBase<derived> const& enu, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ (enu2ecef_rotation(reference_lla) * enu) + lla2ecef(reference_lla) };
	}

	template<typename derived>
	inline auto enu2lla(MatrixBase<derived> const& enu, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ ecef2lla(enu2ecef(enu,reference_lla)) };
	}

	template<typename scalar>
	inline auto modulus_360(scalar degrees) {
		degrees = std::fmod(degrees, 360);
		if (degrees < 0) degrees += 360;
		return degrees;
	}

	// should be more general cartesian2polar
	template<typename derived>
	inline auto enu2rae(MatrixBase<derived> const& enu) {
		using scalar_type = typename derived::Scalar;
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		scalar_type const& east{ enu[0] }; scalar_type const& north{ enu[1] }; scalar_type const& up{ enu[2] };

		scalar_type range{ std::sqrt(east * east + north * north + up * up) };
		scalar_type azimuth{ (scalar_type)modulus_360(radians_to_degrees * std::atan2(enu[0], enu[1])) };
		scalar_type elevation{ (scalar_type)radians_to_degrees * std::asin(enu[2] / range) };

		return matrix_type{ range, azimuth, elevation };
	}

	template<typename derived>
	inline auto rae2enu(MatrixBase<derived>const& rae) {
		using scalar_type = typename derived::Scalar;
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;

		scalar_type sin_az{ (scalar_type)(std::sin(rae[1] * degrees_to_radians)) };
		scalar_type cos_az{ (scalar_type)(std::cos(rae[1] * degrees_to_radians)) };
		scalar_type sin_el{ (scalar_type)(std::sin(rae[2] * degrees_to_radians)) };
		scalar_type cos_el{ (scalar_type)(std::cos(rae[2] * degrees_to_radians)) };
		scalar_type const& rng{ rae[0] };
		return matrix_type{ (rng * cos_el * sin_az),
							(rng * cos_el * cos_az),
							(rng * sin_el) };
	}

	template<typename derived>
	inline auto rae2lla(MatrixBase<derived> const& rae, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ enu2lla(rae2enu(rae), reference_lla) };
	}

	template<typename derived>
	inline auto rae2ecef(MatrixBase<derived> const& rae, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ lla2ecef(rae2lla(rae, reference_lla)) };
	}

	template<typename derived>
	inline auto lla2enu(MatrixBase<derived> const& lla, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ ecef2enu(lla2ecef(lla), reference_lla) };
	}

	template<typename derived>
	inline auto lla2rae(MatrixBase<derived> const& lla, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ enu2rae(lla2enu(lla, reference_lla)) };
	}

	template<typename derived>
	inline auto ecef2rae(MatrixBase<derived> const& ecef, MatrixBase<derived> const& reference_lla) {
		using matrix_type = Matrix < derived::Scalar, derived::RowsAtCompileTime, derived::ColsAtCompileTime >;
		return matrix_type{ enu2rae(ecef2enu(ecef, reference_lla)) };
	}
}

namespace cc = coordinate_conversions;
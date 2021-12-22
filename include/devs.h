#pragma once 
#include <vector>
#include <algorithm>
#include <variant>

#define EIGEN 
#include "cc_mega.h"

/// <summary>
/// discrete event virtual simualator : DEVS 
/// Sequential event processing :
/// 1. 
/// </summary>
/// 




namespace devs{
	
	template <class T, class U> struct variant_concat;
	template <class... T, class U> struct variant_concat<std::variant<T...>, U> { using type = std::variant<T..., U>; };
	template <class T, class U> using variant_concat_t = typename variant_concat<T, U>::type;

	// you can either have an function of position based on time for each agent and
	using frequency_t    = std::uint64_t;
	using seconds_t      = double;
	using position_enu_t = Eigen::Vector3d;
	using velocity_enu_t = Eigen::Vector3d;

	using positions_t  = std::vector<position_enu_t>;
	using velocities_t = std::vector<velocity_enu_t>;

	using rae_t = Eigen::Vector3d;
	using ae_t  = Eigen::Vector2d;
	using ae_vec_t  = std::vector < ae_t >;
	using rae_vec_t = std::vector < rae_t >;
	using obs_t = std::variant < rae_vec_t, ae_vec_t >;
	
	using origin_t        = Eigen::Vector3d;
	using time_samples_t  = std::vector<double>;
	using agents_t        = std::vector<agent_t>;
	using simple_agents_t = std::vector<agent>;

	struct agent_t {
		positions_t positions;
		velocities_t velocities;
	};

	struct agent {
		position_enu_t pos;
		velocity_enu_t vel;
		bool obs;
	};
	
	struct clock_t {
		const std::uint64_t start_time;
		const std::uint64_t end_time;
	};

	class event {
		// constants
		const frequency_t frequency;
		const seconds_t   time_increment;
		const clock_t     clock;

		// generated per event
		agents_t agents;
		time_samples_t times;

		// outputs
		obs_t obs;
		
		auto generate_agents() {
			time_samples_t current_seconds;
			const auto total_seconds = (clock.end_time - clock.start_time);
			const auto time_count    = total_seconds * frequency;

			agents.reserve(time_count);
			times.reserve(time_count);
			current_seconds.reserve(time_count);
			for (std::size_t i{ 0 }; i < total_seconds; i++) {
				//current_second [0]= i;
			
			}

			auto current_second = clock.start_time;
			for (std::size_t i{ 0 }; i < time_count; i++) {
				current_second += time_increment * i;
				times[i] = current_second;
			}
			
		}

	};
	using events_t = std::vector<event>;



	//template<class Fields = Empty_fields>
	class Simulation{
		events_t events;
		simple_agents_t agents;
		origin_t origin;
		clock_t clock;
		
		void create_events() {


		}

	public:
		explicit Simulation(clock_t&& c, origin_t&& o, simple_agents_t&& a) noexcept
			: agents{ a }, clock{ c }, origin{ o } { create_events(); }
	};


	void tester() {
		Simulation test_sim {
			clock_t  { .start_time = 0, .end_time = 600 },
			origin_t { 33.921389, -118.406111, 5000 },
			simple_agents_t {
				agent {.pos = {1500, 200,  100}, .vel = { 200, 0 , 0 }, .obs = false },
				agent {.pos = {100,  200,  500}, .vel = { 100, 0 , 0 }, .obs = false },
				agent {.pos = {100,   20, 1000}, .vel = { 200, 0 , 0 }, .obs = true },
				agent {.pos = {100,   800,   0}, .vel = { 200, 0 , 0 }, .obs = true },
			}

		};
	
	}

	
	void simulate (Simulation const & simulation) {
		//std::for_each(std::begin(simulation.events),std::end(simulation.events),)
	}

	class basic_tracker {

	};
}
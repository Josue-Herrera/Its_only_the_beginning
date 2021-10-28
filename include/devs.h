#pragma once 
#include <vector>
#include <algorithm>

/// <summary>
/// discrete event virtual simualator : DEVS 
/// Sequential event processing :
/// 1. 
/// </summary>
/// 
namespace devs{
	using frequency_t = double;
	struct Empty_fields {};

	template<class Fields>
	struct Sim_context {};

	template<class Fields>
	using Simulate_contex_func_t = void(Sim_context<Fields> &);
	template<class Fields>
	using sim_contexts_v = std::vector<Simulate_contex_func_t<Fields>>;

	template<class Fields>
	struct Sim_context : Fields {
		Simulate_contex_func_t<Fields>  init_context_;
		Simulate_contex_func_t<Fields>  update_context_;
		Simulate_contex_func_t<Fields>  event_context_;
		Simulate_contex_func_t<Fields>	report_context_;
	};

	template<class Fields>
	struct Sim_event {
		Sim_context<Fields>  context;
		frequency_t frequency;
	};

	template<class Fields = Empty_fields>
	struct Simulation { 
		using Sim_events_t = std::vector<Sim_event>;
		Sim_events_t events;
		Sim_context<Fields>  context;
	};

	template<class Fields = Empty_fields>
	void simulate (Simulation <Fields> && simulation) {
		std::for_each(std::begin(simulation.events),std::end(simulation.events),)
	}
}
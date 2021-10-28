#pragma once 

#include <cmath>
#include <string_view>

namespace data_set {

	namespace cities {

		struct city { 
			double latitude;
			double longitude;
			double altitude;
			std::string_view abbrev;
			std::string_view city_name;

			const double & operator[](std::size_t const & index) const {
				return (index == 0) ? latitude  :
					   (index == 1) ? longitude : altitude  ;

			}
			 double& operator[](std::size_t const& index)  {
				return (index == 0) ? latitude  :
					   (index == 1) ? longitude : 
									  altitude;

			}
		};

		bool operator ==(const city& lhs, const city& rhs) {
			return (lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2]);
		}

		constexpr city california[] { 
						{  37.78,  122.32, 500, "[NGZ]",   "Alameda NAS,CA   "},
						{  41.48,  120.53, 500, "[S11]",   "Alturas,CA	     "},
						{  40.98,  124.10, 500, "[ACV]",   "Arcata,CA	     "},
						{  35.43,  119.05, 500, "[BFL]",   "Bakersfield,CA   "},
						{  39.13,  121.45, 500, "[BAB]",   "Beale AFB,CA     "},
						{  33.93,  116.95, 500, "[BUO]",   "Beaumont,CA      "},
						{  35.28,  116.62, 500, "[BYS]",   "Bicycle Lk,CA    "},
						{  34.27,  116.68, 500, "[L35]",  "Big Bear Apt,CA   "},
						{  37.60,  118.60, 500, "[BIH]",  "Bishop,CA	     "},
						{  39.28,  120.70, 500, "[BLU]",  "Blue Canyon,CA    "},
						{  33.62,  114.72, 500, "[BLH]",  "Blythe,CA		 "},
						{  34.20,  118.37, 500, "[BUR]",  "Burbank,CA		 "},
						{  33.30,  117.35, 500, "[NFG]",  "Camp Pendlet,CA	 "},
						{  32.62,  116.47, 500, "[CZZ]",  "Campo,CA			 "},
						{  33.13,  117.28, 500, "[CRQ]",  "Carlsbad,CA		 "},
						{  37.38,  120.57, 500, "[MER]",  "Castle AFB,CA	 "},
						{  39.78,  121.85, 500, "[CIC]",  "Chico,CA			 "},
						{  35.68,  117.68, 500, "[NID]",  "China Lake,CA	 "},
						{  33.97,  117.63, 500, "[CNO]",  "Chino,CA			 "},
						{  37.98,  122.05, 500, "[CCR]",  "Concord,CA		 "},
						{  41.78,  124.23, 500, "[CEC]",  "Crescent Cty,CA	 "},
						{  34.87,  116.78, 500, "[DAG]",  "Daggett,CA		 "},
						{  34.90,  117.88, 500, "[EDW]",  "Edwards AFB,CA	 "},
						{  32.82,  115.68, 500, "[NJK]",  "El Centro,CA		 "},
						{  34.08,  118.03, 500, "[EMT]",  "El Monte,CA		 "},
						{  33.67,  117.73, 500, "[NZJ]",  "El Toro,CA		 "},
						{  41.33,  124.28, 500, "[EKA]",  "Eureka,CA		 "},
						{  36.00,  121.32, 500, "[HGT]",  "Fort Hunter,CA	 "},
						{  36.68,  121.77, 500, "[OAR]",  "Fort Ord,CA		 "},
						{  36.77,  119.72, 500, "[FAT]",  "Fresno,CA		 "},
						{  33.87,  117.97, 500, "[FUL]",  "Fullerton,CA		 "},
						{  34.58,  117.38, 500, "[VCV]",  "George AFB,CA	 "},
						{  33.92,  118.33, 500, "[HHR]",  "Hawthorne,CA		 "},
						{  37.65,  122.12, 500, "[HWD]",  "Hayward,CA		 "},
						{  32.83,  115.57, 500, "[IPL]",  "Imperial,CA		 "},
						{  32.57,  117.12, 500, "[NRS]",  "Imperial Bch,CA	 "},
						{  34.10,  117.78, 500, "[POC]",  "La Verne,CA		 "},
						{  38.90,  120.00, 500, "[TVL]",  "Lake Tahoe,CA	 "},
						{  34.73,  118.22, 500, "[WJF]",  "Lancaster,CA		 "},
						{  36.33,  119.95, 500, "[NLC]",  "Lemoore NAS,CA	 "},
						{  37.70,  121.82, 500, "[LVK]",  "Livermore,CA		 "},
						{  33.82,  118.15, 500, "[LGB]",  "Long Beach,CA	 "},
						{  33.78,  118.05, 500, "[SLI]",  "Los Alamitos,CA	 "},
						{  33.93,  118.40, 500, "[LAX]",  "Los Angeles,CA	 "},
						{  37.63,  118.92, 500, "[MMH]",  "Mammoth Lks,CA	 "},
						{  33.88,  117.27, 500, "[RIV]",  "March AFB,CA		 "},
						{  39.10,  121.57, 500, "[MYV]",  "Marysville,CA	 "},
						{  38.57,  121.30, 500, "[MHR]",  "Mather AFB,CA	 "},
						{  38.67,  121.40, 500, "[MCC]",  "Mcclellan,CA		 "},
						{  37.28,  120.52, 500, "[MCE]",  "Merced,CA		 "},
						{  32.87,  117.15, 500, "[NKX]",  "Miramar NAS,CA	 "},
						{  37.63,  120.95, 500, "[MOD]",  "Modesto,CA		 "},
						{  37.42,  122.05, 500, "[NUQ]",  "Moffet NAS,CA	 "},
						{  35.05,  118.15, 500, "[MHV]",  "Mojave,CA	     "},
						{  41.73,  122.53, 500, "[1O5]",  "Montague,CA		 "},
						{  36.58,  121.85, 500, "[MRY]",  "Monterey,CA		 "},
						{  41.32,  122.32, 500, "[MHS]",  "Mount Shasta,CA	 "},
						{  34.23,  118.07, 500, "[MWS]",  "Mount Wilson,CA	 "},
						{  38.22,  122.28, 500, "[APC]",  "Napa,CA			 "},
						{  34.77,  114.62, 500, "[EED]",  "Needles,CA		 "},
						{  32.70,  117.20, 500, "[NZY]",  "North Is,CA		 "},
						{  34.10,  117.23, 500, "[SBD]",  "Norton AFB,CA	 "},
						{  37.73,  122.22, 500, "[OAK]",  "Oakland,CA		 "},
						{  34.05,  117.62, 500, "[ONT]",  "Ontario Intl,CA	 "},
						{  34.20,  119.20, 500, "[OXR]",  "Oxnard,CA		 "},
						{  33.83,  116.50, 500, "[PSP]",  "Palm Springs,CA	 "},
						{  35.05,  118.13, 500, "[PMD]",  "Palmdale,CA		 "},
						{  37.47,  122.12, 500, "[PAO]",  "Palo Alto,CA		 "},
						{  35.67,  120.63, 500, "[PRB]",  "Paso Robles,CA	 "},
						{  37.83,  122.83, 500, "[53Q]",  "Pillaro Pt,CA	 "},
						{  34.12,  119.12, 500, "[NTD]",  "Point Mugu,CA	 "},
						{  39.58,  124.22, 500, "[PAA]",  "Pt Arena,CA		 "},
						{  34.95,  121.12, 500, "[PGU]",  "Pt Arguello,CA	 "},
						{  35.67,  121.28, 500, "[87Q]",  "Pt Piedras,CA	 "},
						{  36.12,  121.47, 500, "[PPD]",  "Pt Piedras,CA	 "},
						{  40.15,  122.25, 500, "[RBL]",  "Red Bluff,CA		 "},
						{  40.50,  122.30, 500, "[RDD]",  "Redding,CA		 "},
						{  33.95,  117.45, 500, "[RAL]",  "Riverside,CA		 "},
						{  38.52,  121.50, 500, "[SAC]",  "Sacramento,CA	 "},
						{  38.70,  121.60, 500, "[SMF]",  "Sacramento,CA	 "},
						{  36.67,  121.60, 500, "[SNS]",  "Salinas,CA		 "},
						{  37.52,  122.25, 500, "[SQL]",  "San Carlos,CA	 "},
						{  33.42,  117.62, 500, "[L10]",  "San Clemente,CA	 "},
						{  33.02,  118.58, 500, "[NUC]",  "San Clemente,CA	 "},
						{  32.82,  117.13, 500, "[MYF]",  "San Diego,CA		 "},
						{  32.73,  117.17, 500, "[SAN]",  "San Diego,CA		 "},
						{  32.57,  116.98, 500, "[SDM]",  "San Diego,CA		 "},
						{  32.57,  116.98, 500, "[SDM]",  "San Diego,CA		 "},
						{  32.82,  116.97, 500, "[SEE]",  "San Diego,CA		 "},
						{  37.75,  122.68, 500, "[51Q]",  "San Francisco,CA	 "},
						{  37.62,  122.38, 500, "[SFO]",  "San Francisco,CA	 "},
						{  37.37,  121.92, 500, "[SJC]",  "San Jose,CA		 "},
						{  37.33,  121.82, 500, "[RHV]",  "San Jose / Rei,CA "},
						{  35.23,  120.65, 500, "[SBP]",  "San Luis Obi,CA	 "},
						{  33.38,  117.58, 500, "[L98]",  "San Mateo,CA		 "},
						{  34.03,  120.40, 500, "[N5G]",  "San Miguel,CA	 "},
						{  33.25,  119.45, 500, "[NSI]",  "San Nic Isl,CA	 "},
						{  34.75,  118.73, 500, "[SDB]",  "Sandburg,CA		 "},
						{  33.67,  117.88, 500, "[SNA]",  "Santa Ana,CA		 "},
						{  34.43,  119.83, 500, "[SBA]",  "Santa Barb,CA	 "},
						{  34.90,  120.45, 500, "[SMX]",  "Santa Maria,CA	 "},
						{  34.02,  118.45, 500, "[SMO]",  "Santa Monica,CA	 "},
						{  38.52,  122.82, 500, "[STS]",  "Santa Rosa,CA	 "},
						{  40.03,  124.07, 500, "[O87]",  "Shelter Cove,CA	 "},
						{  41.78,  122.47, 500, "[SIY]",  "Siskiyou,CA		 "},
						{  37.90,  121.25, 500, "[SCK]",  "Stockton,CA		 "},
						{  35.33,  117.10, 500, "[4SU]",  "Superior Val,CA	 "},
						{  40.63,  120.95, 500, "[SVE]",  "Susanville,CA	 "},
						{  33.63,  116.17, 500, "[TRM]",  "Thermal,CA		 "},
						{  33.80,  118.33, 500, "[TOA]",  "Torrance,CA		 "},
						{  38.27,  121.93, 500, "[SUU]",  "Travis AFB,CA	 "},
						{  39.32,  120.13, 500, "[TRK]",  "Truckee - Tahoe,CA"},
						{  33.70,  117.83, 500, "[NTK]",  "Tustin Mcas,CA	 "},
						{  34.28,  116.15, 500, "[NXP]",  "Twenty9 Palm,CA	 "},
						{  39.13,  123.20, 500, "[UKI]",  "Ukiah,CA			 "},
						{  34.22,  118.48, 500, "[VNY]",  "Van Nuys,CA		 "},
						{  35.20,  120.95, 500, "[VBG]",  "Vandenberg,CA	 "},
						{  35.20,  120.95, 500, "[VBG]",  "Vandenberg,CA	 "},
						{  36.32,  119.40, 500, "[VIS]",  "Visalia,CA		 "}

					 };
	}

}
#include "generator.h"
#include "enviro.h"
#include "accrete.h"
#include "accreteutils.h"

PlanetPointer innermost_planet;
ldouble dust_density_coeff = DUST_DENSITY_COEFF;

long flag_seed = 0;

int earthlike = 0;
int total_earthlike = 0;
int habitable = 0;
int habitable_jovians = 0;
int total_habitable = 0;

ldouble min_breathable_terrestrial_g = 1000.0;
ldouble min_breathable_g = 1000.0;
ldouble max_breathable_terrestrial_g = 0.0;
ldouble max_breathable_g = 0.0;
ldouble min_breathable_temp = 1000.0;
ldouble max_breathable_temp = 0.0;
ldouble min_breathable_p = 100000.0;
ldouble max_breathable_p = 0.0;
ldouble min_breathable_terrestrial_l = 1000.0;
ldouble min_breathable_l = 1000.0;
ldouble max_breathable_terrestrial_l = 0.0;
ldouble max_breathable_l = 0.0;
ldouble max_moon_mass = 0.0;

int type_counts[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //seb
int type_count = 0;
int max_type_count = 0;

#define EM(x) (x) / SUN_MASS_IN_EARTH_MASSES
#define AVE(x, y) ((x + y) / 2.0)

#define ZEROES 0.0L, 0.0L, 0.0L, 0.0L, 0, 0.0L, 0.0L, 0.0L, 0, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, 0.0L, NULL, 0, NULL, tUnknown

/*					No Orbit	Eccen. Tilt		Mass		Gas Giant?	Dust Mass   Gas */
Planets luna = {1, 2.571e-3, 0.055, 1.53, EM(.01229), false, EM(.01229), 0, ZEROES, 0, NULL, NULL};
Planets callisto = {4, 1.259e-2, 0, 0, EM(1.62e-2), false, EM(1.62 - 2), 0, ZEROES, 0, NULL, NULL};
Planets ganymede = {3, 7.16e-3, 0.0796, 0, EM(2.6e-2), false, EM(2.6e-2), 0, ZEROES, 0, NULL, &callisto};
Planets europa = {2, 4.49e-3, 0.0075, 0, EM(7.9e-3), false, EM(7.9e-3), 0, ZEROES, 0, NULL, &ganymede};
Planets io = {1, 2.82e-3, 0.0006, 0, EM(1.21e-2), false, EM(1.21e-2), 0, ZEROES, 0, NULL, &europa};
Planets iapetus = {6, 2.38e-2, 0.029, 0, EM(8.4e-4), false, EM(8.4e-4), 0, ZEROES, 0, NULL, NULL};
Planets hyperion = {5, 9.89e-3, 0.110, 0, EM(1.82e-5), false, EM(1.82e-5), 0, ZEROES, 0, NULL, &iapetus};
Planets titan = {4, 8.17e-3, 0.0289, 0, EM(2.3e-2), false, EM(2.3e-2), 0, ZEROES, 0, NULL, &hyperion};
Planets rhea = {3, 3.52e-3, 0.0009, 0, EM(3.85e-4), false, EM(3.85e-4), 0, ZEROES, 0, NULL, &titan};
Planets dione = {2, 2.52e-3, 0.0021, 0, EM(1.74e-4), false, EM(1.74e-4), 0, ZEROES, 0, NULL, &rhea};
Planets tethys = {1, 1.97e-3, 0.000, 0, EM(1.09e-4), false, EM(1.09e-4), 0, ZEROES, 0, NULL, &dione};
Planets triton = {1, 2.36e-3, 0.000, 0, EM(2.31e-2), false, EM(2.31e-2), 0, ZEROES, 0, NULL, NULL};
Planets charon = {1, 1.31e-4, 0.000, 0, EM(2.54e-4), false, EM(2.54e-4), 0, ZEROES, 0, NULL, NULL};

Planets eris = {11, 67.6681, 0.44177, 0, EM(.0025), false, EM(.0025), 0, ZEROES, 0, NULL, NULL};
Planets pluto = {10, 39.529, 0.248, 122.5, EM(0.002), false, EM(0.002), 0, ZEROES, 0, &charon, &eris};
Planets neptune = {9, 30.061, 0.010, 29.6, EM(17.14), true, 0, EM(17.14), ZEROES, 0, &triton, &pluto};
Planets uranus = {8, 19.191, 0.046, 97.9, EM(14.530), true, 0, EM(14.530), ZEROES, 0, NULL, &neptune};
Planets saturn = {7, 9.539, 0.056, 26.7, EM(95.18), true, 0, EM(95.18), ZEROES, 0, &tethys, &uranus};
Planets jupiter = {6, 5.203, 0.048, 3.1, EM(317.9), true, 0, EM(317.9), ZEROES, 0, &io, &saturn};
Planets ceres = {5, 2.766, 0.080, 0, EM(0.00015), false, EM(0.00015), 0, ZEROES, 0, NULL, &jupiter};
Planets mars = {4, 1.524, 0.093, 25.2, EM(0.1074), false, EM(0.1074), 0, ZEROES, 0, NULL, &ceres};
Planets earth = {3, 1.000, 0.017, 23.5, EM(1.00), false, EM(1.00), 0, ZEROES, 0, &luna, &mars};
Planets venus = {2, 0.723, 0.007, 177.3, EM(0.815), false, EM(0.815), 0, ZEROES, 0, NULL, &earth};
Planets mercury = {1, 0.387, 0.206, 2, EM(0.055), false, EM(0.055), 0, ZEROES, 0, NULL, &venus};
PlanetPointer solar_system = &mercury;

Planets pluto1 = {10, 39.529, 0.248, 0, 0, false, 0, 0, ZEROES, 0, NULL, NULL};
Planets pluto2 = {10, 39.529, 0.15, 0, 0, false, 0, 0, ZEROES, 0, NULL, NULL};  // The real eccentricity
Planets mars1 = {4, 1.524, 0.093, 0, 0, false, 0, 0, ZEROES, 0, NULL, &pluto2}; // collides Pluto+Neptune
Planets ceres1 = {5, 2.767, 0.079, 0, 0, false, 0, 0, ZEROES, 0, NULL, &mars1};
Planets saturn1 = {7, 9.539, 0.056, 0, 0, false, 0, 0, ZEROES, 0, NULL, &ceres1};
Planets uranus1 = {8, 19.191, 0.046, 0, 0, false, 0, 0, ZEROES, 0, NULL, &saturn1};
Planets neptune1 = {9, 30.061, 0.010, 0, 0, false, 0, 0, ZEROES, 0, NULL, &uranus1};
Planets jupiter1 = {6, 5.203, 0.048, 0, 0, false, 0, 0, ZEROES, 0, NULL, &neptune1};
Planets mercury1 = {1, 0.387, 0.206, 0, 0, false, 0, 0, ZEROES, 0, NULL, &jupiter1};
Planets earth1 = {3, 1.000, 0.017, 0, 0, false, 0, 0, ZEROES, 0, NULL, &mercury1};
Planets venus1 = {2, 0.723, 0.007, 0, 0, false, 0, 0, ZEROES, 0, NULL, &earth1};
PlanetPointer solar_system1 = &venus1;

Planets eriEpsI = {1, 3.3, 0.608, 0, 0, false, 0, 0, ZEROES, 0, NULL, NULL};
Planets UMa47II = {2, 3.73, 0, 0, 0, false, 0, 0, ZEROES, 0, NULL, NULL};
Planets UMa47I = {1, 2.11, 0.096, 0, 0, false, 0, 0, ZEROES, 0, NULL, &UMa47II};
Planets horIotI = {1, 0.925, 0.161, 0, 0, false, 0, 0, ZEROES, 0, NULL, NULL};

Planets smallest = {0, 0.0, 0.0, 20.0, EM(0.4), false, EM(0.4), 0, ZEROES, 0, NULL, NULL};
Planets average = {0, 0.0, 0.0, 20.0, EM(1.0), false, EM(1.0), 0, ZEROES, 0, NULL, NULL};
Planets largest = {0, 0.0, 0.0, 20.0, EM(1.6), false, EM(1.6), 0, ZEROES, 0, NULL, NULL};

/*    L  Mass	Mass2	Eccen.	SemiMajorAxis	Designation	Name	*/
Star perdole[] =
	{
		{0, 1.00, 0, 0, 0, 0, 0, &mercury, "Sol", 1, "The Solar System"},
		{0, 1.08, 0.88, 0.52, 23.2, 0, 0, NULL, "ALF Cen A", 1, "Alpha Centauri A"},
		{0, 0.88, 1.08, 0.52, 23.2, 0, 0, NULL, "ALF Cen B", 1, "Alpha Centauri B"},
		{0, 0.80, 0, 0, 0, 0, 0, &eriEpsI, "EPS Eri", 1, "Epsilon Eridani"},
		{0, 0.82, 0, 0, 0, 0, 0, NULL, "TAU Cet", 1, "Tau Ceti"},
		{0, 0.90, 0.65, 0.50, AVE(22.8, 24.3), 0, 0, NULL, "70 Oph", 1, "70 Ophiuchi A"},
		{0, 0.94, 0.58, 0.53, AVE(69., 71.), 0, 0, NULL, "ETA Cas", 1, "Eta Cassiopeiae A"},
		{0, 0.82, 0, 0, 0, 0, 0, NULL, "SIG Dra", 1, "Sigma Draconis"},
		{0, 0.77, 0.76, 0, 22.0, 0, 0, NULL, "36 Oph", 1, "36 Ophiuchi A"},
		{0, 0.76, 0.77, 0, 22.0, 0, 0, NULL, "36 Oph B", 0, "36 Ophiuchi B"},
		{0, 0.76, 0.5, 0.5, 46.0, 0, 0, NULL, "HD 191408", 1, "HR7703 A"},
		{0, 0.98, 0, 0, 0, 0, 0, NULL, "DEL Pav", 1, "Delta Pavonis"},
		{0, 0.91, 0, 0, 0, 0, 0, NULL, "82 Eri", 1, "82 Eridani"},
		{0, 1.23, 0, 0, 0, 0, 0, NULL, "BET Hyi", 1, "Beta Hydri"},
		{0, 0.74, 0, 0, 0, 0, 0, NULL, "HD 219134", 1, "HR8832"},
		{0, 0.725, 0, 0, 1100.0, 0, 0, NULL, "HD 16160", 1, "HR753 A"}};

Star web[] =
	// L			Mass			Mass2			Eccen.	SMAxis	 Planets	Designation   InCel Name
	{
		{1.00, 1.00, 0, 0, 0, 0, 0, &mercury, "Sol", 1, "The Solar System"},					  // 0
		{1.60, 1.09, 0.90, 0.519, 23.7, 0, 0, NULL, "ALF Cen A", 1, "Alpha Centauri A"},		  // 4.4
		{0.45, 0.90, 1.09, 0.519, 23.7, 0, 0, NULL, "ALF Cen B", 1, "Alpha Centauri B"},		  // 4.4
		{0.34, 0.85, 0, 0, 0, 0, 0, &eriEpsI, "EPS Eri", 1, "Epsilon Eridani"},					  // 10.5
		{0.085, 0.59, 0.5, 0.48, 85.2, 0, 0, NULL, "61 Cyg A", 1, "61 Cygni A"},				  // 11.4
		{0.59, 0.82, 0, 0, 0, 0, 0, NULL, "TAU Cet", 1, "Tau Ceti"},							  // 11.9
		{0.38, 0.75, (0.16 + 0.43), 0, 418.0, 0, 0, NULL, "40 Eri", 1, "40 Eridani A"},			  // 16.5
		{AVE(.44, .47), 0.924, 0.701, 0.495, 23.3, 0, 0, NULL, "70 Oph", 1, "70 Ophiuchi A"},	 // 16.6
		{0.39, 0.82, 0, 0, 0, 0, 0, NULL, "SIG Dra", 1, "Sigma Draconis"},						  // 18.8
		{0.156, 0.76, (0.55 + 0.35), 0.20, 190.0, 0, 0, NULL, "33 g Lib", 1, "HR 5568"},		  // 19.3
		{AVE(1.0, 1.29), 0.91, 0.56, 0.497, 71.0, 0, 0, NULL, "ETA Cas", 1, "Eta Cassiopeiae A"}, // 19.4
		{0.23, 0.82, 0.20, 0.5, 43.0, 0, 0, NULL, "HD 191408", 1, "HR 7703 (HJ 5173) A"},		  // 19.7
		{0.65, 0.97, 0, 0, 0, 0, 0, NULL, "82 Eri", 1, "82 Eridani"},							  // 19.8
		{1.2, 0.98, 0, 0, 0, 0, 0, NULL, "DEL Pav", 1, "Delta Pavonis"},						  // 19.9
		{0, 0.74, 0, 0, 0, 0, 0, NULL, "HD 219134", 1, "HR 8832"},								  // 21.3
		{0.52, 0.90, 0.76, 0.51, 33.6, 0, 0, NULL, "XI Boo", 1, "Xi Bootis A"},					  // 21.8
		{0.21, 0.81, 0.082, 0.75, 15.0, 0, 0, NULL, "HD 16160", 1, "HR 753 A"},					  // 23.5
		{0.24, 0.83, 0, 0, 0, 0, 0, NULL, "HD 4628", 1, "BD+04 123 (HR 222)"},					  // 24.3
		{3.6, 1.1, 0, 0, 0, 0, 0, NULL, "BET Hyi", 1, "Beta Hydri"},							  // 24.4
		{0.37, 0.89, 0, 0, 0, 0, 0, NULL, "107 Psc", 1, "107 Piscium"},							  // 24.4
		{3., 1.3, 0, 0, 0, 0, 0, NULL, "PI3 Ori", 1, "Pi3 Orionis A"},							  // 26.2
		{0.28, 0.88, 0.86, 0.534, 63.7, 0, 0, NULL, "RHO1 Eri", 1, "Rho Eridani A"},			  // 26.6
		{0.25, 0.86, 0.88, 0.534, 63.7, 0, 0, NULL, "RHO2 Eri", 1, "Rho Eridani B"},			  // 26.6
		{1.2, 1.07, 0, 0, 0, 0, 0, NULL, "BET CVn", 1, "Chara"},								  // 27.3
		{2.9, .90, 1.45, 0.412, 21.2, 0, 0, NULL, "XI UMa", 1, "Xi Ursae Majoris Ba"},			  // 27.3
		{0.80, 0.96, 0, 0, 0, 0, 0, NULL, "61 Vir", 1, "61 Virginis"},							  // 27.8
		{1.3, 0.98, 0, 0, 0, 0, 0, NULL, "ZET Tuc", 1, "Zeta Tucanae"},							  // 28.0
		{1.08, 1.0, 0.15, 0.45, 6.4, 0, 0, NULL, "CHI1 Ori", 1, "Chi1 Orionis A"},				  // 28.3
		{0.41, 0.9, 0.6, 0.779, 91.5, 0, 0, NULL, "41 Ari", 1, "41 Arae A"},					  // 28.7
		{0.21, 0.845, 0, 0, 0, 0, 0, NULL, "HR 1614", 0, "BD-05 1123 (HR 1614) A"},				  // 28.8
		{0.33, 0.87, 0, 0, 0, 0, 0, NULL, "HR 7722", 0, "CD-27 14659 (HR 7722)"},				  // 28.8
		{2.6, 1.2, 0.63, 0.5, 864.0, 0, 0, NULL, "GAM Lep", 1, "Gamma Leporis A"},				  // 29.3
		{1.4, 1.05, 0, 0, 0, 0, 0, NULL, "BET Com", 1, "Beta Comae Berenices"},					  // 29.9
		{0.85, 1.0, 0, 0, 0, 0, 0, NULL, "KAP1 Cet", 1, "Kappa Ceti"},							  // 29.9
		{1.5, 0.8, 0, 0, 0, 0, 0, NULL, "GAM Pav", 1, "Gamma Pavonis"},							  // 30.1
		{0.82, 0.8, 0.07, 0.6, 235.0, 0, 0, NULL, "HD 102365", 1, "HR 4523"},					  // 30.1
		{0.588, 0.81, 0, 0, 0, 0, 0, NULL, "61 UMa", 1, "61 Ursae Majoris"},					  // 31.1
		{0.31, 0.87, 0, 0.5, 80.5, 0, 0, NULL, "HR 4458", 0, "CD-32 8179 (HR 4458)"},			  // 31.1
		{AVE(.39, .41), 0.90, 0, 0, 0, 0, 0, NULL, "12 Oph", 1, "12 Ophiuchi"},					  // 31.9
		{0.46, 0.92, 0, 0, 0, 0, 0, NULL, "HR 511", 0, "BD+63 238 (HR 511)"},					  // 32.5
		{0.83, 0.87, 0, 0, 0, 0, 0, NULL, "ALF Men", 1, "Alpha Mensae"},						  // 33.1
		{0.93, 0.79, 1.02, 0.5, 9000.0, 0, 0, NULL, "ZET1 Ret", 1, "Zeta 1 Reticuli"},			  // 39.4-39.5
		{0.99, 1.02, 0.79, 0.5, 9000.0, 0, 0, NULL, "ZET2 Ret", 1, "Zeta 2 Reticuli"},			  // 39.4-39.5
		{1.14, 1.05, 2.0, 0.55, 48.5, 0, 0, NULL, "44 Boo", 1, "44 Bootis A"},					  // 41.6
		{1.7, 1.03, 0, 0, 0, 0, 0, &UMa47I, "47 UMa", 1, "47 Ursae Majoris"},					  // 45.9
		{1.8, 1.03, 0, 0, 0, 0, 0, &horIotI, "IOT Hor", 1, "Iota Horologii"},					  // 56.2
		{AVE(.13, .15), AVE(.59, .71), 0, 0, 0, 0, 0, NULL, "EPS Ind", 1, "Epsilon Indi"},		  // 11.8
		{AVE(.083, .09), 0.701, 0.924, 0.495, 23.3, 0, 0, NULL, "70 Oph", 1, "70 Ophiuchi B"},	// 16.6
		{0.28, 0.85, 0.85, 0.922, 88.0, 0, 0, NULL, "36 Oph", 1, "36 Ophiuchi A"},				  // 19.5
		{0.27, 0.85, 0.85, 0.922, 88.0, 0, 0, NULL, "36 Oph B", 0, "36 Ophiuchi B"},			  // 19.5
		{0.12, 0.75, 0.65, 0.58, 12.6, 0, 0, NULL, "HR 6426", 0, "MLO 4 (HR 6426) A"},			  // 22.7
		{0.146, 0.80, 0.50, 0.5, 500.0, 0, 0, NULL, "BD-05 1844 A", 0, "BD-05 1844 A"}			  // 28.3
};

Catalog solstation = {sizeof(web) / sizeof(Star), "w", web};
Catalog dole = {sizeof(perdole) / sizeof(Star), "d", perdole};

ChemTable gases[] =
	{
		//   An   sym   HTML symbol                      name                 Aw      melt    boil    dens       ABUNDe       ABUNDs         Rea	Max inspired pp
		{AN_H, "H", "H<SUB><SMALL>2</SMALL></SUB>", "Hydrogen", 1.0079, 14.06, 20.40, 8.99e-05, 0.00125893, 27925.4, 1, 0.0},
		{AN_HE, "He", "He", "Helium", 4.0026, 3.46, 4.20, 0.0001787, 7.94328e-09, 2722.7, 0, MAX_HE_IPP},
		{AN_N, "N", "N<SUB><SMALL>2</SMALL></SUB>", "Nitrogen", 14.0067, 63.34, 77.40, 0.0012506, 1.99526e-05, 3.13329, 0, MAX_N2_IPP},
		{AN_O, "O", "O<SUB><SMALL>2</SMALL></SUB>", "Oxygen", 15.9994, 54.80, 90.20, 0.001429, 0.501187, 23.8232, 10, MAX_O2_IPP},
		{AN_NE, "Ne", "Ne", "Neon", 20.1700, 24.53, 27.10, 0.0009, 5.01187e-09, 3.4435e-5, 0, MAX_NE_IPP},
		{AN_AR, "Ar", "Ar", "Argon", 39.9480, 84.00, 87.30, 0.0017824, 3.16228e-06, 0.100925, 0, MAX_AR_IPP},
		{AN_KR, "Kr", "Kr", "Krypton", 83.8000, 116.60, 119.70, 0.003708, 1e-10, 4.4978e-05, 0, MAX_KR_IPP},
		{AN_XE, "Xe", "Xe", "Xenon", 131.3000, 161.30, 165.00, 0.00588, 3.16228e-11, 4.69894e-06, 0, MAX_XE_IPP},
		//                                                                     from here down, these columns were originally: 0.001,         0
		{AN_NH3, "NH3", "NH<SUB><SMALL>3</SMALL></SUB>", "Ammonia", 17.0000, 195.46, 239.66, 0.001, 0.002, 0.0001, 1, MAX_NH3_IPP},
		{AN_H2O, "H2O", "H<SUB><SMALL>2</SMALL></SUB>O", "Water", 18.0000, 273.16, 373.16, 1.000, 0.03, 0.001, 0, 0.0},
		{AN_CO2, "CO2", "CO<SUB><SMALL>2</SMALL></SUB>", "CarbonDioxide", 44.0000, 194.66, 194.66, 0.001, 0.01, 0.0005, 0, MAX_CO2_IPP},
		{AN_O3, "O3", "O<SUB><SMALL>3</SMALL></SUB>", "Ozone", 48.0000, 80.16, 161.16, 0.001, 0.001, 0.000001, 2, MAX_O3_IPP},
		{AN_CH4, "CH4", "CH<SUB><SMALL>4</SMALL></SUB>", "Methane", 16.0000, 90.16, 109.16, 0.010, 0.005, 0.0001, 1, MAX_CH4_IPP},
		{0, "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int max_gas = (sizeof(gases) / sizeof(ChemTable)) - 1;

void run()
{
	Sun sun = {0.0, 0.0, 0.0, 0.0, 0.0, ""};
	ldouble min_mass = 0.4;
	ldouble inc_mass = 0.05;
	ldouble max_mass = 2.35;
	ldouble sys_inc = 0.0; //seb
	ldouble sys_an = 0.0;  //seb
	int system_count = 1;
	int seed_increment = 1;

	char default_path[] = SUBDIR; /* OS specific */
	char default_url_path[] = "../";
	char *url_path = default_url_path;
	char thumbnail_file[300] = "Thumbnails";
	char file_name[300] = "StarGen";
	char subdir[300] = "";
	char csv_file_name[300] = "StarGen.csv";

	int index;

	for (index = 0; index < max_gas; index++)
	{
		if (gases[index].max_ipp == 0.0)
		{
			gases[index].max_ipp = INCREDIBLY_LARGE_NUMBER;
		}
	}

	qsort(gases, (sizeof(gases) / sizeof(ChemTable)) - 1, sizeof(*gases), diminishing_abundance);

	system_count = 1;

	char system_name[80];
	char designation[80];
	char *cp;
	ldouble outer_limit = 0.0;
	ldouble inner_dust_limit = 0.0;
	int sys_no = 0;
	bool has_known_planets = false;
	PlanetPointer seed_planets = NULL;
	bool use_seed_system = false;
	int in_celestia = 0;

	init();

	sun.name = "Sun Name";
	sun.mass = random_number(min_mass, max_mass);

	generate_stellar_system(&sun, use_seed_system, seed_planets, '?', sys_no, sun.name, inner_dust_limit, outer_limit, 0.077, 0.3, false, false);
}

void init()
{
	if (!flag_seed)
	{
		time_t temp_time;
		unsigned seed = (unsigned)(time(&temp_time));

		(void)srand(seed);

		flag_seed = rand();
	}

	(void)srand(flag_seed);
}

void generate_stellar_system(Sun *sun,
							 bool useSeedSystem, PlanetPointer seedSystem,
							 char flagChar, int sysNo, char *system_name,
							 ldouble innerDustLimit, ldouble outerPlanetLimit, ldouble eccCoef, ldouble innerPlanetFactor,
							 bool doGases, bool doMoons)
{
	printf("Starting stellar generation...\n");

	ldouble outer_dust_limit = stellar_dust_limit(sun->mass);

	if (sun->luminosity == 0)
	{
		sun->luminosity = luminosity(sun->mass);
	}

	sun->r_ecosphere = sqrt(sun->luminosity);
	sun->life = 1.0E10 * (sun->mass / sun->luminosity);

	if (useSeedSystem)
	{
		innermost_planet = seedSystem;
		sun->age = 5.0E9;
	}
	else
	{
		ldouble min_age = 1.0E9;
		ldouble max_age = 6.0E9;

		if (sun->life < max_age)
		{
			max_age = sun->life;
		}

		sun->age = random_number(min_age, max_age);
		innermost_planet = distribute_planetary_masses(sun->mass, sun->luminosity, innerDustLimit, outer_dust_limit, outerPlanetLimit, dust_density_coeff, eccCoef, innerPlanetFactor, seedSystem, doMoons);
	}

	generate_planets(sun, !useSeedSystem, flagChar, sysNo, system_name, doGases, doMoons);
}

void calculate_gases(Sun *sun, PlanetPointer planet, char *planetId)
{

	if (planet->surf_pressure > 0)
	{
		ldouble *amount = (long double *)calloc((max_gas + 1), sizeof(long double));
		ldouble totamount = 0;
		ldouble pressure = planet->surf_pressure / MILLIBARS_PER_BAR;
		int n = 0;
		int i;

		for (i = 0; i < max_gas; i++)
		{
			ldouble yp = gases[i].boil / (373.0 * ((log((pressure) + 0.001) / -5050.5) + (1.0 / 373.0)));

			if ((yp >= 0 && yp < planet->low_temp) && (gases[i].weight >= planet->molec_weight))
			{
				ldouble vrms = rms_velocity(gases[i].weight, planet->exospheric_temp);
				ldouble pvrms = pow(1 / (1 + vrms / planet->esc_velocity), sun->age / 1e9);
				ldouble abund = gases[i].abunds; /* gases[i].abunde */
				ldouble react = 1.0;
				ldouble fract = 1.0;
				ldouble pres2 = 1.0;

				if (strcmp(gases[i].symbol, "Ar") == 0)
				{
					react = .15 * sun->age / 4e9;
				}
				else if (strcmp(gases[i].symbol, "He") == 0)
				{
					abund = abund * (0.001 + (planet->gas_mass / planet->mass));
					pres2 = (0.75 + pressure);
					react = pow(1 / (1 + gases[i].reactivity), sun->age / 2e9 * pres2);
				}
				else if ((strcmp(gases[i].symbol, "O") == 0 || strcmp(gases[i].symbol, "O3") == 0) && sun->age > 2e9 && planet->surf_temp > 270 && planet->surf_temp < 400)
				{
					pres2 = (0.89 + pressure / 4); /*	Breathable - M: .6 -1.8 	*/
					react = pow(1 / (1 + gases[i].reactivity), pow(sun->age / 2e9, 0.25L) * pres2);
				}
				else if (strcmp(gases[i].symbol, "CO2") == 0 && sun->age > 2e9 && planet->surf_temp > 270 && planet->surf_temp < 400)
				{
					pres2 = (0.75 + pressure);
					react = pow(1 / (1 + gases[i].reactivity), pow(sun->age / 2e9, 0.5L) * pres2);
					react *= 1.5;
				}
				else
				{
					pres2 = (0.75 + pressure);
					react = pow(1 / (1 + gases[i].reactivity), sun->age / 2e9 * pres2);
				}

				fract = (1 - (planet->molec_weight / gases[i].weight));

				amount[i] = abund * pvrms * react * fract;

				if ((strcmp(gases[i].symbol, "O") == 0 || strcmp(gases[i].symbol, "N") == 0 || strcmp(gases[i].symbol, "Ar") == 0 || strcmp(gases[i].symbol, "He") == 0 || strcmp(gases[i].symbol, "CO2") == 0))
				{
					printf("%-5.2Lf %-3.3s, %-5.2Lf = a %-5.2Lf * p %-5.2Lf * r %-5.2Lf * p2 %-5.2Lf * f %-5.2Lf\t(%.3Lf%%)\n", planet->mass * SUN_MASS_IN_EARTH_MASSES, gases[i].symbol, amount[i], abund, pvrms, react, pres2, fract, 100.0 * (planet->gas_mass / planet->mass));
				}

				totamount += amount[i];
				if (amount[i] > 0.0)
				{
					n++;
				}
			}
			else
			{
				amount[i] = 0.0;
			}
		}

		if (n > 0)
		{
			planet->gases = n;
			planet->atmosphere = (Gas *)calloc(n, sizeof(Gas));

			for (i = 0, n = 0; i < max_gas; i++)
			{
				if (amount[i] > 0.0)
				{
					planet->atmosphere[n].num = gases[i].num;
					planet->atmosphere[n].surf_pressure = planet->surf_pressure * amount[i] / totamount;

					{
						if ((planet->atmosphere[n].num == AN_O) && inspired_partial_pressure(planet->surf_pressure, planet->atmosphere[n].surf_pressure) > gases[i].max_ipp)
						{
							printf("%s\t Poisoned by O2\n", planetId);
						}
					}

					n++;
				}
			}

			qsort(planet->atmosphere, planet->gases, sizeof(Gas), diminishing_pressure);

			{
				printf("\n%s (%5.1Lf AU) gases:\n", planetId, planet->a);

				for (i = 0; i < planet->gases; i++)
				{
					printf("%3d: %6.1Lf, %11.7Lf%%\n", planet->atmosphere[i].num, planet->atmosphere[i].surf_pressure, 100. * (planet->atmosphere[i].surf_pressure / planet->surf_pressure));
				}
			}
		}

		free(amount);
	}
}

void generate_planet(PlanetPointer planet, int planetNo, Sun *sun, bool random_tilt, char *planet_id, bool do_gases, bool do_moons, bool is_moon)
{
	ldouble tmp;
	ldouble ecc_coef = 0.077;

	planet->atmosphere = NULL;
	planet->gases = 0;
	planet->surf_temp = 0;
	planet->high_temp = 0;
	planet->low_temp = 0;
	planet->max_temp = 0;
	planet->min_temp = 0;
	planet->greenhs_rise = 0;
	planet->planet_no = planetNo;
	planet->sun = sun;
	planet->resonant_period = false;
	planet->orbit_zone = orbital_zone(sun->luminosity, planet->a);
	planet->orb_period = period(planet->a, planet->mass, sun->mass);

	if (random_tilt)
	{
		planet->axial_tilt = inclination(planet->a);
	}

	planet->exospheric_temp = EARTH_EXOSPHERE_TEMP / pow2(planet->a / sun->r_ecosphere);
	planet->rms_velocity = rms_velocity(MOLECULAR_NITROGEN, planet->exospheric_temp);
	planet->core_radius = kothari_radius(planet->dust_mass, false, planet->orbit_zone);
	planet->density = empirical_density(planet->mass, planet->a, sun->r_ecosphere, true);
	planet->radius = volume_radius(planet->mass, planet->density);
	planet->surf_accel = acceleration(planet->mass, planet->radius);
	planet->surf_gravity = gravity(planet->surf_accel);
	planet->molec_weight = min_molec_weight(planet);

	if (((planet->mass * SUN_MASS_IN_EARTH_MASSES) > 1.0) && ((planet->gas_mass / planet->mass) > 0.05) && (min_molec_weight(planet) <= 4.0))
	{
		if ((planet->gas_mass / planet->mass) < 0.20)
		{
			planet->type = tSubSubGasGiant;
		}
		else if ((planet->mass * SUN_MASS_IN_EARTH_MASSES) < 20.0)
		{
			planet->type = tSubGasGiant;
		}
		else if ((planet->mass * SUN_MASS_IN_EARTH_MASSES) > 1200.0) //seb
		{
			planet->type = tBrownDwarf; //seb
		}
		else
		{
			planet->type = tGasGiant;
		}
	}
	else // If not, it's rocky.
	{
		planet->radius = kothari_radius(planet->mass, false, planet->orbit_zone);
		planet->density = volume_density(planet->mass, planet->radius);

		planet->surf_accel = acceleration(planet->mass, planet->radius);
		planet->surf_gravity = gravity(planet->surf_accel);

		if ((planet->gas_mass / planet->mass) > 0.000001)
		{
			ldouble h2_mass = planet->gas_mass * 0.85;
			ldouble he_mass = (planet->gas_mass - h2_mass) * 0.999;

			ldouble h2_loss = 0.0;
			ldouble he_loss = 0.0;

			ldouble h2_life = gas_life(MOLECULAR_HYDROGEN, planet);
			ldouble he_life = gas_life(HELIUM, planet);

			if (h2_life < sun->age)
			{
				h2_loss = ((1.0 - (1.0 / exp(sun->age / h2_life))) * h2_mass);

				planet->gas_mass -= h2_loss;
				planet->mass -= h2_loss;

				planet->surf_accel = acceleration(planet->mass, planet->radius);
				planet->surf_gravity = gravity(planet->surf_accel);
			}

			if (he_life < sun->age)
			{
				he_loss = ((1.0 - (1.0 / exp(sun->age / he_life))) * he_mass);

				planet->gas_mass -= he_loss;
				planet->mass -= he_loss;

				planet->surf_accel = acceleration(planet->mass, planet->radius);
				planet->surf_gravity = gravity(planet->surf_accel);
			}

			if ((h2_loss + he_loss) > .000001)
			{
				printf("%s\tLosing gas: H2: %5.3Lf EM, He: %5.3Lf EM\n", planet_id, h2_loss * SUN_MASS_IN_EARTH_MASSES, he_loss * SUN_MASS_IN_EARTH_MASSES);
			}
		}
	}

	tmp = (planet->mass * SUN_MASS_IN_EARTH_MASSES) / 317.82; // mass as frac of Mj
	if (((planet->type == tGasGiant) || (planet->type == tBrownDwarf)) && (tmp > 0.22))
	{
		if (tmp < 1.65)
		{
			planet->radius = (pow(tmp, 0.1L) * 71492.0);
		} // planet
		else if (tmp < 6.04)
		{
			planet->radius = (pow(tmp, -0.125L) * 71492.0);
		} // brown dwarf
		else
		{
			planet->radius = (0.799 * 71492.0 + (tmp - 6.04) * 71492.0);
		} // star
	}

	planet->day = day_length(planet); /* Modifies planet->resonant_period */

	if ((int)planet->day == (int)(planet->orb_period * 24.0))
	{
		planet->axial_tilt = 0.0;
	}

	planet->esc_velocity = escape_velocity(planet->mass, planet->radius);

	if ((planet->type == tGasGiant) || (planet->type == tBrownDwarf) || (planet->type == tSubGasGiant) || (planet->type == tSubSubGasGiant))
	{
		ldouble temp;

		planet->greenhouse_effect = false;
		planet->volatile_gas_inventory = INCREDIBLY_LARGE_NUMBER;
		planet->surf_pressure = INCREDIBLY_LARGE_NUMBER;

		planet->boil_point = INCREDIBLY_LARGE_NUMBER;

		//seb need temp for surface texture (compare to est.temp)
		// planet->surf_temp				= INCREDIBLY_LARGE_NUMBER;
		planet->greenhs_rise = 0;
		planet->albedo = about(GAS_GIANT_ALBEDO, 0.1);
		planet->hydrosphere = 1.0;
		planet->cloud_cover = 1.0;
		planet->ice_cover = 0.0;
		planet->surf_gravity = gravity(planet->surf_accel);
		planet->molec_weight = min_molec_weight(planet);
		planet->estimated_temp = est_temp(sun->r_ecosphere, planet->a, planet->albedo);
		planet->estimated_terr_temp = est_temp(sun->r_ecosphere, planet->a, EARTH_ALBEDO);

		temp = planet->estimated_terr_temp;

		if ((temp >= FREEZING_POINT_OF_WATER) && (temp <= EARTH_AVERAGE_KELVIN + 10.) && (sun->age > 2.0E9))
		{
			habitable_jovians++;

			printf("%s\t%s (%4.2LfEM %5.3Lf By)%s with earth-like temperature (%.1Lf C, %.1Lf F, %+.1Lf C Earth).\n", planet_id, planet->type == tBrownDwarf ? "Brown Dwarf" : planet->type == tGasGiant ? "Jovian" : planet->type == tSubGasGiant ? "Sub-Jovian" : planet->type == tSubSubGasGiant ? "Gas Dwarf" : "Big", planet->mass * SUN_MASS_IN_EARTH_MASSES, sun->age / 1.0E9, planet->first_moon == NULL ? "" : " WITH MOON", temp - FREEZING_POINT_OF_WATER, 32 + ((temp - FREEZING_POINT_OF_WATER) * 1.8), temp - EARTH_AVERAGE_KELVIN);
		}
		else
		{
			planet->estimated_temp = est_temp(sun->r_ecosphere, planet->a, EARTH_ALBEDO);
			planet->estimated_terr_temp = est_temp(sun->r_ecosphere, planet->a, EARTH_ALBEDO);

			planet->surf_gravity = gravity(planet->surf_accel);
			planet->molec_weight = min_molec_weight(planet);

			planet->greenhouse_effect = greenhouse(sun->r_ecosphere, planet->a);
			planet->volatile_gas_inventory = vol_inventory(planet->mass, planet->esc_velocity, planet->rms_velocity, sun->mass, planet->orbit_zone, planet->greenhouse_effect, (planet->gas_mass / planet->mass) > 0.000001);
			planet->surf_pressure = pressure(planet->volatile_gas_inventory, planet->radius, planet->surf_gravity);

			if ((planet->surf_pressure == 0.0))
			{
				planet->boil_point = 0.0;
			}
			else
			{
				planet->boil_point = boiling_point(planet->surf_pressure);
			}

			iterate_surface_temp(planet); /*	Sets:
								 *		planet->surf_temp
								 *		planet->greenhs_rise
								 *		planet->albedo
								 *		planet->hydrosphere
								 *		planet->cloud_cover
								 *		planet->ice_cover
								 */

			if (do_gases && planet->surf_pressure > 0.05) //seb: but only if there's some air
			{
				calculate_gases(sun, planet, planet_id);
			}

			/*
			 *	Next we assign a type to the planet.
			 */

			if (planet->surf_pressure < 1.0)
			{
				if (!is_moon && ((planet->mass * SUN_MASS_IN_EARTH_MASSES) < ASTEROID_MASS_LIMIT))
				{
					planet->type = tAsteroids;
				}
				else
				{
					planet->type = tRock;
				}
			}
			else if ((planet->surf_pressure > 6000.0) && (planet->molec_weight <= 2.0)) // Retains Hydrogen
			{
				planet->type = tSubSubGasGiant;
			}
			else
			{ // Atmospheres:
				if (((int)planet->day == (int)(planet->orb_period * 24.0) || (planet->resonant_period)))
				{
					planet->type = t1Face;
				}
				else if (planet->hydrosphere >= 0.95)
				{
					planet->type = tWater; // >95% water
				}
				else if (planet->ice_cover >= 0.95)
				{
					planet->type = tIce; // >95% ice
				}
				else if (planet->hydrosphere > 0.05)
				{
					planet->type = tTerrestrial; // Terrestrial
				}
				// else <5% water
				else if (planet->max_temp > planet->boil_point)
				{
					planet->type = tVenusian; // Hot = Venusian
				}
				else if ((planet->gas_mass / planet->mass) > 0.0001)
				{										 // Accreted gas
					planet->type = tIce;				 // But no Greenhouse
					planet->ice_cover = 1.0;			 // or liquid water
				}										 // Make it an Ice World
				else if (planet->surf_pressure <= 250.0) // Thin air = Martian
				{
					planet->type = tMartian;
				}
				else if (planet->surf_temp < FREEZING_POINT_OF_WATER)
				{
					planet->type = tIce;
				}
				else
				{
					planet->type = tUnknown;

					//printf ("%12s\tp=%4.2Lf\tm=%4.2Lf\tg=%4.2Lf\tt=%+.1Lf\t%s\t Unknown %s\n", type_string(planet->type), planet->surf_pressure, planet->mass * SUN_MASS_IN_EARTH_MASSES, planet->surf_gravity, planet->surf_temp  - EARTH_AVERAGE_KELVIN, planet_id, ((int)planet->day == (int)(planet->orb_period * 24.0) || (planet->resonant_period)) ? "(1-Face)" : "");
				}
			}
		}

		if (do_moons && !is_moon)
		{
			if (planet->first_moon != NULL)
			{
				int n;
				PlanetPointer ptr;

				for (n = 0, ptr = planet->first_moon; ptr != NULL; ptr = ptr->next_planet)
				{
					if (ptr->mass * SUN_MASS_IN_EARTH_MASSES > .000001)
					{
						char moon_id[80];
						long double roche_limit = 0.0;
						long double hill_sphere = 0.0;

						ptr->a = planet->a;
						ptr->e = planet->e;

						n++;

						printf(moon_id, "%s.%d", planet_id, n);
						generate_planet(ptr, n, sun, random_tilt, moon_id, do_gases, do_moons, true); // Adjusts ptr->density

						roche_limit = 2.44 * planet->radius * pow((planet->density / ptr->density), (1.0L / 3.0));
						hill_sphere = planet->a * KM_PER_AU * pow((planet->mass / (3.0 * sun->mass)), (1.0L / 3.0));

						if ((roche_limit * 3.0) < hill_sphere)
						{
							ptr->moon_a = random_number(roche_limit * 1.5, hill_sphere / 2.0) / KM_PER_AU;
							ptr->moon_e = random_eccentricity(ecc_coef);
						}
						else
						{
							ptr->moon_a = 0;
							ptr->moon_e = 0;
						}

						printf("   Roche limit: R = %4.2Lg, rM = %4.2Lg, rm = %4.2Lg -> %.0Lf km\n"
							   "   Hill Sphere: a = %4.2Lg, m = %4.2Lg, M = %4.2Lg -> %.0Lf km\n"
							   "%s Moon orbit: a = %.0Lf km, e = %.0Lg\n",
							   planet->radius, planet->density, ptr->density,
							   roche_limit,
							   planet->a * KM_PER_AU, planet->mass * SOLAR_MASS_IN_KILOGRAMS, sun->mass * SOLAR_MASS_IN_KILOGRAMS,
							   hill_sphere,
							   moon_id,
							   ptr->moon_a * KM_PER_AU, ptr->moon_e);

						printf("  %s: (%7.2LfEM) %d %4.2LgEM\n",
							   planet_id,
							   planet->mass * SUN_MASS_IN_EARTH_MASSES,
							   n,
							   ptr->mass * SUN_MASS_IN_EARTH_MASSES);
					}
				}
			}
		}
	}
}

void check_planet(PlanetPointer planet, char *planetId, bool isMoon)
{
	int tIndex = 0;
	uint breathe;
	switch (planet->type)
	{
	case tUnknown:
		tIndex = 0;
		break;
	case tRock:
		tIndex = 1;
		break;
	case tVenusian:
		tIndex = 2;
		break;
	case tTerrestrial:
		tIndex = 3;
		break;
	case tSubSubGasGiant:
		tIndex = 4;
		break;
	case tSubGasGiant:
		tIndex = 5;
		break;
	case tGasGiant:
		tIndex = 6;
		break;
	case tMartian:
		tIndex = 7;
		break;
	case tWater:
		tIndex = 8;
		break;
	case tIce:
		tIndex = 9;
		break;
	case tAsteroids:
		tIndex = 10;
		break;
	case t1Face:
		tIndex = 11;
		break;
	case tBrownDwarf:
		tIndex = 12;
		break; //seb
	}

	if (type_counts[tIndex] == 0)
	{
		++type_count;
	}

	++type_counts[tIndex];

	breathe = breathability(planet);

	if ((breathe == BREATHABLE) && (!planet->resonant_period) && ((int)planet->day != (int)(planet->orb_period * 24.0)))
	{
		bool list_it = false;
		long double illumination = pow2(1.0 / planet->a) * (planet->sun)->luminosity;

		habitable++;

		if (min_breathable_temp > planet->surf_temp)
		{
			min_breathable_temp = planet->surf_temp;
			list_it = true;
		}

		if (max_breathable_temp < planet->surf_temp)
		{
			max_breathable_temp = planet->surf_temp;
			list_it = true;
		}

		if (min_breathable_g > planet->surf_gravity)
		{
			min_breathable_g = planet->surf_gravity;
			list_it = true;
		}

		if (max_breathable_g < planet->surf_gravity)
		{
			max_breathable_g = planet->surf_gravity;
			list_it = true;
		}

		if (min_breathable_l > illumination)
		{
			min_breathable_l = illumination;
			list_it = true;
		}

		if (max_breathable_l < illumination)
		{
			max_breathable_l = illumination;
			list_it = true;
		}

		if (planet->type == tTerrestrial)
		{
			if (min_breathable_terrestrial_g > planet->surf_gravity)
			{
				min_breathable_terrestrial_g = planet->surf_gravity;
				list_it = true;
			}

			if (max_breathable_terrestrial_g < planet->surf_gravity)
			{
				max_breathable_terrestrial_g = planet->surf_gravity;
				list_it = true;
			}

			if (min_breathable_terrestrial_l > illumination)
			{
				min_breathable_terrestrial_l = illumination;
				list_it = true;
			}

			if (max_breathable_terrestrial_l < illumination)
			{
				max_breathable_terrestrial_l = illumination;
				list_it = true;
			}
		}

		if (min_breathable_p > planet->surf_pressure)
		{
			min_breathable_p = planet->surf_pressure;
			list_it = true;
		}

		if (max_breathable_p < planet->surf_pressure)
		{
			max_breathable_p = planet->surf_pressure;
			list_it = true;
		}

		list_it = true;

		if (list_it)
		{
			//	printf ("%12s\tp=%4.2Lf\tm=%4.2Lf\tg=%4.2Lf\tt=%+.1Lf\tl=%4.2Lf\t%s\n", type_string(planet->type), planet->surf_pressure, planet->mass * SUN_MASS_IN_EARTH_MASSES, planet->surf_gravity, planet->surf_temp  - EARTH_AVERAGE_KELVIN, illumination, planet_id);
		}
	}
}

void generate_planets(Sun *sun, bool randomTilt, char flag_char, int sys_no, char *systemName, bool doGases, bool doMoons)
{
	PlanetPointer planet;
	int planet_no = 0;
	PlanetPointer moon;
	int moons = 0;

	for (planet = innermost_planet, planet_no = 1; planet != NULL; planet = planet->next_planet, planet_no++)
	{
		char planet_id[80];

		printf(planet_id, "%s (-s%ld -%c%d) %d", systemName, flag_seed, flag_char, sys_no, planet_no);

		generate_planet(planet, planet_no, sun, randomTilt, planet_id, doGases, doMoons, false);

		/*
		 *	Now we're ready to test for habitable planets,
		 *	so we can count and log them and such
		 */

		check_planet(planet, planet_id, false);

		for (moon = planet->first_moon, moons = 1; moon != NULL; moon = moon->next_planet, moons++)
		{
			char moon_id[80];

			printf(moon_id, "%s.%d", planet_id, moons);
			check_planet(moon, moon_id, true);
		}
	}
}

static int diminishing_abundance(const void *xp, const void *yp)
{
	const ChemTable *x = (ChemTable *)xp;
	const ChemTable *y = (ChemTable *)yp;
	long double xx = x->abunds * x->abunde;
	long double yy = y->abunds * y->abunde;

	if (xx < yy)
	{
		return +1;
	}
	return (xx > yy ? -1 : 0);
}

/*
 *  Sort a ChemTable by decreasing pressure.
 */

static int diminishing_pressure(const void *xp, const void *yp)
{
	const Gas *x = (Gas *)xp;
	const Gas *y = (Gas *)yp;

	if (x->surf_pressure < y->surf_pressure)
	{
		return +1;
	}
	return (x->surf_pressure > y->surf_pressure ? -1 : 0);
}

int stargen(actions action, // One of the above
			char flag_char,
			char *path,			// OS path to where to write files
			char *url_path_arg, // HTML path to parent of both the
			//  directory named in 'path' and
			//  the ref directory with images
			char *filename_arg, // Output file name (optional)
			char *sys_name_arg, // Human readble System name (opt.)

			FILE *sgOut, // Main stream to write to
			//	Thumbnails will be written there
			//  for HTML format
			FILE *sgErr,	  // Stream to write errors to (opt.)
			char *prognam,	// Name of program (opt.)
			ldouble mass_arg, // Mass of star (not used with catalog)
			long seed_arg,	// Random number seed
			int count_arg,	// Number of systems (or cats) to do
			int incr_arg,	 // Amount to increment seed by
			Catalog *cat_arg, // A star catalog (see below)
			int sys_no_arg,   // Star within a catalog (0 = all)

			ldouble ratio_arg,				 // Change dust density (experimental)
			ldouble ecc_coef_arg,			 //seb: Change eccentricity coeffecient in accrete
			ldouble inner_planet_factor_arg, //seb: Change innermost planet limit  in accrete

			int flags_arg,	 // Options (see below)
			int out_format,	// Output file formats (see below)
			int graphic_format // Graphic file formats (see below)
)
{
}

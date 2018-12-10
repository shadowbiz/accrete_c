#include "acdisplay.h"
#include "acmain.h"
#include "acstructs.h"

char *acPlanetTypeToString(planetType type)
{
    char *typeString = "Unknown";

    switch (type)
    {
    case tUnknown:
        typeString = "Unknown";
        break;
    case tRock:
        typeString = "Rock";
        break;
    case tVenusian:
        typeString = "Venusian";
        break;
    case tTerrestrial:
        typeString = "Terrestrial";
        break;
    case tSubSubGasGiant:
        typeString = "GasDwarf";
        break;
    case tSubGasGiant:
        typeString = "Sub-Jovian";
        break;
    case tGasGiant:
        typeString = "Jovian";
        break;
    case tMartian:
        typeString = "Martian";
        break;
    case tWater:
        typeString = "Water";
        break;
    case tIce:
        typeString = "Ice";
        break;
    case tAsteroids:
        typeString = "Asteroids";
        break;
    case t1Face:
        typeString = "1Face";
        break;
    case tBrownDwarf:
        typeString = "BrownDwarf";
        break;
    }
    return typeString;
}

char *acPlanetTypeDescription(planetType type)
{
    char *typeString = "This designation is used for planets that are not classifiable as one of the above types. This category is always temporary, until the program can be made to classify them.";

    switch (type)
    {
    case tRock:
        typeString = "These are planets without atmospheres. They can be hot or cold. The only requirement is a lack of atmosphere. Examples of \"rocks\" in the solar system are Mercury and Pluto.";
        break;
    case tVenusian:
        typeString = "These are planets with a runaway greenhouse effect. The defining characteristics are that they have an atmosphere, water covering less than 5% of their surface and a surface temperature greater than the boiling point of water (and thus actually should have no surface water at all).";
        break;
    case tTerrestrial:
        typeString = "These are Earth-like planets with an atmosphere and a hydrosphere, Water covers between 5% and 95% of their surface.";
        break;
    case tSubSubGasGiant:
        typeString = "These are planets which like the gas giants retain Hydrogen, but are nonetheless mostly rock. Their gas mass is less than 20% of their total mass. They tend to be very far from their sun.";
        break;
    case tSubGasGiant:
        typeString = "These are smaller gas giants such as Neptune and Uranus. They have masses less than 20 times that of Earth. The cut-off is arbitrary, and based on the fact that the blue/green Neptune and Uranus look quite different from the striped Jupiter and Saturn.";
        break;
    case tGasGiant:
        typeString = "These are the larger gas giants, represented in the solar system by Jupiter and Saturn. They have masses at least 20 times that of the Earth.";
        break;
    case tMartian:
        typeString = "These planets have a thin atmosphere and little or no surface water. Less than 5% of the surface is covered with water and less than 95% with ice.";
        break;
    case tWater:
        typeString = "These planets have an atmosphere and a hydrosphere. The water covers more than 95% of the planet's surface.";
        break;
    case tIce:
        typeString = "These planets are covered in ice. It covers at least 95% of the surface. These are generally planets that would be terrestrials if they were warmer. Some are so far out they retain Hydrogen, but are so cold their gases freeze. There is no example of this type of planet in the solar system.";
        break;
    case tAsteroids:
        typeString = "These are small airless bodies. Any planet with a mass less than .1% that of Earth is designated an asteroid. It is assumed that there is a whole belt of such objects.";
        break;
    case t1Face:
        typeString = "This is another class of planets not present in our solar system. It consists of planets whose rotation is tidally locked such that the same side always faces their sun (much as the Moon always presents the same face towards Earth) and which have an atmosphere. Barren 1-face worlds, like our own Mercury, are listed as \"Rock\".";
        break;
    case tBrownDwarf:
        typeString = "These bodies are larger than gas giants. They have masses at least 6 times that of Jupiter. This class of body is not present in our solar system.";
        break;
    }
    return typeString;
}

void acCreateSvgFile(planetPointer innermost_planet)
{
    planetPointer outermost_planet;
    planetPointer planet;
    FILE *file = stdout;
    char file_spec[300];

    sprintf(&file_spec[0], "%s%s%s", "./", "svgfile", ".svg");
    file = fopen(file_spec, "w");

    if (NULL != file)
    {
        for (outermost_planet = innermost_planet; outermost_planet->nextPlanet != NULL; outermost_planet = outermost_planet->nextPlanet)
            ;
        {
            double max_x = 760.0;
            double max_y = 120.0;
            double margin = 20.0;
            double inner_edge = innermost_planet->a * (1.0 - innermost_planet->e);
            double outer_edge = outermost_planet->a * (1.0 + outermost_planet->e);
            double floor = (int)(log10(inner_edge) - 1.);
            double min_log = floor;
            double ceiling = (int)(log10(outer_edge) + 1.);
            double max_log = 0.0;
            double mult = max_x / (max_log - min_log);
            double offset = -mult * (1.0 + min_log);
            double em_scale = 5;
            double x;

            for (x = floor; x <= ceiling; x += 1.0)
            {
                float n;

                for (n = 1.; n < 9.9; n++)
                {
                    if (inner_edge > pow(10.0, x + log10(n)))
                        min_log = x + log10(n);

                    if ((max_log == 0.0) && (outer_edge < pow(10.0, x + log10(n))))
                    {
                        max_log = x + log10(n);
                    }
                }
            }

            mult = max_x / (max_log - min_log);
            offset = -mult * (1.0 + min_log);
            em_scale = 5;

            fprintf(file,
                    "<?xml version='1.0' standalone='no'?>\n"
                    "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' \n"
                    "  'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
                    "\n"
                    "<svg width='100%%' height='100%%' version='1.1'\n"
                    "     xmlns='http://www.w3.org/2000/svg'\n"
                    "     viewBox='-%.0Lf -%.0Lf %.0Lf %.0Lf' >\n"
                    "\n"
                    "<title>%s</title>\n"
                    "<desc>Created by: %s - %s</desc>\n"
                    "\n"
                    "<g stroke='black' stroke-width='1'>\n"
                    "    <line x1='%.0Lf' y1='%.0Lf' x2='%.0Lf' y2='%.0Lf' />\n",
                    margin, margin,
                    max_x + (margin * 2.), max_y + (margin * 2.),
                    "Star",
                    "EngiceC", "0.1",
                    (offset + mult) + (min_log * mult), max_y - margin,
                    (offset + mult) + (max_log * mult), max_y - margin);

            for (x = floor; x <= ceiling; x += 1.0)
            {
                float n;

                for (n = 1.; n < 9.9; n++)
                {
                    if ((min_log <= x + log10(n)) && (max_log >= x + log10(n)))
                    {
                        fprintf(file,
                                "    <line x1='%.0Lf' y1='%.0Lf' x2='%.0Lf' y2='%.0Lf' />\n",
                                (offset + mult) + ((x + log10(n)) * mult),
                                max_y - margin - ((n == 1) ? 10 : 5),
                                (offset + mult) + ((x + log10(n)) * mult),
                                max_y - margin + ((n == 1) ? 10 : 5));
                    }
                }
            }

            fprintf(file, "</g>\n\n");

            {
                double min_r_ecosphere = sqrt(innermost_planet->sun->luminosity / 1.51);
                double max_r_ecosphere = sqrt(innermost_planet->sun->luminosity / 0.48);

                fprintf(file,
                        "<line x1='%.0Lf' y1='%.0Lf' x2='%.0Lf' y2='%.0Lf' stroke='blue' stroke-width='1' />\n",
                        (offset + mult) + (log10(innermost_planet->sun->r_ecosphere) * mult), (max_y - margin) - 5,
                        (offset + mult) + (log10(innermost_planet->sun->r_ecosphere) * mult), (max_y - margin) + 5);

                fprintf(file,
                        "<line x1='%.0Lf' y1='%.0Lf' x2='%.0Lf' y2='%.0Lf' stroke='#66c' stroke-width='10' stroke-opacity='0.5' />\n",
                        (offset + mult) + (log10(min_r_ecosphere) * mult), max_y - margin,
                        (offset + mult) + (log10(max_r_ecosphere) * mult), max_y - margin);
            }

            fprintf(file,
                    "<g font-family='Arial' font-size='10' \n"
                    "   font-style='normal' font-weight='normal'\n"
                    "   fill='black' text-anchor='middle'>\n");

            for (x = floor; x <= ceiling; x += 1.0)
            {
                if ((min_log <= x) && (max_log >= x))
                {
                    fprintf(file, "    <text x='%.0Lf' y='120'> %.0f AU </text>\n", (offset + mult) + (x * mult), pow(10.0, x));
                }
            }

            fprintf(file, "\n");

            for (planet = innermost_planet; planet != NULL; planet = planet->nextPlanet)
            {
                double x = (offset + mult) + (log10(planet->a) * mult);
                double r = pow((planet->mass * SUN_MASS_IN_EARTH_MASSES), 1.0 / 3.0) * em_scale;
                double x1 = (offset + mult) + (log10(planet->a * (1. - planet->e)) * mult);
                double x2 = (offset + mult) + (log10(planet->a * (1. + planet->e)) * mult);

                fprintf(file,
                        "    <circle cx='%.0Lf' cy='30' r='%.0Lf' fill='none' stroke='black' stroke-width='1' />\n"
                        "    <line x1='%.0Lf' y1='%.0Lf' x2='%.0Lf' y2='%.0Lf' stroke='black' stroke-width='8' stroke-opacity='0.3'/>\n"
                        "    <text x='%.0Lf' y='%.0Lf'>",
                        x, r,
                        x1, (max_y - margin) - 15, x2, (max_y - margin) - 15,
                        x, max_y - (margin * 2.));

                fprintf(file,
                        (planet->mass * SUN_MASS_IN_EARTH_MASSES) <= 1.0
                            ? "%.2Lf"
                            : (planet->mass * SUN_MASS_IN_EARTH_MASSES) <= 10.0
                                  ? "%.1Lf"
                                  : "%.0Lf",
                        planet->mass * SUN_MASS_IN_EARTH_MASSES);

                fprintf(file, "</text>\n\n");
            }

            fprintf(file,
                    "</g>\n"
                    "</svg>\n");
        }

        fflush(file);
        fclose(file);
    }
}

void acPrintPlanetStats(planetPointer planet, bool isMoon, int counter)
{
    char *spaces = (isMoon ? "\t" : "");

    printf("%s", spaces);
    if (isMoon)
    {
        printf("Moon %d", counter);
    }
    else
    {
        printf("Planet %d", counter);
    }

    printf("\t%s\n", acPlanetTypeToString(planet->planetClass));

    //printf("%s",spaces);
    //printf("%s\n", acPlanetTypeDescription(planet->type));

    if ((int)planet->day == (int)(planet->orbitalPeriod * 24.0))
    {
        printf("%s", spaces);
        printf("Planet is tidally locked with one face to star.\n");
    }
    if (planet->resonantPeriod)
    {
        printf("%s", spaces);
        printf("Planet's rotation is in a resonant spin lock with the star\n");
    }
    printf("%s", spaces);
    printf("Distance from primary star:\t%5.2Lf AU\n", planet->a);
    printf("%s", spaces);
    printf("Mass:\t\t\t\t%5.2Lf Earth masses\n", planet->mass * SUN_MASS_IN_EARTH_MASSES);
    if (!(planet->gas_giant))
    {
        //printf("%s",spaces);
        //printf("Surface gravity:\t\t%5.2Lf Earth Gees\n",planet->surfaceGravity);
        if (planet->surfacePressure / 1000.0 > 0)
        {
            printf("%s", spaces);
            printf("Surface pressure:\t\t%5.2Lf Earth atmospheres", (planet->surfacePressure / 1000.0));
            if ((planet->greenhouseEffect) && (planet->surfacePressure > 0.0))
            {
                printf(" GREENHOUSE EFFECT\n");
            }
            else
            {
                printf("\n");
            }
        }

        printf("%s", spaces);
        printf("Surface temperature:\t\t%5.2Lf degrees Celcius\n", (planet->surfaceTemperature - FREEZING_POINT_OF_WATER));
    }
    printf("%s", spaces);
    printf("Equatorial radius:\t\t%5.2Lf Km\n", planet->radius);
    //printf("%s",spaces);
    //printf("Density:\t\t\t%5.2Lf grams/cc\n",planet->density);
    printf("%s", spaces);
    printf("Eccentricity of orbit:\t\t%5.2Lf\n", planet->e);
    //printf("%s",spaces);
    //printf("Escape Velocity:\t\t%5.2Lf Km/sec\n",planet->escapeVelocity / CM_PER_KM);
    //printf("%s",spaces);
    //printf("Molecular weight retained:\t%5.2Lf and above\n",planet->moleculeWeight);
    //printf("%s",spaces);
    //printf("Surface acceleration:\t\t%5.2Lf cm/sec2\n",planet->surfaceAcceleration);

    printf("%s", spaces);
    printf("Axial tilt:\t\t\t%5.2Lf degrees\n", planet->axialTilt);
    printf("%s", spaces);
    printf("Planetary albedo:\t\t%5.2Lf\n", planet->albedo);
    printf("%s", spaces);
    printf("Length of year:\t\t\t%5.2Lf days\n", planet->orbitalPeriod);
    printf("%s", spaces);
    printf("Length of day:\t\t\t%5.2Lf hours", planet->day);
    if (planet->resonantPeriod)
    {
        printf(" IN RESONANT PERIOD\n");
    }
    else
    {
        printf("\n");
    }
#ifdef VII
    if (!(planet->gas_giant))
    {
        printf("%s", spaces);
        printf("Boiling point of water:\t\t%5.2Lf degrees Celcius\n", (planet->boilPoint - FREEZING_POINT_OF_WATER));

        if (planet->hydrosphere * 100.0 > 0)
        {
            printf("%s", spaces);
            printf("Hydrosphere percentage:\t\t%5.2Lf\n", (planet->hydrosphere * 100.0));
        }
        if (planet->cloudCover * 100 > 0)
        {
            printf("%s", spaces);
            printf("Cloud cover percentage:\t\t%5.2Lf\n", (planet->cloudCover * 100));
        }

        if (planet->iceCover * 100 > 0)
        {
            printf("%s", spaces);
            printf("Ice cover percentage:\t\t%5.2Lf\n", (planet->iceCover * 100));
        }
    }
#endif
    if ((planet->planetClass != tGasGiant) && (planet->planetClass != tSubGasGiant) && (planet->planetClass != tSubSubGasGiant))
    {
        if (planet->gases > 0)
        {
            int i;
            bool first = true;
            unsigned int temp;

            printf("%s", spaces);
            printf("Gases in atmosphere:\t\t");

            for (i = 0; i < planet->gases; i++)
            {
                int n;
                int index = max_gas;

                for (n = 0; n < max_gas; n++)
                {
                    if (gases[n].num == planet->atmosphere[i].num)
                        index = n;
                }

                if ((planet->atmosphere[i].surf_pressure / planet->surfacePressure) > .001)
                {
                    printf("%s", spaces);
                    if (i > 0)
                    {
                        printf(", ");
                    }
                    printf("%s %3.3f Atm", gases[index].name, (planet->atmosphere[i].surf_pressure / 1000.0));
                }
            }
            printf("\n");
        }
    }
    printf("\n\n");
}

void acDisplaySystem(accrete *ac)
{
    planetPointer planetHead = ac->planetHead;

    planetPointer planet;
    planetPointer moon;

    int counter;
    int moonCounter;

    printf("Stargen\n");
    printf("                          SYSTEM  CHARACTERISTICS\n");
    printf("Stellar mass: %4.2Lf solar masses\n", planetHead->sun->mass);
    printf("Stellar luminosity: %4.2Lf\n", planetHead->sun->luminosity);
    printf("Age: %5.3Lf billion years	(%5.3Lf billion left on main sequence)\n", (planetHead->sun->age / 1.0E9), (planetHead->sun->life - planetHead->sun->age) / 1.0E9);
    printf("Habitable ecosphere radius: %3.3Lf AU\n", planetHead->sun->r_ecosphere);
    printf("\n");
    printf("Planets present at:\n");
    for (planet = planetHead, counter = 1; planet != NULL; planet = planet->nextPlanet, counter++)
    {
        printf("%d\t%7.3Lf AU\t%8.3Lf EM\t%c\n",
               counter, planet->a, planet->mass * SUN_MASS_IN_EARTH_MASSES,
               planet->gas_giant ? 'O' : ((planet->greenhouseEffect) && (planet->surfacePressure > 0.0)) ? '+' : ((planet->hydrosphere > .05) && (planet->hydrosphere < .95)) ? '*' : ((planet->mass * SUN_MASS_IN_EARTH_MASSES) > .1) ? 'o' : '.');
    }
    printf("\n\n\n");
    for (planet = planetHead, counter = 1; planet != NULL; planet = planet->nextPlanet, counter++)
    {
        acPrintPlanetStats(planet, false, counter);
        for (moon = planet->first_moon, moonCounter = 1; moon != NULL; moon = moon->nextPlanet, moonCounter++)
        {
            acPrintPlanetStats(moon, true, moonCounter);
        }
    }
    //acCreateSvgFile();
}

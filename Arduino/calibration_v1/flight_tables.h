#include <math.h>

// change to use vectors instead of arrays?

float m430[21] = {0, 9, 18.6, 29.1, 40.4, 52.6, 65.9, 80.3, 96, 113.1, 131.9, 152.5, 175.2, 200.4, 228.6, 260.3, 296.7, 339.2, 390.8, 458.3, 570.1};
float m385[20] = {0, 9, 18.7, 29.3, 40.8, 53.4, 66.9, 81.8, 98.2, 116.1, 135.8, 157.6, 181.9, 209, 239.6, 274.6, 315.3, 364.2, 426.4, 518.4};
float m918[22] = {0, 8.9, 18.5, 28.8, 39.9, 51.8, 64.7, 78.5, 93.5, 109.8, 127.5, 146.8, 167.8, 191, 216.6, 245, 277.1, 313.7, 356.5, 408.5, 477.2, 757.3};
float xm918[28] = {0, 4.6, 9.5, 14.9, 20.6, 26.7, 33.3, 40.4, 48.1, 56.3, 65.2, 74.9, 85.3, 96.6, 108.9, 122.2, 136.7, 152.6, 169.9, 189, 210, 233.4, 259.6, 289.3, 323.4, 363.8, 413.8, 482.4};
float ranges[28] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700};

// interpolation helper function
float interpolate(float y2, float y1, float x2, float x1, float x)
{
    return (y1 + (x - x1) * ((y2 - y1) / (x2 - x1)));
}

float calculate(float position = 0, float *table = 0x0, unsigned int num_elements = 0)
{
    /* guard clauses */
    // if position below or above table range, return range of 0 or range of <max range> respectively
    if (num_elements == 0 || position <= 0)
        return 0; //
    if (position > *(table + num_elements - 1))
        return ranges[num_elements - 1];
    if (table == 0x0)
        return -1; // return -1 so we can see on the LCD if something is wrong - i.e. if range is ever -1

    /* find index of mil value closest to current position */
    int closest_index = 0;
    float difference = __DBL_MAX__;
    // loop through all table elements
    for (unsigned int i = 0; i < num_elements; i++)
    {
        // if the current table element is closer to the position, overwrite closest_index with the current table element's index
        if (abs(*(table + i) - position) < difference)
        {
            difference = abs(*(table + i) - position);
            closest_index = i;
        }
    }

    // range should be overwritten by the following if/else statement, but just in case set to -1. If range is ever -1 we know something is wrong
    float range = -1;

    /* interpolate within table */
    // if position is between the closest angle entry in the table and the previous angle entry in the table, interpolate between the current entry and previous entry
    if ((*(table + closest_index) - position) > 0)
    {
        range = interpolate(ranges[closest_index], ranges[closest_index - 1], *(table + closest_index), *(table + closest_index - 1), position);
    }
    // if position is between the current angle entry in the table and the next angle entry in the table, interpolate between the current entry and next entry
    else if ((*(table + closest_index) - position) < 0)
    {
        range = interpolate(ranges[closest_index + 1], ranges[closest_index], *(table + closest_index + 1), *(table + closest_index), position);
    }
    // otherwise the position must be exactly equal to one of the table mil values
    // in this case, we just take the range value of that index directly
    else
    {
        range = ranges[closest_index];
    }

    return range;
}

// call calculate() with different range tables depending on the table value passed from calibration_v1.ino
float find_range(float position = 0, int tableState = 0)
{
    int range;

    switch (tableState)
    {
    case 0:
        range = calculate(position, m430, (sizeof(m430) / sizeof(m430[0])));
        break;
    case 1:
        range = calculate(position, m385, (sizeof(m385) / sizeof(m385[0])));
        break;
    case 2:
        range = calculate(position, m918, (sizeof(m918) / sizeof(m918[0])));
        break;
    case 3:
        range = calculate(position, xm918, (sizeof(xm918) / sizeof(xm918[0])));
        break;
    default:
        range = -1;
        break;
    }

    return range;
}
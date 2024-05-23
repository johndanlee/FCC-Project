#include <math.h>

// change to use vectors instead of arrays?

float m430[21] = {0, 9, 18.6, 29.1, 40.4, 52.6, 65.9, 80.3, 96, 113.1, 131.9, 152.5, 175.2, 200.4, 228.6, 260.3, 296.7, 339.2, 390.8, 458.3, 570.1};
float m385[20] = {0, 9, 18.7, 29.3, 40.8, 53.4, 66.9, 81.8, 98.2, 116.1, 135.8, 157.6, 181.9, 209, 239.6, 274.6, 315.3, 364.2, 426.4, 518.4};
float m918[22] = {0, 8.9, 18.5, 28.8, 39.9, 51.8, 64.7, 78.5, 93.5, 109.8, 127.5, 146.8, 167.8, 191, 216.6, 245, 277.1, 313.7, 356.5, 408.5, 477.2, 757.3};
float xm918[28] = {0, 4.6, 9.5, 14.9, 20.6, 26.7, 33.3, 40.4, 48.1, 56.3, 65.2, 74.9, 85.3, 96.6, 108.9, 122.2, 136.7, 152.6, 169.9, 189, 210, 233.4, 259.6, 289.3, 323.4, 363.8, 413.8, 482.4};
float ranges[28] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700};

// interpolation helper function
float interpolate(float y2, float y1, float x2, float x1, float x) {
    return (y1 + (x - x1) * ((y2 - y1)/(x2 - x1)));
}

int find_closest_index(float* arr, float pos, unsigned int size) {
    int closest_index = 0;
    float difference = __DBL_MAX__;
    for (unsigned int i = 0; i < size; i++) {

    // Serial.println(difference, 2);

        if (abs(*(arr + i) - pos) < difference) {
            difference = abs(*(arr + i) - pos);
            closest_index = i;
        }
    }
    
    return closest_index;
}

float calculate_range(float position = 0) {

    int num_elements = (sizeof(m430)/sizeof(m430[0]));

    // guard clauses
    // if position below or above table range, return range of 0 or range of <max range> respectively
    if (position <= 0) return 0;
    if (position > m430[num_elements - 1]) return ranges[num_elements - 1];


    int closest_index = find_closest_index(m430, position, num_elements);

    float range = -1;
    // interpolate within table
    if ((m430[closest_index] - position) > 0) {
        range = interpolate(ranges[closest_index], ranges[closest_index - 1], m430[closest_index], m430[closest_index - 1], position);
    }
    else if ((m430[closest_index] - position) < 0)
    {
        range = interpolate(ranges[closest_index + 1], ranges[closest_index], m430[closest_index + 1], m430[closest_index], position);
    } else {
        // will only run when mil measurement is exactly equal to one of the table values
        range = ranges[closest_index];
    }

    return range;
}
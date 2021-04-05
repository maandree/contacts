/* See LICENSE file for copyright and license details. */
#include "common.h"


int
parse_coord(const char *s, double *lat, double *lat_min, double *lat_max, double *lon, double *lon_min, double *lon_max)
{
        int withsign = 0, neg;
	long int tmp;
	double prec;

	errno = 0;

	if (s[0] == ':') {
		*lat = 0;
		*lat_min = -90;
		*lat_max = +90;
	} else {
		prec = 1;
		withsign = (s[0] == '-' || s[0] == '+');
		neg = s[0] == '-';
		s = &s[withsign];
		tmp = 0;
		if (isdigit(s[withsign])) {
			tmp = strtol(s, (void *)&s, 0);
			if (errno || tmp > INT_MAX)
				return -1;
		}
		*lat = (double)tmp;
		if (s[0] == '.') {
			for (s++; isdigit(*s); s++) {
				*lat *= 10;
				*lat += (double)(*s & 15);
				prec *= 10;
			}
			*lat /= prec;
		}
		if (!withsign && (s[0] == 'N' || s[0] == 'S')) {
			neg = s[0] == 'S';
			s = &s[1];
		}
		if (neg)
			*lat = -*lat;
		prec = (1 / prec) / 2;
		*lat_min = *lat - prec;
		*lat_max = *lat + prec;
		if (s[0] != ':')
			return -1;
	}

	s = &s[1];

	if (!s[0]) {
		*lon = 0;
		*lon_min = -180;
		*lon_max = +180;
	} else {
		prec = 1;
		withsign = (s[0] == '-' || s[0] == '+');
		neg = s[0] == '-';
		s = &s[withsign];
		tmp = 0;
		if (isdigit(s[withsign])) {
			tmp = strtol(s, (void *)&s, 0);
			if (errno || tmp > INT_MAX)
				return -1;
		}
		*lon = (double)tmp;
		if (s[0] == '.') {
			for (s++; isdigit(*s); s++) {
				*lon *= 10;
				*lon += (double)(*s & 15);
				prec *= 10;
			}
			*lon /= prec;
		}
		if (!withsign && (s[0] == 'E' || s[0] == 'W')) {
			neg = s[0] == 'W';
			s = &s[1];
		}
		if (neg)
			*lon = -*lon;
		prec = (1 / prec) / 2;
		*lon_min = *lon - prec;
		*lon_max = *lon + prec;
		if (s[0])
			return -1;
	}

	return 0;
}

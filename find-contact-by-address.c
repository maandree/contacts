/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address] [-c context] [-g [latitude]:[longitude]] [-n country] [-o care-of] [-p post-code] [-t city]");


int
main(int argc, char *argv[])
{
	char *lookup_addr = NULL, *lookup_ctx = NULL, *lookup_loc = NULL;
	char *lookup_country = NULL, *lookup_careof = NULL;
	char *lookup_postcode = NULL, *lookup_city = NULL;
	double lat, lon, lat_min = 0, lat_max = 0, lon_min = 0, lon_max = 0;
	struct passwd *user;
	struct libcontacts_contact **contacts;
	struct libcontacts_address **addresses, *addr;
	size_t i;

	ARGBEGIN {
	case 'c':
		if (lookup_ctx)
			usage();
		lookup_ctx = ARG();
		break;
	case 'o':
		if (lookup_careof)
			usage();
		lookup_careof = ARG();
		break;
	case 'a':
		if (lookup_addr)
			usage();
		lookup_addr = ARG();
		break;
	case 'p':
		if (lookup_postcode)
			usage();
		lookup_postcode = ARG();
		break;
	case 't':
		if (lookup_city)
			usage();
		lookup_city = ARG();
		break;
	case 'n':
		if (lookup_country)
			usage();
		lookup_country = ARG();
		break;
	case 'g':
		if (lookup_loc)
			usage();
		lookup_loc = ARG();
		if (parse_coord(lookup_loc, &lat, &lat_min, &lat_max, &lon, &lon_min, &lon_max))
			usage();
		break;
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contacts(&contacts, user))
		eprintf("libcontacts_load_contacts:");
	for (i = 0; contacts[i]; i++) {
		if ((addresses = contacts[i]->addresses)) {
			for (; (addr = *addresses); addresses++) {
				if (lookup_ctx && strcmpnul(addr->context, lookup_ctx))
					continue;
				if (lookup_careof && strcmpnul(addr->care_of, lookup_careof))
					continue;
				if (lookup_addr && strcmpnul(addr->address, lookup_addr))
					continue;
				if (lookup_postcode && strcmpnul(addr->postcode, lookup_postcode))
					continue;
				if (lookup_city && strcmpnul(addr->city, lookup_city))
					continue;
				if (lookup_country && strcmpnul(addr->country, lookup_country))
					continue;
				if (lookup_loc && !addr->have_coordinates)
					continue;
				lat = addr->latitude;
				lon = addr->longitude;
				if (lookup_loc && !(lat_min <= lat && lat <= lat_max && lon_min <= lon && lon <= lon_max))
					continue;
				if (!addr->context && !addr->care_of && !addr->address && !addr->postcode && !addr->city &&
				    !addr->country && !addr->have_coordinates)
					continue;
				printf("%s\n", contacts[i]->id);
			}
		}
		libcontacts_contact_destroy(contacts[i]);
		free(contacts[i]);
	}
	free(contacts);

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");

	return 0;
}

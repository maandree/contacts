/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address] [-c context] [-g [latitude]:[longitude]] [-l country] "
      "[-o care-of] [-p post-code] [-t city] [-ACGLOPT] contact-id ...");


int
main(int argc, char *argv[])
{
	int fields = 0;
	struct passwd *user;
	struct libcontacts_contact contact;
	struct libcontacts_address **addresses, *addr;
	char *lookup_addr = NULL, *lookup_ctx = NULL, *lookup_loc = NULL;
	char *lookup_country = NULL, *lookup_careof = NULL;
	char *lookup_postcode = NULL, *lookup_city = NULL;
	double lat, lon, lat_min = 0, lat_max = 0, lon_min = 0, lon_max = 0;
	int ret = 0, f;
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
	case 'l':
		if (lookup_country)
			usage();
		lookup_country = ARG();
		break;
	case 'g':
		if (lookup_loc)
			usage();
		lookup_loc = ARG();
		if (parse_coord(lookup_loc, &lat_min, &lat_max, &lon_min, &lon_max))
			usage();
		break;
	case 'C':
		fields |= 0x01;
		break;
	case 'O':
		fields |= 0x02;
		break;
	case 'A':
		fields |= 0x04;
		break;
	case 'P':
		fields |= 0x08;
		break;
	case 'T':
		fields |= 0x10;
		break;
	case 'L':
		fields |= 0x20;
		break;
	case 'G':
		fields |= 0x40;
		break;
	default:
		usage();
	} ARGEND;

	if (!argc)
		usage();
	
	if (!fields)
		fields = 0x7F;

	for (i = 0; argv[i]; i++)
		if (!*argv[i] || strchr(argv[i], '/'))
			usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	for (; *argv; argv++) {
		if (libcontacts_load_contact(*argv, &contact, user)) {
			weprintf("libcontacts_load_contact %s: %s\n", *argv, errno ? strerror(errno) : "contact file is malformatted");
			ret = 1;
			continue;
		}
		if ((addresses = contact.addresses)) {
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
				if (argc > 1)
					printf("%s: ", *argv);
				f = fields;
				if (f & 0x01) {
					f ^= 0x01;
					if (addr->context)
						printf("%s: ", addr->context);
				}
				if (f & 0x02) {
					f ^= 0x02;
					printf("%s%s", addr->care_of  ? addr->care_of  : "", f ? "; " : "\n");
				}
				if (f & 0x04) {
					f ^= 0x04;
					printf("%s%s", addr->address  ? addr->address  : "", f ? "; " : "\n");
				}
				if (f & 0x08) {
					f ^= 0x08;
					printf("%s%s", addr->postcode ? addr->postcode : "", f ? "; " : "\n");
				}
				if (f & 0x10) {
					f ^= 0x10;
					printf("%s%s", addr->city     ? addr->city     : "", f ? "; " : "\n");
				}
				if (f & 0x20) {
					f ^= 0x20;
					printf("%s%s", addr->country  ? addr->country  : "", f ? "; " : "\n");
				}
				if (f & 0x40) {
					if (addr->have_coordinates) {
						printf("%lg%s ", fabs(lat), lat < 0 ? "S" : lat > 0 ? "N" : "");
						printf("%lg%s",  fabs(lon), lon < 0 ? "W" : lon > 0 ? "E" : "");
					}
					printf("\n");
				}
			}
		}
		libcontacts_contact_destroy(&contact);
	}

	if (fflush(stdout) || ferror(stdout) || fclose(stdout))
		eprintf("printf:");
	return ret;
}

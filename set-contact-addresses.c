/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-A address] [-a address] [-C context] [-c context] [-G [latitude]:[longitude]] [-g latitude:longitude] "
      "[-N country] [-n country] [-O care-of] [-o care-of] [-P post-code] [-p post-code] [-T city] [-t city] "
      "[-u] contact-id");


static int
size_t_cmp(const void *av, const void *bv)
{
	const size_t *ap = av, *bp = bv;
	return *ap < *bp ? -1 : *ap > *bp;
}

int
main(int argc, char *argv[])
{
	int add = 1, edit = 0, remove = 0;
	const char *address = NULL, *context = NULL, *location = NULL;
	const char *country = NULL, *careof = NULL, *postcode = NULL, *city = NULL;
	const char *lookup_address = NULL, *lookup_context = NULL, *lookup_location = NULL;
	const char *lookup_country = NULL, *lookup_careof = NULL, *lookup_postcode = NULL, *lookup_city = NULL;
	double lat, lon, lat_min = 0, lat_max = 0, lon_min = 0, lon_max = 0, flat, flon;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t i, *indices = NULL, nindices, naddresses;
	char *p;

	ARGBEGIN {
	case 'C':
		add = 0;
		if (lookup_context)
			usage();
		lookup_context = ARG();
		break;
	case 'c':
		edit = 1;
		if (context)
			usage();
		context = ARG();
		break;
	case 'O':
		add = 0;
		if (lookup_careof)
			usage();
		lookup_careof = ARG();
		break;
	case 'o':
		add = 0;
		edit = 1;
		if (careof)
			usage();
		careof = ARG();
		break;
	case 'A':
		add = 0;
		if (lookup_address)
			usage();
		lookup_address = ARG();
		break;
	case 'a':
		edit = 1;
		if (address)
			usage();
		address = ARG();
		break;
	case 'P':
		add = 0;
		if (lookup_postcode)
			usage();
		lookup_postcode = ARG();
		break;
	case 'p':
		edit = 1;
		if (postcode)
			usage();
		postcode = ARG();
		break;
	case 'T':
		add = 0;
		if (lookup_city)
			usage();
		lookup_city = ARG();
		break;
	case 't':
		edit = 1;
		if (city)
			usage();
		city = ARG();
		break;
	case 'N':
		add = 0;
		if (lookup_country)
			usage();
		lookup_country = ARG();
		break;
	case 'n':
		edit = 1;
		if (country)
			usage();
		country = ARG();
		break;
	case 'G':
		add = 0;
		if (lookup_location)
			usage();
		lookup_location = ARG();
		break;
	case 'g':
		edit = 1;
		if (location)
			usage();
		location = ARG();
		p = strchr(location, ':');
		if (!p || !p[1] || p == location)
			usage();
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (location && parse_coord(location, &lat, &lat_min, &lat_max, &lon, &lon_min, &lon_max))
		usage();
	if (lookup_location && parse_coord(lookup_location, &flat, &lat_min, &lat_max, &flon, &lon_min, &lon_max))
		usage();

	if (remove == edit) {
		if (edit)
			eprintf("-u cannot be combined with -acglopt\n");
		eprintf("at least one of -acgloptu is required\n");
	}

	if (add)
		edit = 0;

	if (argc != 1 || !*argv[0] || strchr(argv[0], '/'))
		usage();

	errno = 0;
	user = getpwuid(getuid());
	if (!user)
		eprintf("getpwuid: %s\n", errno ? strerror(errno) : "user does not exist");

	if (libcontacts_load_contact(argv[0], &contact, user))
		eprintf("libcontacts_load_contact %s: %s\n", argv[0], errno ? strerror(errno) : "contact file is malformatted");

	naddresses = 0;
	if (contact.addresses)
		for (; contact.addresses[naddresses]; naddresses++);

	if ((edit || remove) && naddresses) {
		nindices = naddresses;
		indices = emalloc(nindices * sizeof(*contact.addresses));
		for (i = 0; i < nindices; i++)
			indices[i] = i;

		if (lookup_context)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->context, lookup_context))
					indices[--i] = indices[--nindices];
		if (lookup_careof)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->care_of, lookup_careof))
					indices[--i] = indices[--nindices];
		if (lookup_address)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->address, lookup_address))
					indices[--i] = indices[--nindices];
		if (lookup_postcode)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->postcode, lookup_postcode))
					indices[--i] = indices[--nindices];
		if (lookup_city)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->city, lookup_city))
					indices[--i] = indices[--nindices];
		if (lookup_country)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->country, lookup_country))
					indices[--i] = indices[--nindices];
		if (lookup_location) {
			for (i = 0; i < nindices;) {
				if (!contact.addresses[indices[i++]]->have_coordinates) {
					flat = contact.addresses[indices[i - 1]]->latitude;
					flon = contact.addresses[indices[i - 1]]->longitude;
					if (!(lat_min <= flat && flat <= lat_max && lon_min <= flon && flon <= lon_max))
						indices[--i] = indices[--nindices];
				}
			}
		}

		if (edit) {
			while (nindices--) {
				i = indices[nindices];
				if (context) {
					free(contact.addresses[i]->context);
					contact.addresses[i]->context = estrdup(context);
				}
				if (careof) {
					free(contact.addresses[i]->care_of);
					contact.addresses[i]->care_of = estrdup(careof);
				}
				if (address) {
					free(contact.addresses[i]->address);
					contact.addresses[i]->address = estrdup(address);
				}
				if (postcode) {
					free(contact.addresses[i]->postcode);
					contact.addresses[i]->postcode = estrdup(postcode);
				}
				if (city) {
					free(contact.addresses[i]->city);
					contact.addresses[i]->city = estrdup(city);
				}
				if (country) {
					free(contact.addresses[i]->country);
					contact.addresses[i]->country = estrdup(country);
				}
				if (location) {
					contact.addresses[i]->latitude = lat;
					contact.addresses[i]->longitude = lon;
					contact.addresses[i]->have_coordinates = 1;
				}
			}
		} else {
			qsort(indices, nindices, sizeof(*indices), size_t_cmp);
			while (nindices--) { /* reverse order is important */
				i = indices[nindices];
				libcontacts_address_destroy(contact.addresses[i]);
				free(contact.addresses[i]);
				/* we using memmove for simplity as we only expect to remove one entry */
				memmove(&contact.addresses[i], &contact.addresses[i + 1],
				        (naddresses-- - i) * sizeof(*contact.addresses));
			}
		}
	} else if (!edit && !remove) {
		i = naddresses;
		contact.addresses = erealloc(contact.addresses, (i + 2) * sizeof(*contact.addresses));
		contact.addresses[i + 1] = NULL;
		contact.addresses[i] = ecalloc(1, sizeof(**contact.addresses));
		contact.addresses[i]->context  = context  ? estrdup(context)  : NULL;
		contact.addresses[i]->care_of  = careof   ? estrdup(careof)   : NULL;
		contact.addresses[i]->address  = address  ? estrdup(address)  : NULL;
		contact.addresses[i]->postcode = postcode ? estrdup(postcode) : NULL;
		contact.addresses[i]->city     = city     ? estrdup(city)     : NULL;
		contact.addresses[i]->country  = country  ? estrdup(country)  : NULL;
		if (location) {
			contact.addresses[i]->latitude = lat;
			contact.addresses[i]->longitude = lon;
			contact.addresses[i]->have_coordinates = 1;
		}
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);
	free(indices);

	return 0;
}

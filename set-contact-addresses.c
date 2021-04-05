/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("[-a address | -A address] [-c context | -C context] [-g [latitude]:[longitude] | -G latitude:longitude] "
      "[-n country | -N country] [-o care-of | -O care-of] [-p post-code | -P post-code] [-t city | -T city] "
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
	int edit_address = 0, edit_context = 0, edit_location = 0;
	int edit_country = 0, edit_careof = 0, edit_postcode = 0;
	int edit_city = 0, edit = 0, remove = 0;
	const char *address = NULL, *context = NULL, *location = NULL;
	const char *country = NULL, *careof = NULL, *postcode = NULL, *city = NULL;
	double lat, lon, lat_min = 0, lat_max = 0, lon_min = 0, lon_max = 0, flat, flon;
	struct passwd *user;
	struct libcontacts_contact contact;
	size_t i, *indices = NULL, nindices, naddresses;
	char *p;

	ARGBEGIN {
	case 'C':
		edit_context = 1;
		edit = 1;
		/* fall through */
	case 'c':
		if (context)
			usage();
		context = ARG();
		break;
	case 'O':
		edit_careof = 1;
		edit = 1;
		/* fall through */
	case 'o':
		if (careof)
			usage();
		careof = ARG();
		break;
	case 'A':
		edit_address = 1;
		edit = 1;
		/* fall through */
	case 'a':
		if (address)
			usage();
		address = ARG();
		break;
	case 'P':
		edit_postcode = 1;
		edit = 1;
		/* fall through */
	case 'p':
		if (postcode)
			usage();
		postcode = ARG();
		break;
	case 'T':
		edit_city = 1;
		edit = 1;
		/* fall through */
	case 't':
		if (city)
			usage();
		city = ARG();
		break;
	case 'N':
		edit_country = 1;
		edit = 1;
		/* fall through */
	case 'n':
		if (country)
			usage();
		country = ARG();
		break;
	case 'G':
		edit_location = 1;
		edit = 1;
		/* fall through */
	case 'g':
		if (location)
			usage();
		location = ARG();
		if (edit_location) {
			p = strchr(location, ':');
			if (!p || !p[1] || p == location)
				usage();
		}
		if (parse_coord(location, &lat, &lat_min, &lat_max, &lon, &lon_min, &lon_max))
			usage();
		break;
	case 'u':
		remove = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (remove == edit) {
		if (edit)
			eprintf("-u cannot be combined with -ACGLOPT\n");
		eprintf("at least one of -ACGLOPTu is required\n");
	}

	if ((!context  || edit_context) && (!careof   || edit_careof)   &&
	    (!address  || edit_address) && (!postcode || edit_postcode) &&
	    (!city     || edit_city)    && (!country  || edit_country)  &&
	    (!location || edit_location))
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

		if (context && !edit_context)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->context, context))
					indices[--i] = indices[--nindices];
		if (careof && !edit_careof)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->care_of, careof))
					indices[--i] = indices[--nindices];
		if (address && !edit_address)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->address, address))
					indices[--i] = indices[--nindices];
		if (postcode && !edit_postcode)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->postcode, postcode))
					indices[--i] = indices[--nindices];
		if (city && !edit_city)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->city, city))
					indices[--i] = indices[--nindices];
		if (country && !edit_country)
			for (i = 0; i < nindices;)
				if (strcmpnul(contact.addresses[indices[i++]]->country, country))
					indices[--i] = indices[--nindices];
		if (location && !edit_location) {
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
				if (edit_context) {
					free(contact.addresses[i]->context);
					contact.addresses[i]->context = estrdup(context);
				}
				if (edit_careof) {
					free(contact.addresses[i]->care_of);
					contact.addresses[i]->care_of = estrdup(careof);
				}
				if (edit_address) {
					free(contact.addresses[i]->address);
					contact.addresses[i]->address = estrdup(address);
				}
				if (edit_postcode) {
					free(contact.addresses[i]->postcode);
					contact.addresses[i]->postcode = estrdup(postcode);
				}
				if (edit_city) {
					free(contact.addresses[i]->city);
					contact.addresses[i]->city = estrdup(city);
				}
				if (edit_country) {
					free(contact.addresses[i]->country);
					contact.addresses[i]->country = estrdup(country);
				}
				if (edit_location) {
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
			contact.addresses[i]->have_coordinates = 1;
			contact.addresses[i]->latitude = lat;
			contact.addresses[i]->longitude = lon;
		}
	}

	if (libcontacts_save_contact(&contact, user))
		eprintf("libcontacts_save_contact %s:", argv[0]);
	libcontacts_contact_destroy(&contact);
	free(indices);

	return 0;
}

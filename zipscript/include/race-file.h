#ifndef _RACE_FILE_H_
#define _RACE_FILE_H_

#include <sys/stat.h>
#include "objects.h"
#include "zsfunctions.h"

/* this is what we write to racedata files */
typedef struct {
	unsigned int	crc32,
			speed; /* should become unsigned long, but causes
				* backwards incompatibility; might be overcome
				* via sfv_version checking when using readrace()
				*/
	off_t		size;
	time_t		start_time;
	unsigned char	status;
	unsigned char	dummy1;
	char		fname[NAMEMAX],
			uname[24],
			group[24],
			dummy2[31], /* not used anymore, */
			dummy3[31]; /* kept for compatibilty */
} RACEDATA;

/* this is put in sfvdata files */
typedef struct {
	unsigned int	crc32;
	char		fname[NAMEMAX];
} SFVDATA;

/* this is what we put in a special 'head' file for version control, lock etc */
typedef struct {
	unsigned int	data_version,		// version control.
			data_type,		// type of release.
			data_in_use,		// which program currently holds the lock.
			data_incrementor,	// a check to see if nothing else wants the lock.
			data_queue,		// positions in queue.
			data_qcurrent,		// current position in queue.
			data_pid,		// the pid of the process holding the lock.
			data_completed;		// flag to mark release as complete.
} HEADDATA;

extern unsigned int readsfv(const char *, struct VARS *, int);
extern char *get_first_filename_from_sfvdata(const char *);
extern int parse_sfv(char *, GLOBAL *, DIR *);
extern void update_sfvdata(const char *, const char *, const unsigned int);
extern void delete_sfv(const char *, struct VARS *);
extern void readrace(const char *, struct VARS *, struct USERINFO **, struct GROUPINFO **);
extern void maketempdir(char *);
extern void read_write_leader(const char *, struct VARS *, struct USERINFO *);
extern void testfiles(struct LOCATIONS *, struct VARS *, int);
extern int copysfv(const char *, const char *, struct VARS *);
extern void create_indexfile(const char *, struct VARS *, char *);
extern short clear_file(const char *, char *);
extern void writerace(const char *, struct VARS *, unsigned int, unsigned char);
extern void remove_from_race(const char *, const char *, struct VARS *);
extern int verify_racedata(const char *, struct VARS *);
extern int create_lock(struct VARS *, const char *, unsigned int, unsigned int, unsigned int);
extern void remove_lock(struct VARS *);
extern int update_lock(struct VARS *, unsigned int, unsigned int);
extern short match_file(char *,	char *);
extern int check_rarfile(const char *);
extern int check_zipfile(const char *, const char *, int);
extern void removedir(const char *);
extern void create_dirlist(const char *, char *, const int);
extern int filebanned_match(const char *);
extern int lenient_compare(char *, char *);
extern int read_headdata(const char *);
#endif


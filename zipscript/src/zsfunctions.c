#include "zsfunctions.h"
#include "constants.h"
#include "convert.h"

struct dirent	**dirlist;
unsigned int direntries = 0;

#if ( debug_mode == TRUE )
void d_log(char *fmt, ...) {
 time_t		timenow;
 FILE		*file;
 va_list	ap;

 va_start(ap, fmt);
 timenow = time(NULL);

 if ((file = fopen(".debug", "a+")) != NULL ) {
	fprintf(file, "%.24s - ", ctime(&timenow));
	vfprintf(file, fmt, ap);
	fclose(file);
	}
}
#else 
void __inline__ d_log(char *fmt, ...) {  }
#endif

void create_missing(char *f, short int l) {
 char	*fname;
 
 fname = m_alloc(l + 9);
 memcpy(fname, f, l);
 memcpy(fname + l, "-missing", 9);
 createzerofile(fname);
 m_free(fname);
}


/*
 * Modified: 01.16.2002
 */
char* findfileext(char *fileext) { 
 int	n, k;
 
 n = direntries;
 while(n--) {
	if ((k = NAMLEN(dirlist[n])) < 4 ) continue;
  	if ( strcasecmp(dirlist[n]->d_name + k - 4, fileext) == 0 ) return dirlist[n]->d_name;
	}
 return NULL;
}



/*
 * Modified: 11.12.2003 (daxxar)
 */
int findfileextcount(char *fileext) { 
 int	n, k, c = 0;
 
 n = direntries;
 while(n--) {
	if ((k = NAMLEN(dirlist[n])) < 4 ) continue;
  	if ( strcasecmp(dirlist[n]->d_name + k - 4, fileext) == 0 ) c++;
	}
 return c;
}



/*
 * Modified: 01.16.2002
 */
unsigned int hexstrtodec(unsigned char *s) {
 unsigned int  n = 0;
 unsigned char  r;

 while ( 1 ) {
	if ((unsigned char)*s >= 48 && (unsigned char)*s <= 57) {
		r = 48;
		} else if ((unsigned char)*s >= 65 && (unsigned char)*s <= 70 ) {
		r = 55;
		} else if ((unsigned char)*s >= 97 && (unsigned char)*s <= 102 ) {
		r = 87;
		} else if ((unsigned char)*s == 0 ) {
		return n;
		} else {
		return 0;
		}
	n <<= 4;
	n += *s++ - r;
	}
}

/*
 * dangling links
 */
#if defined(__linux__)
int selector (const struct dirent *d) {
#elif defined(__NetBSD__)
int selector (const struct dirent *d) {
#else
int selector (struct dirent *d) {
#endif
        struct stat st;
        if ((stat(d->d_name, &st) < 0)) return 0;
        return 1;
}

#if defined(__linux__)
int selector2 (const struct dirent *d) {
#elif defined(__NetBSD__)
int selector2 (const struct dirent *d) {
#else
int selector2 (struct dirent *d) {
#endif
        struct stat st;
        if ((stat(d->d_name, &st) < 0) || S_ISDIR(st.st_mode)) return 0;
        return 1;
}


/*
 * Modified: Unknown
 */
void rescandir() {
 if ( direntries > 0 ) {
	while ( direntries-- ) {
		free(dirlist[direntries]);
		}
	free(dirlist);
	}
/*
     int
     scandir(const char *dirname, struct dirent ***namelist,
             int (*select)(const struct dirent *),
             int (*compar)(const void *, const void *));

     int
     alphasort(const void *d1, const void *d2);
*/

 direntries = scandir(".", &dirlist, selector, 0);
}

void rescandir2() {
 if ( direntries > 0 ) {
	while ( direntries-- ) {
		free(dirlist[direntries]);
		}
	free(dirlist);
	}
 direntries = scandir(".", &dirlist, selector2, alphasort);
}


/*
 * Modified: 01.16.2002
 */
void strtolower(char *s) {
 while ( (*s = tolower(*s)) ) s++;
}



/*
 * Modified: 01.16.2002
 */
char israr(char *fileext) {
 if ( (*fileext == 'r' || *fileext =='s' || isdigit(*fileext)) &&
   ((isdigit(*(fileext + 1)) && isdigit(*(fileext + 2))) ||
   (*(fileext + 1) == 'a' && *(fileext + 2) == 'r')) &&
   *(fileext + 3) == 0 ) return 1;
 return 0;
}


/*
 * Created    : 02.20.2002
 * Author     : dark0n3
 *
 * Description: Checks if file is known mpeg/avi file
 *
 *
 */
char isvideo(char *fileext) {

 switch ( *fileext++ ) {
	case 'm':
		if (! memcmp(fileext, "pg", 3) ||
		    ! memcmp(fileext, "peg", 4) ||
		    ! memcmp(fileext, "2v", 3) ||
		    ! memcmp(fileext, "2p", 3)) {
			return 1;
			}
		break;
	case 'a':
		if (! memcmp(fileext, "vi", 3)) {
			return 1;
			}
		break;
	}

 return 0;
}

 

/*
 * Modified: Unknown
 */
void buffer_progress_bar(struct VARS *raceI) {
 int	n;

 raceI->misc.progress_bar[14] = 0;
 if (raceI->total.files > 0) {
   for ( n = 0 ; n < (raceI->total.files - raceI->total.files_missing) * 14 / raceI->total.files ; n++) raceI->misc.progress_bar[n] = '#';
   for ( ; n < 14 ; n++) raceI->misc.progress_bar[n] = ':';
 }
}



/*
 * Modified: 01.16.2002
 */
void move_progress_bar(short int delete, struct VARS *raceI) {
	char		*bar;
	int			n;
	regex_t		preg;
	regmatch_t	pmatch[ 1 ];

	regcomp(&preg, del_progressmeter, REG_NEWLINE|REG_EXTENDED);
	n = direntries;

	if (delete) {
		d_log("Removing progress bar.");
		while (n--) {
			if ( regexec( &preg, dirlist[n]->d_name, 1, pmatch, 0) == 0 ) {
				if ( ! (int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[n]) ) {
					d_log("Found progress bar (%s), removing.", dirlist[n]->d_name);
					remove(dirlist[n]->d_name);
					*dirlist[n]->d_name = 0;
					return;
				}
			}
		}
	} else {
		d_log("Moving (updating) progress bar.");
		if (!raceI->total.files) return;
		bar = convert(raceI, userI, groupI, progressmeter);
		while (n--) {
			if (regexec( &preg, dirlist[n]->d_name, 1, pmatch, 0) == 0) {
				if (!(int)pmatch[0].rm_so && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[n])) {
					d_log("Found progress bar (%s), renaming (to %s).", dirlist[n]->d_name, bar);
					rename(dirlist[n]->d_name, bar);
					return;
				}
			}
		}
		createstatusbar(bar);
	}
}



/*
 * Modified: Unknown
 */
short int findfile(char *filename) {
 int	n = direntries;

 while( n-- ) {
	if ( ! strcasecmp(dirlist[n]->d_name, filename)) {
		return 1;
		}
	}
 return 0;
}



/*
 * Modified: 01.16.2002
 */
void removecomplete() {
 int		n;
 regex_t	preg;
 regmatch_t	pmatch[1];

 unlink(message_file_name);
 regcomp( &preg, del_completebar, REG_NEWLINE|REG_EXTENDED );
 n = direntries;
 while(n--) {
	if(regexec(&preg, dirlist[n]->d_name, 1, pmatch, 0 ) == 0) {
		if ((int)pmatch[0].rm_so == 0 && (int)pmatch[0].rm_eo == (int)NAMLEN(dirlist[n])) {
			remove(dirlist[n]->d_name);
			*dirlist[n]->d_name = 0;
			}
		}
	}
}




/*
 * Modified: 01.16.2002
 */
short int matchpath(char *instr, char *path) {
 int    pos = 0;
 
 do {
	switch ( *instr ) {
		case 0:
		case ' ':
			if (strncmp(instr - pos, path, pos - 1) == 0) {
				return 1;
				}
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	} while (*instr++);
 return 0;
}



/*
 * Modified: 01.16.2002
 */
short int strcomp(char *instr, char *searchstr) {
 int	pos = 0,
	k;

 k = strlen( searchstr );

 do {
	switch ( *instr ) {
		case 0:
		case ',':
			if ( k == pos && ! strncasecmp( instr - pos, searchstr, pos ) ) {
				return 1;
				}
			pos = 0;
			break;
		default:
			pos++;
			break;
		}
	} while ( *instr++ );
 return 0;
}



/* Checks if file exists */
short int fileexists(char *f) {
 if ( close(open(f, O_RDONLY)) == -1 ) return 0;
 return 1;
}



/* Create symbolic link */
void createlink(char *factor1, char *factor2, char *source, char *ltarget) {
#if ( userellink == 1 )
 char	result[MAXPATHLEN];
#endif
 char	*org,
	*target;
 int	l1, l2, l3;

 
 l1 = strlen(factor1) + 1;
 l2 = strlen(factor2) + 1;
 l3 = strlen(ltarget) + 1;

 org = target = malloc(l1 + l2 + l3);

 memcpy(target, factor1, l1);
 target += l1 - 1;
 if (*(target - 1) != '/' ) {
  *(target) = '/'; target += 1;
 }

 memcpy(target, factor2, l2);
 target += l2;
 memcpy(target - 1, "/", 2);

 mkdir(org, 0777);
#if ( userellink == 1 )
 abs2rel(source, org, result, MAXPATHLEN);
#endif

 memcpy(target, ltarget, l3);

#if ( userellink == 1 )
 d_log("new: ln -s %s %s\n", result, org);
 symlink(result, org);
#else
 d_log("old: ln -s %s %s\n", source, org);
 symlink(source, org);
#endif

 m_free(org);
}



void readsfv_ffile(char *filename, off_t buf_bytes ) {
 int    fd,
        line_start = 0,
        index_start,
        ext_start,
        n;
 char   *buf,
        *fname;

 fd  = open( filename, O_RDONLY );
 buf = m_alloc( buf_bytes + 2 );
 read(fd, buf, buf_bytes);
 close( fd );
  
 for ( n = 0 ; n <= buf_bytes; n++ ) {
	if ( buf[n] == '\n' || n == buf_bytes ) {
		index_start = n - line_start;
		if ( buf[line_start] != ';' ) {
			while ( buf[index_start + line_start] != ' ' && index_start-- );
			if ( index_start > 0 ) {
				buf[index_start + line_start] = 0;
				fname = buf + line_start;
				ext_start = index_start;
				while ( (fname[ext_start] = tolower(fname[ext_start])) != '.' && ext_start > 0 ) ext_start--;
				if ( fname[ext_start] != '.' ) {
					ext_start = index_start; 
					} else {
					ext_start++;
					}
				index_start++;
				raceI.total.files++;
				if ( ! strcomp(ignored_types, fname + ext_start) ) {
					if ( findfile(fname) ) { 
						raceI.total.files_missing--;
						}
					}
				}
			}
		line_start = n + 1;
		}
	}
raceI.total.files_missing = raceI.total.files + raceI.total.files_missing;
 m_free(buf);
}



void get_rar_info(char *filename) {
 FILE	*file;

 if ((file = fopen(filename, "r"))) {
   fseek(file, 45, SEEK_CUR);
   fread(&raceI.file.compression_method, 1, 1, file);
   fclose(file);
 }
}



/*
 * Modified   : 02.07.2002
 * Author     : dark0n3
 *
 * Description: Executes extern program and returns return value
 *
 */
int execute(char *s) {
 int    n;
 int	args = 0;
 char	*argv[128]; /* Noone uses this many args anyways */

 argv[0] = s;
 while ( 1 ) {
	if ( *s == ' ' ) {
		*s = 0;
		args++;
		argv[args] = s + 1;
		} else if ( *s == 0 ) {
		args++;
		argv[args] = NULL;
		break;
		}
	s++;
	}

 switch ( fork() ) {
	case 0: 
		close(1);
		close(2);
		n = execv(argv[0], argv);
		exit(0);
		break;
	default:
		wait(&n);
		break;
	}

 return n >> 8;
}
/*
 * Copyright (c) 1997 Shigio Yamaguchi. All rights reserved.
 * Copyright (c) 1999 Tama Communications Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * abs2rel: convert an absolute path name into relative.
 *
 *	i)	path	absolute path
 *	i)	base	base directory (must be absolute path)
 *	o)	result	result buffer
 *	i)	size	size of result buffer
 *	r)		!= NULL: relative path
 *			== NULL: error
 */
char *
abs2rel(path, base, result, size)
	const char *path;
	const char *base;
	char *result;
	const size_t size;
{
	const char *pp, *bp, *branch;
	/*
	 * endp points the last position which is safe in the result buffer.
	 */
	const char *endp = result + size - 1;
	char *rp;

	if (*path != '/') {
		if (strlen(path) >= size)
			goto erange;
		strcpy(result, path);
		goto finish;
	} else if (*base != '/' || !size) {
		errno = EINVAL;
		return (NULL);
	} else if (size == 1)
		goto erange;
	/*
	 * seek to branched point.
	 */
	branch = path;
	for (pp = path, bp = base; *pp && *bp && *pp == *bp; pp++, bp++)
		if (*pp == '/')
			branch = pp;
	if ((*pp == 0 || (*pp == '/' && *(pp + 1) == 0)) &&
	    (*bp == 0 || (*bp == '/' && *(bp + 1) == 0))) {
		rp = result;
		*rp++ = '.';
		if (*pp == '/' || *(pp - 1) == '/')
			*rp++ = '/';
		if (rp > endp)
			goto erange;
		*rp = 0;
		goto finish;
	}
	if ((*pp == 0 && *bp == '/') || (*pp == '/' && *bp == 0))
		branch = pp;
	/*
	 * up to root.
	 */
	rp = result;
	for (bp = base + (branch - path); *bp; bp++)
/* fixed by iwdisb */
		if (*bp == '/' && *(bp + 1) != 0 && *(bp + 1) != '/') {
			if (rp + 3 > endp)
				goto erange;
			*rp++ = '.';
			*rp++ = '.';
			*rp++ = '/';
		}
	if (rp > endp)
		goto erange;
	*rp = 0;
	/*
	 * down to leaf.
	 */
	if (*branch) {
		if (rp + strlen(branch + 1) > endp)
			goto erange;
		strcpy(rp, branch + 1);
	} else
		*--rp = 0;
finish:
	return result;
erange:
	errno = ERANGE;
	return (NULL);
}

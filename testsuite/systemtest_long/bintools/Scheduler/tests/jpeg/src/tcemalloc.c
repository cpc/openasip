#include "tcemalloc.h"

#include <stddef.h>

/* reserve 16M for the heap -- should be enough for most TTA systems */
#ifndef HEAP_SIZE
#define HEAP_SIZE 256*1024
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 16*1024
#endif

static char heap_array[HEAP_SIZE];

static void bcopy(char* src, char* dst, int length) {
	char *end = src + length;
	int *src1, *dst1, *end1;
	short *src2, *dst2, *end2;

	if(length == 0)
		return;

	if(!(length % 4) && !((int) src % 4) && !((int) dst % 4))
	{
		src1 = (int *) src;
		dst1 = (int *) dst;
		end1 = (int *) end;

		do
		{
			*dst1++ = *src1++;
		} while(src1 != end1);
	}
	else if(!(length % 2) && !((int) src % 2) && !((int) dst % 2))
	{
		src2 = (short *) src;
		dst2 = (short *) dst;
		end2 = (short *) end;

		do
		{
			*dst2++ = *src2++;
		} while(src2 != end2);
	}
	else
	{
		do
		{
			*dst++ = *src++;
		} while(src != end);
	}
}

static void morecore();
static int findbucket();

/*
 * The overhead on a block is at least 4 bytes.  When free, this space
 * contains a pointer to the next free block, and the bottom two bits must
 * be zero.  When in use, the first byte is set to MAGIC, and the second
 * byte is the size index.  The remaining bytes are for alignment.
 * If range checking is enabled then a second word holds the size of the
 * requested block, less 1, rounded up to a multiple of sizeof(RMAGIC).
 * The order of elements is critical: ov_magic must overlay the low order
 * bits of ov_next, and ov_magic can not be a valid ov_next bit pattern.
 */
union	overhead {
	union	overhead *ov_next;	/* when free */
	struct {
		u_char	ovu_magic;	/* magic number */
		u_char	ovu_index;	/* bucket # */
#ifdef RCHECK
		u_short	ovu_rmagic;	/* range magic number */
		u_int	ovu_size;	/* actual block size */
#endif
	} ovu;
#define	ov_magic	ovu.ovu_magic
#define	ov_index	ovu.ovu_index
#define	ov_rmagic	ovu.ovu_rmagic
#define	ov_size		ovu.ovu_size
};

#define	MAGIC		0xef		/* magic # on accounting info */
#define RMAGIC		0x5555		/* magic # on range info */

#ifdef RCHECK
#define	RSLOP		sizeof (u_short)
#else
#define	RSLOP		0
#endif

/*
 * nextf[i] is the pointer to the next free block of size 2^(i+3).  The
 * smallest allocatable block is 8 bytes.  The overhead information
 * precedes the data area returned to the user.
 */
#define	NBUCKETS 30
static	union overhead *nextf[NBUCKETS];

static	int pagesz;			/* page size */
static	int pagebucket;			/* page size bucket */

static char* sbrk(int incr){
  static char* heap_end = heap_array;
  char* prev_heap_end;
 
  prev_heap_end = heap_end;
  heap_end += incr;
  return prev_heap_end;
}

static int getpagesize() {
    return PAGE_SIZE;
}

void* malloc(size_t nbytes) {
  	register union overhead *op;
  	register int bucket, n;
	register unsigned amt;

	/*
	 * First time malloc is called, setup page size and
	 * align break pointer so all data will be page aligned.
	 */
	if (pagesz == 0) {
		pagesz = n = getpagesize();
		op = (union overhead *)sbrk(0);
  		n = n - sizeof (*op) - ((int)op & (n - 1));
		if (n < 0)
			n += pagesz;
  		if (n) {
  			if (sbrk(n) == (char *)-1)
				return (NULL);
		}
		bucket = 0;
		amt = 8;
		while (pagesz > amt) {
			amt <<= 1;
			bucket++;
		}
		pagebucket = bucket;
	}
	/*
	 * Convert amount of memory requested into closest block size
	 * stored in hash buckets which satisfies request.
	 * Account for space used per block for accounting.
	 */
	if (nbytes <= (n = pagesz - sizeof (*op) - RSLOP)) {
#ifndef RCHECK
		amt = 8;	/* size of first bucket */
		bucket = 0;
#else
		amt = 16;	/* size of first bucket */
		bucket = 1;
#endif
		n = -(sizeof (*op) + RSLOP);
	} else {
		amt = pagesz;
		bucket = pagebucket;
	}
	while (nbytes > amt + n) {
		amt <<= 1;
		if (amt == 0)
			return (NULL);
		bucket++;
	}
	/*
	 * If nothing in hash bucket right now,
	 * request more memory from the system.
	 */
  	if ((op = nextf[bucket]) == NULL) {
  		morecore(bucket);
  		if ((op = nextf[bucket]) == NULL)
  			return (NULL);
	}
	/* remove from linked list */
  	nextf[bucket] = op->ov_next;
	op->ov_magic = MAGIC;
	op->ov_index = bucket;
#ifdef RCHECK
	/*
	 * Record allocated size of block and
	 * bound space with magic numbers.
	 */
	op->ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
	op->ov_rmagic = RMAGIC;
  	*(u_short *)((caddr_t)(op + 1) + op->ov_size) = RMAGIC;
#endif
  	return ((char *)(op + 1));
}

/*
 * Allocate more memory to the indicated bucket.
 */
static void morecore(int bucket) {
  	register union overhead *op;
	register int sz;		/* size of desired block */
  	int amt;			/* amount to allocate */
  	int nblks;			/* how many blocks we get */

	/*
	 * sbrk_size <= 0 only for big, FLUFFY, requests (about
	 * 2^30 bytes on a VAX, I think) or for a negative arg.
	 */
	sz = 1 << (bucket + 3);
	if (sz <= 0)
		return;
	if (sz < pagesz) {
		amt = pagesz;
  		nblks = amt / sz;
	} else {
		amt = sz + pagesz;
		nblks = 1;
	}
	op = (union overhead *)sbrk(amt);
	/* no more room! */
  	if ((int)op == -1)
  		return;
	/*
	 * Add new memory allocated to that on
	 * free list for this hash bucket.
	 */
  	nextf[bucket] = op;
  	while (--nblks > 0) {
		op->ov_next = (union overhead *)((caddr_t)op + sz);
		op = (union overhead *)((caddr_t)op + sz);
  	}
}

void free(void* cp) {   
  	register int size;
	register union overhead *op;

  	if (cp == NULL)
  		return;
	op = (union overhead *)((caddr_t)cp - sizeof (union overhead));
	if (op->ov_magic != MAGIC)
		return;				/* sanity */
  	size = op->ov_index;
	op->ov_next = nextf[size];	/* also clobbers ov_magic */
  	nextf[size] = op;
}

/*
 * When a program attempts "storage compaction" as mentioned in the
 * old malloc man page, it realloc's an already freed block.  Usually
 * this is the last block it freed; occasionally it might be farther
 * back.  We have to search all the free lists for the block in order
 * to determine its bucket: 1st we make one pass thru the lists
 * checking only the first block in each; if that fails we search
 * ``realloc_srchlen'' blocks in each list for a match (the variable
 * is extern so the caller can modify it).  If that fails we just copy
 * however many bytes was given to realloc() and hope it's not huge.
 */
int realloc_srchlen = 4;	/* 4 should be plenty, -1 =>'s whole list */

void* realloc(void* cp, size_t nbytes) {   
  	register u_int onb;
	register int i;
	union overhead *op;
  	char *res;
	int was_alloced = 0;

  	if (cp == NULL)
  		return (malloc(nbytes));
	op = (union overhead *)((caddr_t)cp - sizeof (union overhead));
	if (op->ov_magic == MAGIC) {
		was_alloced++;
		i = op->ov_index;
	} else {
		/*
		 * Already free, doing "compaction".
		 *
		 * Search for the old block of memory on the
		 * free list.  First, check the most common
		 * case (last element free'd), then (this failing)
		 * the last ``realloc_srchlen'' items free'd.
		 * If all lookups fail, then assume the size of
		 * the memory block being realloc'd is the
		 * largest possible (so that all "nbytes" of new
		 * memory are copied into).  Note that this could cause
		 * a memory fault if the old area was tiny, and the moon
		 * is gibbous.  However, that is very unlikely.
		 */
		if ((i = findbucket(op, 1)) < 0 &&
		    (i = findbucket(op, realloc_srchlen)) < 0)
			i = NBUCKETS;
	}
	onb = 1 << (i + 3);
	if (onb < pagesz)
		onb -= sizeof (*op) + RSLOP;
	else
		onb += pagesz - sizeof (*op) - RSLOP;
	/* avoid the copy if same size block */
	if (was_alloced) {
		if (i) {
			i = 1 << (i + 2);
			if (i < pagesz)
				i -= sizeof (*op) + RSLOP;
			else
				i += pagesz - sizeof (*op) - RSLOP;
		}
		if (nbytes <= onb && nbytes > i) {
#ifdef RCHECK
			op->ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
			*(u_short *)((caddr_t)(op + 1) + op->ov_size) = RMAGIC;
#endif
			return(cp);
		} else
			free(cp);
	}
  	if ((res = malloc(nbytes)) == NULL)
  		return (NULL);
  	if (cp != res)		/* common optimization if "compacting" */
		bcopy(cp, res, (nbytes < onb) ? nbytes : onb);
  	return (res);
}

/*
 * Search ``srchlen'' elements of each free list for a block whose
 * header starts at ``freep''.  If srchlen is -1 search the whole list.
 * Return bucket number, or -1 if not found.
 */
static int findbucket(union overhead* freep, int srchlen) {
	register union overhead *p;
	register int i, j;

	for (i = 0; i < NBUCKETS; i++) {
		j = 0;
		for (p = nextf[i]; p && j != srchlen; p = p->ov_next) {
			if (p == freep)
				return (i);
			j++;
		}
	}
	return (-1);
}

/*
 * bzero -- vax movc5 instruction
 */
static void bzero(void* b, size_t length) {
    char* p;
	for (p = b; length--;)
		*p++ = '\0';
}

void* calloc(size_t num, size_t size) {
	register void *p;

	size *= num;
	if ((p = malloc(size)))
		bzero(p, size);
	return(p);
}

